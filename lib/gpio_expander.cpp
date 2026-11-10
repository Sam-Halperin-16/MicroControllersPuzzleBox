#include "gpio_expander.h"
GpioExpander::GpioExpander() {}

void GpioExpander::setPin(std::uint8_t pin, std::uint8_t value) {
    pins[pin] = value;
}

std::uint8_t GpioExpander::getPin(std::uint8_t pin) {
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