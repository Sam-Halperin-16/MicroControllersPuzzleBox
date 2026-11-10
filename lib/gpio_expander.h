#ifndef GPIO_EXPANDER_H
#define GPIO_EXPANDER_H

#include <cstdint>

class GpioExpander {
private:
    std::uint8_t pins[16]; // Assuming 16 pins for the GPIO expander
    std::uint8_t previousPins[16]; // Store the previous pin states for comparison
public:
    GpioExpander();
    void setPin(std::uint8_t pin, std::uint8_t value);
    std::uint8_t getPin(std::uint8_t pin);
    void update();
};

#endif // GPIO_EXPANDER_H