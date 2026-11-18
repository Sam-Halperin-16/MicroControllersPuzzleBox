#ifndef GPIO_EXPANDER_H
#define GPIO_EXPANDER_H

#include <cstdint>

class GpioExpander {
private:
    std::uint16_t pins = 0;
    std::uint16_t previousPins = 0;
    bool busInitialized = false;
    bool initialized = false;
    std::uint64_t retryAfterUs = 0;
public:
    // MCP23017 at 0x20 on i2c0, GP0 SDA / GP1 SCL (100 kHz).
    // Construction only initializes memory; call init/update after SDK startup.
    GpioExpander() = default;
    bool init();
    // Buffered logical outputs: 0..7 = GPA0..7, 8..15 = GPB0..7.
    // true means enabled. Board LEDs 0..9 are active low; 10..15 active high.
    // All 16 pins are outputs. Invalid pin numbers are ignored/read as false.
    void setPin(std::uint8_t pin, bool value);
    // Returns the requested logical state, not a hardware input reading.
    bool getPin(std::uint8_t pin) const;
    // Lazily initializes, then flushes changed outputs. False means I2C failed
    // or a retry is pending; desired values are retained. Retry after 100 ms.
    // Call regularly on core 0, outside interrupts, like other I2C users.
    bool update();
};

#endif // GPIO_EXPANDER_H
