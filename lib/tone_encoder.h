#ifndef TONE_ENCODER_H
#define TONE_ENCODER_H

namespace tone_encoder {
struct Input {
    int steps = 0; // Signed full detents since the last poll.
    bool pressed = false; // One debounced press event, not a held level.
};

// Drain interrupt-captured rotation and debounced presses in the main loop.
// Initialize before input is needed. All access is on core 0; common goes to GND.
Input poll();
// Discard partial turns and held presses when starting a new session.
// Returns false if any input pin is owned by another interrupt subscriber.
bool reset();
}

#endif
