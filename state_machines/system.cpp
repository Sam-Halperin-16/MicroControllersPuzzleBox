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

    // Initialize the active state machines to a list of active state machines
    // Morse LEDs
    morseLedsStateMachine = new MorseLedsStateMachine();
    shakeVibrateStateMachine = new ShakeVibrateStateMachine(finalCode[0]);
    morseLedsStateMachine->setState(MorseLedsState::Blinking);
    shakeVibrateStateMachine->setState(ShakeVibrateState::Waiting);
}

void SystemStateMachine::tick() {
    // Implement the system state machine logic here
    if (morseLedsStateMachine != nullptr) {
        morseLedsStateMachine->tick();
    }
    if (shakeVibrateStateMachine != nullptr) {
        shakeVibrateStateMachine->tick();
    }
    if (currentState == SystemState::Active) {
        //
    }
}