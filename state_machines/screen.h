#ifndef SCREEN_H
#define SCREEN_H

#include "state_machine.h"
#include <cstdint>

enum class ScreenState {
    Stopwatch,
    CodeEntry,
    Countdown,
    Completed
};

class ScreenStateMachine : public StateMachine<ScreenState> {
private:
    std::uint8_t finalCode[4];
    std::uint8_t enteredCode[4] = {};
    std::uint8_t nextDigit = 0;
    std::uint8_t digitCount = 0;
    std::uint64_t startTime;
    std::uint64_t lastInputTime = 0;
    std::uint64_t countdownStartTime = 0;
    bool displayInitialized = false;
    int displayedNumber = -1;
    bool displayVisible = true;

    void clearEntry();
    bool codeMatches() const;
    void showNumber(int number, bool visible = true);

public:
    explicit ScreenStateMachine(const std::uint8_t code[4]);
    void tick() override;
    void onIrDigit(std::uint8_t digit);
    void onIrOk();
};

#endif // SCREEN_H
