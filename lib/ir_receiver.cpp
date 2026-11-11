#include "ir_receiver.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"

namespace {
// TODO: Set the demodulating, active-low NEC receiver's GPIO number.
constexpr int RECEIVER_GPIO = -1;

struct ButtonMapping {
    int command;
    ir_receiver::Button button;
    std::uint8_t digit;
};

// TODO: Replace -1 with each button's NEC command byte (0x00..0xFF).
// Unconfigured entries never match. Repeat frames do not enter extra digits.
constexpr ButtonMapping BUTTONS[] = {
    {-1, ir_receiver::Button::Digit, 0},
    {-1, ir_receiver::Button::Digit, 1},
    {-1, ir_receiver::Button::Digit, 2},
    {-1, ir_receiver::Button::Digit, 3},
    {-1, ir_receiver::Button::Digit, 4},
    {-1, ir_receiver::Button::Digit, 5},
    {-1, ir_receiver::Button::Digit, 6},
    {-1, ir_receiver::Button::Digit, 7},
    {-1, ir_receiver::Button::Digit, 8},
    {-1, ir_receiver::Button::Digit, 9},
    {-1, ir_receiver::Button::Ok, 0}
};

// Small single-core interrupt-to-main-loop queue. Drop new presses if full.
constexpr unsigned int QUEUE_SIZE = 8;
volatile std::uint8_t commands[QUEUE_SIZE] = {};
volatile unsigned int readIndex = 0;
volatile unsigned int writeIndex = 0;
std::uint32_t lastEdgeTime = 0;
std::uint32_t frame = 0;
unsigned int bitCount = 0;
bool receiving = false;

bool near(std::uint32_t duration, std::uint32_t expected, std::uint32_t tolerance) {
    return duration >= expected - tolerance && duration <= expected + tolerance;
}

void onEdge(unsigned int gpio, std::uint32_t events) {
    if (gpio != static_cast<unsigned int>(RECEIVER_GPIO) || !(events & GPIO_IRQ_EDGE_FALL)) {
        return;
    }
    const auto now = time_us_32();
    const auto elapsed = now - lastEdgeTime;
    lastEdgeTime = now;

    // Falling edges span the 9ms leader mark plus its 4.5ms space.
    if (near(elapsed, 13500, 1000)) {
        frame = 0;
        bitCount = 0;
        receiving = true;
        return;
    }
    if (!receiving) {
        return;
    }
    // Each interval includes a 562us mark plus a short/long data space.
    if (near(elapsed, 2250, 400)) {
        frame |= std::uint32_t{1} << bitCount;
    } else if (!near(elapsed, 1125, 300)) {
        receiving = false;
        return;
    }
    if (++bitCount != 32) {
        return;
    }
    receiving = false;
    const auto address = static_cast<std::uint8_t>(frame);
    const auto inverseAddress = static_cast<std::uint8_t>(frame >> 8);
    const auto command = static_cast<std::uint8_t>(frame >> 16);
    const auto inverseCommand = static_cast<std::uint8_t>(frame >> 24);
    if ((address ^ inverseAddress) != 0xFF || (command ^ inverseCommand) != 0xFF) {
        return; // Standard NEC frame integrity check.
    }
    const auto next = (writeIndex + 1) % QUEUE_SIZE;
    if (next != readIndex) {
        commands[writeIndex] = command;
        writeIndex = next;
    }
}
}

namespace ir_receiver {
bool init() {
    if (RECEIVER_GPIO < 0 ||
        !(RECEIVER_GPIO <= 22 || (RECEIVER_GPIO >= 26 && RECEIVER_GPIO <= 28))) {
        return false;
    }
    gpio_init(RECEIVER_GPIO);
    gpio_set_dir(RECEIVER_GPIO, GPIO_IN);
    gpio_pull_up(RECEIVER_GPIO);
    // This owns the Pico SDK's GPIO callback on this core.
    gpio_set_irq_enabled_with_callback(RECEIVER_GPIO, GPIO_IRQ_EDGE_FALL, true, onEdge);
    return true;
}

bool poll(Event& event) {
    while (true) {
        const auto interrupts = save_and_disable_interrupts();
        if (readIndex == writeIndex) {
            restore_interrupts(interrupts);
            return false;
        }
        const auto command = commands[readIndex];
        readIndex = (readIndex + 1) % QUEUE_SIZE;
        restore_interrupts(interrupts);
        for (const auto& mapping : BUTTONS) {
            if (mapping.command == command) {
                event = {mapping.button, mapping.digit};
                return true;
            }
        }
    }
}
} // namespace ir_receiver
