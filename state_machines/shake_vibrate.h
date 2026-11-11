#ifndef SHAKE_VIBRATE_H
#define SHAKE_VIBRATE_H

#include "state_machine.h"
#include <cstdint>

enum class ShakeVibrateState {
    Waiting,
    Vibrating
};

class ShakeVibrateStateMachine : public StateMachine<ShakeVibrateState> {
protected:
    std::uint8_t code;
    std::uint32_t vibrateStartTime = 0;
    bool vibrating = false;
public:
    ShakeVibrateStateMachine(std::uint8_t code) : StateMachine<ShakeVibrateState>(ShakeVibrateState::Waiting), code(code) {}
    void tick() override;
};

#endif // SHAKE_VIBRATE_H
