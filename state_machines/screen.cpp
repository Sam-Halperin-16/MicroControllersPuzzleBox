#include "screen.h"
#include "lib/seven_segment.h"
#include "pico/time.h"

namespace {
constexpr std::uint64_t SECOND_US = 1000000;
constexpr std::uint64_t ENTRY_TIMEOUT_US = 3 * SECOND_US;
constexpr std::uint64_t COUNTDOWN_US = 30 * SECOND_US;
constexpr std::uint64_t BLINK_US = 500000;
}

ScreenStateMachine::ScreenStateMachine(const std::uint8_t code[4])
    : StateMachine<ScreenState>(ScreenState::Stopwatch), startTime(time_us_64()) {
    for (int i = 0; i < 4; ++i) {
        finalCode[i] = code[i];
    }
}

void ScreenStateMachine::clearEntry() {
    for (auto& digit : enteredCode) {
        digit = 0;
    }
    nextDigit = 0;
    digitCount = 0;
}

bool ScreenStateMachine::codeMatches() const {
    if (digitCount != 4) {
        return false;
    }
    // Accept any ordering while preserving repeated reward digits.
    int counts[10] = {};
    for (int i = 0; i < 4; ++i) {
        ++counts[finalCode[i]];
        --counts[enteredCode[i]];
    }
    for (int count : counts) {
        if (count != 0) {
            return false;
        }
    }
    return true;
}

void ScreenStateMachine::showNumber(int number, bool visible) {
    if (!displayInitialized) {
        displayInitialized = seven_segment::init();
        if (!displayInitialized) {
            return;
        }
    }
    if (number != displayedNumber &&
        seven_segment::set_number(number / 1000, number / 100 % 10,
                                  number / 10 % 10, number % 10)) {
        displayedNumber = number;
    }
    if (visible != displayVisible && seven_segment::set_visible(visible)) {
        displayVisible = visible;
    }
}

void ScreenStateMachine::tick() {
    const auto now = time_us_64();
    if (currentState == ScreenState::CodeEntry && now - lastInputTime >= ENTRY_TIMEOUT_US) {
        clearEntry();
        setState(ScreenState::Stopwatch);
    }
    if (currentState == ScreenState::Countdown && now - countdownStartTime >= COUNTDOWN_US) {
        clearEntry();
        setState(ScreenState::Stopwatch);
    }

    switch (currentState) {
    case ScreenState::Stopwatch: {
        const auto seconds = (now - startTime) / SECOND_US;
        showNumber((seconds / 60 % 100) * 100 + seconds % 60);
        break;
    }
    case ScreenState::CodeEntry:
        showNumber(enteredCode[0] * 1000 + enteredCode[1] * 100 +
                   enteredCode[2] * 10 + enteredCode[3],
                   (now - lastInputTime) / BLINK_US % 2 == 0);
        break;
    case ScreenState::Countdown:
        // Round up so the display starts at 0030 and stays nonzero until expiry.
        showNumber((COUNTDOWN_US - (now - countdownStartTime) + SECOND_US - 1) / SECOND_US);
        break;
    case ScreenState::Completed:
        showNumber(8888);
        break;
    }
}

void ScreenStateMachine::onIrDigit(std::uint8_t digit) {
    tick(); // Expire entry/lockout even if input arrives before the next loop tick.
    if (digit > 9 || currentState == ScreenState::Countdown || currentState == ScreenState::Completed) {
        return;
    }
    if (currentState == ScreenState::Stopwatch) {
        clearEntry();
        setState(ScreenState::CodeEntry);
    }
    enteredCode[nextDigit] = digit;
    nextDigit = (nextDigit + 1) % 4;
    if (digitCount < 4) {
        ++digitCount;
    }
    lastInputTime = time_us_64();
    tick();
}

void ScreenStateMachine::onIrOk() {
    tick();
    if (currentState != ScreenState::CodeEntry) {
        return;
    }
    if (codeMatches()) {
        setState(ScreenState::Completed);
    } else {
        countdownStartTime = time_us_64();
        setState(ScreenState::Countdown);
    }
    clearEntry();
    tick();
}
