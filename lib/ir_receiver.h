#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <cstdint>

namespace ir_receiver {

enum class Button {
    Digit,
    Ok,
    Hash,
    Star
};

struct Event {
    Button button;
    std::uint8_t digit;
};

// Configure the receiver GPIO and NEC command table in ir_receiver.cpp first.
// Returns false while the receiver pin is unconfigured.
bool init();

// Consume decoded presses in the main loop; repeated/unknown frames are ignored.
bool poll(Event& event);

} // namespace ir_receiver

#endif // IR_RECEIVER_H
