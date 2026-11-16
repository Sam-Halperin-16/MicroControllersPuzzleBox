#ifndef WIRE_MATCHING_H
#define WIRE_MATCHING_H

#include "state_machine.h"
#include <cstdint>

enum class WireMatchingState { Waiting, Settling, Solved };

// The existing resistor network encodes the four connections in one ADC voltage.
// This cannot distinguish incorrect connections that produce the same voltage.
class WireMatchingStateMachine : public StateMachine<WireMatchingState> {
public:
    WireMatchingStateMachine() : StateMachine<WireMatchingState>(WireMatchingState::Waiting) {}
    void tick() override;
    void reset();
private:
    bool needsReset = true;
    std::uint64_t matchStartTime = 0;
};

#endif
