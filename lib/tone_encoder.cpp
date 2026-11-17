#include "tone_encoder.h"
#include "gpio_interrupts.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/sync.h"
#include <cstdint>
#include <limits>

namespace {
constexpr unsigned int A_GPIO = 2;
constexpr unsigned int B_GPIO = 3;
constexpr unsigned int BUTTON_GPIO = 6; // Previously spare; switch to GND.
constexpr std::uint64_t DEBOUNCE_US = 30000;
constexpr std::uint32_t EDGES = GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL;
bool initialized = false;
unsigned int previousAB = 0;
int quarterSteps = 0;
volatile int pendingSteps = 0;
volatile bool rawPressed = false;
volatile bool stablePressed = false;
volatile std::uint64_t buttonChangedAt = 0;
volatile unsigned int pendingPresses = 0;

unsigned int readAB() {
    const auto pins = gpio_get_all(); // Read both contacts in one snapshot.
    return ((pins >> A_GPIO) & 1u) * 2u | ((pins >> B_GPIO) & 1u);
}

// Called from the ISR or with interrupts disabled in the main loop. Settling
// the preceding level on the next edge captures a complete click between polls.
void settleButton(std::uint64_t now) {
    if (rawPressed != stablePressed && now - buttonChangedAt >= DEBOUNCE_US) {
        stablePressed = rawPressed;
        if (stablePressed && pendingPresses < std::numeric_limits<unsigned int>::max()) {
            ++pendingPresses;
        }
    }
}

void onEdge(unsigned int gpio, std::uint32_t, void*) {
    if (!initialized) return;
    if (gpio == BUTTON_GPIO) {
        const auto now = time_us_64();
        settleButton(now);
        const bool pressed = !gpio_get(BUTTON_GPIO);
        if (pressed != rawPressed) {
            rawPressed = pressed;
            buttonChangedAt = now;
        }
        return;
    }
    const auto ab = readAB();
    if ((previousAB ^ ab) == 3) {
        quarterSteps = 0; // An impossible transition cannot invent a detent.
    } else {
        static constexpr int transitions[16] = {
             0, -1,  1,  0,
             1,  0,  0, -1,
            -1,  0,  0,  1,
             0,  1, -1,  0
        };
        quarterSteps += transitions[previousAB * 4 + ab];
        if (quarterSteps >= 4) {
            if (pendingSteps < std::numeric_limits<int>::max()) ++pendingSteps;
            quarterSteps = 0;
        } else if (quarterSteps <= -4) {
            if (pendingSteps > std::numeric_limits<int>::min()) --pendingSteps;
            quarterSteps = 0;
        }
    }
    previousAB = ab;
}
}

bool tone_encoder::reset() {
    if (get_core_num() != 0) return false;
    const auto interrupts = save_and_disable_interrupts();
    initialized = false;
    constexpr unsigned int pins[] = {A_GPIO, B_GPIO, BUTTON_GPIO};
    for (auto pin : pins) {
        if (!gpio_interrupts::attach(pin, EDGES, onEdge)) {
            // Release only our own subscriptions if initialization is partial.
            for (auto owned : pins) gpio_interrupts::detach(owned, onEdge);
            restore_interrupts(interrupts);
            return false;
        }
    }
    for (auto pin : pins) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
    }
    previousAB = readAB();
    quarterSteps = 0;
    pendingSteps = 0;
    pendingPresses = 0;
    rawPressed = stablePressed = !gpio_get(BUTTON_GPIO);
    buttonChangedAt = time_us_64();
    initialized = true;
    restore_interrupts(interrupts);
    return true;
}

tone_encoder::Input tone_encoder::poll() {
    if (!initialized && !reset()) return {};
    const auto interrupts = save_and_disable_interrupts();
    settleButton(time_us_64());
    Input input;
    input.steps = pendingSteps;
    pendingSteps = 0;
    input.pressed = pendingPresses != 0;
    if (input.pressed) --pendingPresses;
    restore_interrupts(interrupts);
    return input;
}
