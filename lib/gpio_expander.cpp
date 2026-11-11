#include "gpio_expander.h"
#include <cstdint>
GpioExpander::GpioExpander() {}

void GpioExpander::setPin(std::uint8_t pin, bool value) {
    pins[pin] = value;
}

bool GpioExpander::getPin(std::uint8_t pin) {
    return pins[pin];
}

void GpioExpander::update() {
    for (std::uint8_t pin = 0; pin < 16; pin++) {
        if (pins[pin] != previousPins[pin]) {
            // Handle the change in the pin state
            // TODO: I2C
        }
        previousPins[pin] = pins[pin];
    }
}