#ifndef SYSTEM_H
#define SYSTEM_H

#include "state_machine.h"
#include "morse_leds.h"
#include "shake_vibrate.h"
#include <cstdint>

enum class SystemState {
    Idle,
    Active,
    Error
};

class SystemStateMachine : public StateMachine<SystemState> {
protected:
    // State machines during active operation
    MorseLedsStateMachine* morseLedsStateMachine;
    ShakeVibrateStateMachine* shakeVibrateStateMachine;
    std::uint8_t finalCode[4];
public:
    SystemStateMachine();
    void tick() override;
};

#endif // SYSTEM_H