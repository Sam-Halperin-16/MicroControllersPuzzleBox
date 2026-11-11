#ifndef GPIO_EXPANDER_H
#define GPIO_EXPANDER_H

#include <cstdint>

class GpioExpander {
private:
    bool pins[16]; // Assuming 16 pins for the GPIO expander
    bool previousPins[16]; // Store the previous pin states for comparison
public:
    GpioExpander();
    void setPin(std::uint8_t pin, bool value);
    bool getPin(std::uint8_t pin);
    void update();
};

#endif // GPIO_EXPANDER_H