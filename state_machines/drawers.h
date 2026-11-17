#ifndef DRAWERS_H
#define DRAWERS_H

#include "state_machine.h"
#include "matching_config.h"
#include <cstdint>

enum class DrawerState { Locked, Released };
enum class ToneState { Waiting, PlayingTarget, Tuning, Complete };

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
    unsigned int currentFrequency = matching_config::INITIAL_HZ;
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
