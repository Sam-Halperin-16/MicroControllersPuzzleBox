#include "gpio_expander.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

namespace {
constexpr std::uint8_t ADDRESS = 0x20;
constexpr std::uint8_t IODIRA = 0x00;
constexpr std::uint8_t IOCON = 0x0a;
constexpr std::uint8_t OLATA = 0x14;
// constexpr std::uint16_t ACTIVE_LOW_MASK = 0x03c0;
constexpr std::uint16_t ACTIVE_LOW_MASK = 0x0000;
constexpr std::uint32_t TIMEOUT_US = 2000;
constexpr std::uint64_t RETRY_US = 100000;

bool writeRegister(std::uint8_t reg, std::uint8_t value) {
    const std::uint8_t data[] = {reg, value};
    return i2c_write_timeout_us(i2c0, ADDRESS, data, sizeof(data), false, TIMEOUT_US)
        == static_cast<int>(sizeof(data));
}

bool writePair(std::uint8_t reg, std::uint16_t value) {
    const std::uint8_t data[] = {
        reg, static_cast<std::uint8_t>(value), static_cast<std::uint8_t>(value >> 8)
    };
    return i2c_write_timeout_us(i2c0, ADDRESS, data, sizeof(data), false, TIMEOUT_US)
        == static_cast<int>(sizeof(data));
}
}

bool GpioExpander::init() {
    if (initialized) return true;
    if (time_us_64() < retryAfterUs) return false;
    if (!busInitialized) {
        i2c_init(i2c0, 100000);
        gpio_set_function(0, GPIO_FUNC_I2C);
        gpio_set_function(1, GPIO_FUNC_I2C);
        gpio_pull_up(0);
        gpio_pull_up(1);
        busInitialized = true;
    }

    // Force BANK=0 even if only the Pico was reset: 0x05 is IOCON in
    // BANK=1, or GPINTENB in BANK=0. Then enable sequential addressing.
    // Preload all outputs OFF before changing directions to avoid LED flashes.
    if (!writeRegister(0x05, 0) || !writeRegister(IOCON, 0) ||
        !writePair(OLATA, ACTIVE_LOW_MASK) || !writePair(IODIRA, 0)) {
        retryAfterUs = time_us_64() + RETRY_US;
        return false;
    }
    previousPins = 0;
    initialized = true;
    return true;
}

void GpioExpander::setPin(std::uint8_t pin, bool value) {
    if (pin >= 16) return;
    const auto mask = static_cast<std::uint16_t>(1u << pin);
    if (value) pins |= mask;
    else pins &= static_cast<std::uint16_t>(~mask);
}

bool GpioExpander::getPin(std::uint8_t pin) const {
    return pin < 16 && (pins & (1u << pin)) != 0;
}

bool GpioExpander::update() {
    if (!init()) return false;
    if (pins == previousPins) return true;
    if (!writePair(OLATA, pins ^ ACTIVE_LOW_MASK)) {
        // A short write may have changed only port A. Reconfigure and resend
        // the entire requested state on retry, even if it changes back meanwhile.
        initialized = false;
        retryAfterUs = time_us_64() + RETRY_US;
        return false;
    }
    previousPins = pins;
    return true;
}
