#include "morse_leds.h"
#include <cstdint>
#include "pico/stdlib.h"
#include "globals.h"
#include <cstdio>

MorseLedsStateMachine::MorseLedsStateMachine() : StateMachine<MorseLedsState>(MorseLedsState::Off) {
    startTime = time_us_32();
}

namespace MorseCodeData {
    // 5 LEDs simultaneously blinking in Morse code
    // S, H, A, K and E
    // S = .x.x.xxx
    // H = .x.x.x.xxx
    // A = .x---xxx
    // K = ---x.x---xxx
    // E = .xxx
    const std::uint8_t morseCodeLengths[] = {
        8,  // S
        10, // H
        8,  // A
        12, // K
        4   // E
    };
    const std::uint16_t morseCode[] = {
        0b10101000, // S
        0b1010101000, // H
        0b10111000, // A
        0b111010111000, // K
        0b1000 // E
    };
    const std::uint32_t DOT_DURATION = 200000; // 200ms in microseconds
    const std::uint32_t LED_ID[] = {
        0, // S
        1, // H
        2, // A
        3, // K
        4  // E
    };
}


void MorseLedsStateMachine::tick() {
    // Implement the Morse code blinking logic here
    if (currentState == MorseLedsState::Blinking) {
        // Calculate the elapsed time since the last state change
        std::uint32_t elapsedTime = time_us_32() - startTime;
        std::uint32_t elapsedDots = elapsedTime / MorseCodeData::DOT_DURATION;
        for (std::uint8_t currentLetter = 0; currentLetter < 5; currentLetter++) {
            // Determine the current position in the Morse code sequence
            std::uint8_t currentBit = elapsedDots % MorseCodeData::morseCodeLengths[currentLetter];
            // Update the LED states based on the current bit
            update_leds(currentLetter, currentBit);
        }
    } else if (currentState == MorseLedsState::Off) {
        for (std::uint8_t currentLetter = 0; currentLetter < 5; currentLetter++) {
            update_leds(0, currentLetter);
        }
    }
}

void MorseLedsStateMachine::update_leds(std::uint8_t letter, std::uint8_t currentBit) {
    // Update the LED state based on the current bit of the Morse code
    const std::uint8_t shift =
        MorseCodeData::morseCodeLengths[letter] - 1 - currentBit;

    const bool on =
        ((MorseCodeData::morseCode[letter] >> shift) & 1u) != 0;

    gpio_expander.setPin(MorseCodeData::LED_ID[letter], on);

}