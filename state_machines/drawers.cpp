#include "drawers.h"
#include "lib/drawer_io.h"
#include "pico/rand.h"
#include "pico/time.h"
#include "matching_config.h"
#include "lib/tone_encoder.h"

using namespace matching_config;

namespace {
unsigned int randomTarget() {
    return MIN_HZ + get_rand_32() % (MAX_HZ - MIN_HZ + 1);
}

unsigned int newTarget(unsigned int previous) {
    static_assert(MAX_HZ > MIN_HZ, "Tone matching needs at least two target frequencies");
    // Select from every frequency except the old target, without retry loops.
    const auto candidate = MIN_HZ + get_rand_32() % (MAX_HZ - MIN_HZ);
    return candidate >= previous ? candidate + 1 : candidate;
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
    toneState = currentState == DrawerState::Released ? ToneState::Complete : ToneState::Waiting;
}

void ToneDrawerStateMachine::reset() {
    drawer_io::stopTone();
    setState(DrawerState::Locked);
    toneState = ToneState::Waiting;
    targetFrequency = randomTarget();
    currentFrequency = INITIAL_HZ;
    phaseStartTime = 0;
    tone_encoder::reset();
    drawer_io::setReleased(drawer_io::Drawer::Wires, false);
    needsLock = false;
}

void ToneDrawerStateMachine::tick() {
    if (needsLock) {
        drawer_io::setReleased(drawer_io::Drawer::Wires, false);
        needsLock = false;
    }
    // Drain inputs in every phase. A press during the reference must not submit
    // on entry to tuning, and holding the button must never count twice.
    const auto input = tone_encoder::poll();
    const auto now = time_us_64();
    switch (toneState) {
    case ToneState::Waiting:
        if (input.pressed) {
            drawer_io::playTone(targetFrequency);
            phaseStartTime = now;
            toneState = ToneState::PlayingTarget;
        }
        break;
    case ToneState::PlayingTarget:
        if (now - phaseStartTime >= TARGET_TONE_US) {
            drawer_io::playTone(currentFrequency);
            toneState = ToneState::Tuning;
        }
        break;
    case ToneState::Tuning: {
        // Use a wide signed intermediate so turns clamp rather than wrap.
        auto adjusted = static_cast<std::int64_t>(currentFrequency) +
                        static_cast<std::int64_t>(input.steps) * ENCODER_STEP_HZ;
        if (adjusted < MIN_HZ) adjusted = MIN_HZ;
        if (adjusted > MAX_HZ) adjusted = MAX_HZ;
        const auto frequency = static_cast<unsigned int>(adjusted);
        if (frequency != currentFrequency) {
            currentFrequency = frequency;
            drawer_io::playTone(currentFrequency);
        }
        if (input.pressed) {
            const int difference = static_cast<int>(currentFrequency) - static_cast<int>(targetFrequency);
            if (difference >= -static_cast<int>(MATCH_TOLERANCE_HZ) &&
                difference <= static_cast<int>(MATCH_TOLERANCE_HZ)) {
                drawer_io::stopTone();
                drawer_io::setReleased(drawer_io::Drawer::Wires, true);
                setState(DrawerState::Released);
                toneState = ToneState::Complete;
            } else {
                targetFrequency = newTarget(targetFrequency);
                drawer_io::playTone(targetFrequency);
                phaseStartTime = now;
                toneState = ToneState::PlayingTarget;
            }
        }
        break;
    }
    case ToneState::Complete:
        break;
    }
}
