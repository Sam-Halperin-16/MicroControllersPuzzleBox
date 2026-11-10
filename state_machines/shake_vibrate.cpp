#include "shake_vibrate.h"
#include "pico/time.h"

const std::uint32_t VIBRATE_DURATION_US = 500000; // 500ms in microseconds

void ShakeVibrateStateMachine::tick() {
    // Detect a vigorous shake and transition to Vibrating state if necessary
    if (currentState == ShakeVibrateState::Waiting) {
        // Check for vigorous shake
        // if (detectVigorousShake()) {
        //     setState(ShakeVibrateState::Vibrating);
        //     vibrateStartTime = time_us_32();
        // }
    }
    if (currentState == ShakeVibrateState::Vibrating) {
        std::uint32_t elapsedTime = time_us_32() - vibrateStartTime;
        if (elapsedTime > VIBRATE_DURATION_US * 2 * code) {
            setState(ShakeVibrateState::Waiting);
            return;
        }
        if(((elapsedTime / (VIBRATE_DURATION_US)) % 2 == 0) != vibrating) {
            // Toggle the vibration state
            vibrating = !vibrating;
            // setVibrate(vibrating);
            // set_gpio(vibratingpin, vibrating); ?
        }
    }
}