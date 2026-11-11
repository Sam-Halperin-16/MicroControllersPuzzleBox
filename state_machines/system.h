#ifndef SYSTEM_H
#define SYSTEM_H

#include "state_machine.h"
#include "morse_leds.h"
#include "shake_vibrate.h"
#include "screen.h"
#include "drawers.h"
#include "lib/ir_receiver.h"
#include <cstdint>

enum class SystemState {
    Idle,
    Active,
    Frozen,
    Error
};

class SystemStateMachine : public StateMachine<SystemState> {
protected:
    // State machines during active operation
    MorseLedsStateMachine* morseLedsStateMachine;
    ShakeVibrateStateMachine* shakeVibrateStateMachine;
    ScreenStateMachine* screenStateMachine;
    TouchDrawerStateMachine* touchDrawerStateMachine;
    ToneDrawerStateMachine* toneDrawerStateMachine;
    std::uint8_t finalCode[4];
    std::uint8_t resetSequence = 0;
    void generateFinalCode();
public:
    SystemStateMachine();
    void tick() override;
    void reset();
    void onIrEvent(const ir_receiver::Event& event);
    // Call from the IR decoder in the main loop, rather than an interrupt.
    void onIrDigit(std::uint8_t digit);
    void onIrOk();
};

#endif // SYSTEM_H
