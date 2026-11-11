#include "drawers.h"
#include "lib/drawer_io.h"
#include "pico/rand.h"
#include "pico/time.h"

namespace {
// Initial tuning values; calibrate for the potentiometer and speaker.
constexpr unsigned int MIN_HZ = 400;
constexpr unsigned int MAX_HZ = 1600;
constexpr unsigned int MATCH_TOLERANCE_HZ = 20;
constexpr int POT_DEADBAND = 8;
constexpr std::uint64_t SETTLE_US = 200000;
constexpr std::uint64_t TONE_US = 500000;
constexpr std::uint64_t GAP_US = 150000;

unsigned int randomTarget() {
    return MIN_HZ + get_rand_32() % (MAX_HZ - MIN_HZ + 1);
}
}

void TouchDrawerStateMachine::reset() {
    setState(DrawerState::Locked);
    drawer_io::setReleased(drawer_io::Drawer::Magnet, false);
    needsLock = false;
}

void TouchDrawerStateMachine::tick() {
    if (needsLock) {
        reset();
    }
    if (currentState == DrawerState::Locked &&
        drawer_io::screwTouched(0) && drawer_io::screwTouched(1)) {
        drawer_io::setReleased(drawer_io::Drawer::Magnet, true);
        setState(DrawerState::Released);
    }
}

ToneDrawerStateMachine::ToneDrawerStateMachine()
    : StateMachine<DrawerState>(DrawerState::Locked), targetFrequency(randomTarget()) {}

void ToneDrawerStateMachine::suspend() {
    drawer_io::stopTone();
    toneState = ToneState::Waiting;
}

void ToneDrawerStateMachine::reset() {
    suspend();
    setState(DrawerState::Locked);
    targetFrequency = randomTarget(); // Keep this target for the entire new run.
    lastValue = 0;
    currentFrequency = 0;
    phaseStartTime = 0;
    drawer_io::setReleased(drawer_io::Drawer::Wires, false);
    needsLock = false;
}

void ToneDrawerStateMachine::tick() {
    if (needsLock) {
        drawer_io::setReleased(drawer_io::Drawer::Wires, false);
        needsLock = false;
    }
    if (currentState == DrawerState::Released) {
        return;
    }
    std::uint16_t value;
    if (!drawer_io::readPotentiometer(value) || value > 4095) {
        suspend();
        return;
    }
    const auto now = time_us_64();
    const int change = static_cast<int>(value) - lastValue;
    if (toneState == ToneState::Waiting || change >= POT_DEADBAND || change <= -POT_DEADBAND) {
        drawer_io::stopTone();
        lastValue = value;
        currentFrequency = MIN_HZ + static_cast<unsigned int>(value) * (MAX_HZ - MIN_HZ) / 4095;
        toneState = ToneState::Settling;
        phaseStartTime = now;
        return;
    }
    const auto elapsed = now - phaseStartTime;
    switch (toneState) {
    case ToneState::Settling:
        if (elapsed >= SETTLE_US) {
            drawer_io::playTone(currentFrequency);
            toneState = ToneState::PlayingCurrent;
            phaseStartTime = now;
        }
        break;
    case ToneState::PlayingCurrent:
        if (elapsed >= TONE_US) {
            drawer_io::stopTone();
            toneState = ToneState::Gap;
            phaseStartTime = now;
        }
        break;
    case ToneState::Gap:
        if (elapsed >= GAP_US) {
            drawer_io::playTone(targetFrequency);
            toneState = ToneState::PlayingTarget;
            phaseStartTime = now;
        }
        break;
    case ToneState::PlayingTarget:
        if (elapsed >= TONE_US) {
            drawer_io::stopTone();
            const int difference = static_cast<int>(currentFrequency) - static_cast<int>(targetFrequency);
            if (difference >= -static_cast<int>(MATCH_TOLERANCE_HZ) &&
                difference <= static_cast<int>(MATCH_TOLERANCE_HZ)) {
                drawer_io::setReleased(drawer_io::Drawer::Wires, true);
                setState(DrawerState::Released);
            }
            // Stay here silently until the knob changes; do not repeat feedback.
            toneState = ToneState::AwaitingAdjustment;
        }
        break;
    case ToneState::Waiting:
    case ToneState::AwaitingAdjustment:
        break;
    }
}
