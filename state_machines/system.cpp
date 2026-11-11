#include "system.h"
#include "morse_leds.h"
#include "pico/rand.h"

SystemStateMachine::SystemStateMachine() : StateMachine<SystemState>(SystemState::Idle) {
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
    screenStateMachine = new ScreenStateMachine(finalCode);

    // Initialize the active state machines to a list of active state machines
    // Morse LEDs
    morseLedsStateMachine = new MorseLedsStateMachine();
    shakeVibrateStateMachine = new ShakeVibrateStateMachine(finalCode[0]);
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
        setState(SystemState::Frozen);
        return;
    }
    setState(SystemState::Active);
    if (morseLedsStateMachine != nullptr) {
        morseLedsStateMachine->tick();
    }
    if (shakeVibrateStateMachine != nullptr) {
        shakeVibrateStateMachine->tick();
    }
}

void SystemStateMachine::onIrDigit(std::uint8_t digit) {
    if (currentState == SystemState::Active || currentState == SystemState::Frozen) {
        screenStateMachine->onIrDigit(digit);
    }
}

void SystemStateMachine::onIrOk() {
    if (currentState == SystemState::Active || currentState == SystemState::Frozen) {
        screenStateMachine->onIrOk();
        if (screenStateMachine->getState() == ScreenState::Countdown ||
            screenStateMachine->getState() == ScreenState::Completed) {
            setState(SystemState::Frozen);
        }
    }
}
