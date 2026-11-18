#include "system.h"
#include "morse_leds.h"
#include "pico/rand.h"
#include "globals.h"
#include <cstdio>
#include "lib/drawer_io.h"
#include "lib/touch_sensor.h"

// TOUCH1_GPIO
// TOUCH2_GPIO

void SystemStateMachine::generateFinalCode() {
    // Shake/Vibrate
    // 2-9
    finalCode[0] = get_rand_32() % 8 + 2; // Random number between 2 and 9
    // Walking code
    // 6
    finalCode[1] = 6;
    // Backlight code
    // 3
    finalCode[2] = 3;
    // Binary
    // 1-7
    finalCode[3] = get_rand_32() % 7 + 1; // Random number between 1 and 7
}

SystemStateMachine::SystemStateMachine() : StateMachine<SystemState>(SystemState::Idle) {
    generateFinalCode();
    touch_sensor_init(TOUCH1_GPIO, TOUCH2_GPIO);

    // Initialize the active state machines to a list of active state machines
    screenStateMachine = new ScreenStateMachine(finalCode);
    morseLedsStateMachine = new MorseLedsStateMachine();
    shakeVibrateStateMachine = new ShakeVibrateStateMachine(finalCode[0]);
    touchDrawerStateMachine = new TouchDrawerStateMachine();
    toneDrawerStateMachine = new ToneDrawerStateMachine();
    morseLedsStateMachine->setState(MorseLedsState::Blinking);
    shakeVibrateStateMachine->setState(ShakeVibrateState::Waiting);
}

void SystemStateMachine::tick() {
    if (currentState != SystemState::Active && currentState != SystemState::Frozen) {
        return;
    }
    // drawer_io::setWireBacklight(true);
    // An incorrect-code timeout blocks IR entry, not the independent puzzles.
    screenStateMachine->tick();
    if (screenStateMachine->getState() == ScreenState::Completed) {
        if (currentState != SystemState::Frozen) {
            toneDrawerStateMachine->suspend();
        }
        setState(SystemState::Frozen);
        return;
    }
    setState(SystemState::Active);
    touchDrawerStateMachine->tick();
    toneDrawerStateMachine->tick();
    wireMatchingStateMachine.tick();
    if (morseLedsStateMachine != nullptr) {
        morseLedsStateMachine->tick();
    }
    if (shakeVibrateStateMachine != nullptr) {
        shakeVibrateStateMachine->tick();
    }
}

void SystemStateMachine::onIrDigit(std::uint8_t digit) {
    resetSequence = 0;
    if (currentState == SystemState::Active || currentState == SystemState::Frozen) {
        screenStateMachine->onIrDigit(digit);
    }
}

void SystemStateMachine::onIrOk() {
    resetSequence = 0;
    if (currentState == SystemState::Active || currentState == SystemState::Frozen) {
        screenStateMachine->onIrOk();
        if (screenStateMachine->getState() == ScreenState::Completed) {
            toneDrawerStateMachine->suspend();
            setState(SystemState::Frozen);
        }
    }
}

void SystemStateMachine::reset() {
    generateFinalCode();
    // Reuse the existing machines so repeated resets do not allocate memory.
    *screenStateMachine = ScreenStateMachine(finalCode);
    *morseLedsStateMachine = MorseLedsStateMachine();
    *shakeVibrateStateMachine = ShakeVibrateStateMachine(finalCode[0]);
    morseLedsStateMachine->setState(MorseLedsState::Blinking);
    touchDrawerStateMachine->reset();
    toneDrawerStateMachine->reset();
    for (std::uint8_t pin = 0; pin < 16; ++pin) {
        gpio_expander.setPin(pin, false);
    }
    gpio_expander.update();
    wireMatchingStateMachine.reset();
    resetSequence = 0;
    setState(SystemState::Active);
    screenStateMachine->tick();
}

void SystemStateMachine::onIrEvent(const ir_receiver::Event& event) {
    printf("SystemStateMachine::onIrEvent() - Button: %d, Digit: %d\n",
           static_cast<int>(event.button), event.digit);
    // Reset is available in every system state, including completed/lockout.
    if (event.button == ir_receiver::Button::Hash) {
        if (resetSequence == 2) {
            reset();
        } else {
            resetSequence = 1;
        }
    } else if (event.button == ir_receiver::Button::Star) {
        resetSequence = resetSequence == 1 ? 2 : 0;
    } else {
        resetSequence = 0;
        if (event.button == ir_receiver::Button::Digit) {
            onIrDigit(event.digit);
        } else if (event.button == ir_receiver::Button::Ok) {
            onIrOk();
        }
    }
}
