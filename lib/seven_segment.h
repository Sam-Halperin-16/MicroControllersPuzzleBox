#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H

// Independent HW-069 / TM1637 driver for the Raspberry Pi Pico SDK.
// Only this driver touches the display GPIOs. Application timing belongs
// in main.cpp. The driver manages one display and is called from one core.
namespace seven_segment {

// Initialize the module and display 0000. Call before set_number().
// Pin arguments are GPIO numbers, NOT physical header pin numbers.
// Brightness: 0 = dimmest ON setting, 7 = brightest.
// Returns false for invalid settings or a missing hardware acknowledgement.
bool init(unsigned int clk_pin = 14, unsigned int dio_pin = 15,
          int brightness = 3);

// Display four INDEPENDENT integer digits, ordered LEFT to RIGHT.
// Example: set_number(0, 1, 2, 3) displays 0123, including the leading zero.
// Each argument must be 0..9. Invalid input leaves the display unchanged.
// Returns false if uninitialized, invalid, or the display does not ACK.
bool set_number(int digit1, int digit2, int digit3, int digit4);

// Blank or restore the display without changing its digits or brightness.
bool set_visible(bool visible);

}  // namespace seven_segment

#endif
