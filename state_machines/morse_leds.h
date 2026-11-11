#ifndef MORSE_LEDS_H
#define MORSE_LEDS_H

#include "state_machine.h"
#include <cstdint>

enum class MorseLedsState {
    Off,
    Blinking
};

class MorseLedsStateMachine : public StateMachine<MorseLedsState> {
protected:
    // Start time
    unsigned long startTime;
    void update_leds(std::uint16_t morseCode, std::uint8_t currentBit);
public:
    MorseLedsStateMachine();
    void tick() override;
};

#endif // MORSE_LEDS_H