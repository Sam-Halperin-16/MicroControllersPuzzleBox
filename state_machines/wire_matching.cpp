#include "wire_matching.h"
#include "matching_config.h"
#include "lib/drawer_io.h"
#include "pico/time.h"
#include <cstdio>

void WireMatchingStateMachine::reset() {
    setState(WireMatchingState::Waiting);
    matchStartTime = 0;
    drawer_io::setWireBacklight(false);
    needsReset = false;
}

void WireMatchingStateMachine::tick() {
    if (needsReset) {
        reset();
    }
    if (currentState == WireMatchingState::Solved) {
        // return; // Keep the earned clue lit until the session is reset.
    }

    using namespace matching_config;
    std::uint16_t sample;
    if (!drawer_io::readWireNetwork(sample) || sample > ADC_MAX) {
        setState(WireMatchingState::Waiting);
        return;
    }
    const float voltage = sample * ADC_REFERENCE_VOLTS / ADC_MAX;
    if (voltage < WIRE_TARGET_VOLTS - WIRE_TOLERANCE_VOLTS ||
        voltage > WIRE_TARGET_VOLTS + WIRE_TOLERANCE_VOLTS) {
        setState(WireMatchingState::Waiting);
        return;
    }

    const auto now = time_us_64();
    if (currentState == WireMatchingState::Waiting) {
        matchStartTime = now;
        setState(WireMatchingState::Settling);
    } else if (now - matchStartTime >= WIRE_SETTLE_US) {
        drawer_io::setWireBacklight(true);
        setState(WireMatchingState::Solved);
    }
}
