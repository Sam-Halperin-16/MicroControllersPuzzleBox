#ifndef DRAWERS_H
#define DRAWERS_H

#include "state_machine.h"
#include <cstdint>

enum class DrawerState { Locked, Released };
enum class ToneState { Waiting, Settling, PlayingCurrent, Gap, PlayingTarget, AwaitingAdjustment };

class TouchDrawerStateMachine : public StateMachine<DrawerState> {
private:
    bool needsLock = true;
public:
    TouchDrawerStateMachine() : StateMachine<DrawerState>(DrawerState::Locked) {}
    void tick() override;
    void reset();
};

class ToneDrawerStateMachine : public StateMachine<DrawerState> {
private:
    ToneState toneState = ToneState::Waiting;
    bool needsLock = true;
    std::uint16_t lastValue = 0;
    unsigned int currentFrequency = 0;
    unsigned int targetFrequency;
    std::uint64_t phaseStartTime = 0;
public:
    ToneDrawerStateMachine();
    void tick() override;
    void reset();
    void suspend();
    ToneState getToneState() const { return toneState; }
};

#endif // DRAWERS_H
