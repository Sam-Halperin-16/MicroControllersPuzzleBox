#include "system.h"
#include "morse_leds.h"
#include "pico/rand.h"
#include "globals.h"

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
    // Spoken
    // 0-9
    finalCode[3] = get_rand_32() % 10; // Random number between 0 and 9
}

SystemStateMachine::SystemStateMachine() : StateMachine<SystemState>(SystemState::Idle) {
    generateFinalCode();
    screenStateMachine = new ScreenStateMachine(finalCode);

    // Initialize the active state machines to a list of active state machines
    // Morse LEDs
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
    // The screen must keep ticking while the puzzles are frozen for a timeout.
    screenStateMachine->tick();
    if (screenStateMachine->getState() == ScreenState::Countdown ||
        screenStateMachine->getState() == ScreenState::Completed) {
        if (currentState != SystemState::Frozen) {
            toneDrawerStateMachine->suspend();
        }
        setState(SystemState::Frozen);
        return;
    }
    setState(SystemState::Active);
    touchDrawerStateMachine->tick();
    toneDrawerStateMachine->tick();
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
        if (screenStateMachine->getState() == ScreenState::Countdown ||
            screenStateMachine->getState() == ScreenState::Completed) {
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
    resetSequence = 0;
    setState(SystemState::Active);
    screenStateMachine->tick();
}

void SystemStateMachine::onIrEvent(const ir_receiver::Event& event) {
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
