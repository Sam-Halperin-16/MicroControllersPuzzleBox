#include "seven_segment.h"

#include <cstddef>
#include <cstdint>
#include "pico/stdlib.h"

namespace {
// TM1637 segment bits: a,b,c,d,e,f,g occupy bits 0 through 6.
// Bit 7 controls a decimal point/colon on some modules; leave it off here.
constexpr uint8_t SEGMENTS[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};
constexpr uint32_t BIT_DELAY_US = 10;
unsigned int clk_gpio = 14;
unsigned int dio_gpio = 15;
bool initialized = false;
uint8_t display_brightness = 3;

bool exposed_pico_gpio(unsigned int pin) {
    // Pico 1 exposes GP0..GP22 and GP26..GP28 on its main headers.
    return pin <= 22 || (pin >= 26 && pin <= 28);
}

void pause_bus() {
    sleep_us(BIT_DELAY_US);
}

void dio_low() {
    // Set the output latch LOW before enabling output, avoiding a HIGH glitch.
    gpio_put(dio_gpio, 0);
    gpio_set_dir(dio_gpio, GPIO_OUT);
}

void dio_release() {
    // Emulate an open-drain signal: HIGH is input mode plus a pull-up.
    // This also lets the display pull DIO low for its acknowledgement.
    gpio_set_dir(dio_gpio, GPIO_IN);
}

void start_condition() {
    // START: DIO falls while CLK is HIGH. Data then changes with CLK LOW.
    dio_release();
    gpio_put(clk_gpio, 1);
    pause_bus();
    dio_low();
    pause_bus();
    gpio_put(clk_gpio, 0);
    pause_bus();
}

void stop_condition() {
    // STOP: DIO rises while CLK is HIGH; both lines finish idle HIGH.
    gpio_put(clk_gpio, 0);
    dio_low();
    pause_bus();
    gpio_put(clk_gpio, 1);
    pause_bus();
    dio_release();
    pause_bus();
}

bool write_byte(uint8_t value) {
    // TM1637 has its own protocol, not I2C. Send least-significant bit first.
    for (unsigned int bit = 0; bit < 8; ++bit) {
        if (value & (1u << bit)) {
            dio_release();
        } else {
            dio_low();
        }
        pause_bus();
        gpio_put(clk_gpio, 1);
        pause_bus();
        gpio_put(clk_gpio, 0);
        pause_bus();
    }

    // Ninth clock: release DIO so the display can acknowledge by pulling LOW.
    // Never actively drive DIO HIGH against the display's acknowledgement.
    dio_release();
    pause_bus();
    gpio_put(clk_gpio, 1);
    pause_bus();
    const bool acknowledged = !gpio_get(dio_gpio);
    gpio_put(clk_gpio, 0);
    pause_bus();
    return acknowledged;
}

bool send_bytes(const uint8_t* bytes, std::size_t length) {
    start_condition();
    for (std::size_t i = 0; i < length; ++i) {
        if (!write_byte(bytes[i])) {
            stop_condition();  // Also release the bus on a missing ACK.
            return false;
        }
    }
    stop_condition();
    return true;
}

bool write_digits(int d1, int d2, int d3, int d4) {
    const uint8_t write_mode[] = {0x40};  // Normal write, auto-increment.
    const uint8_t data[] = {
        0xC0,  // Begin at first digit; subsequent bytes select the next digit.
        SEGMENTS[d1], SEGMENTS[d2], SEGMENTS[d3], SEGMENTS[d4]
    };
    return send_bytes(write_mode, sizeof(write_mode)) &&
           send_bytes(data, sizeof(data));
}
}  // Private driver helpers; not visible to main.cpp.

namespace seven_segment {
bool init(unsigned int clk_pin, unsigned int dio_pin, int brightness) {
    if (!exposed_pico_gpio(clk_pin) || !exposed_pico_gpio(dio_pin) ||
        clk_pin == dio_pin || brightness < 0 || brightness > 7) {
        return false;
    }

    initialized = false;
    clk_gpio = clk_pin;
    dio_gpio = dio_pin;

    gpio_init(clk_gpio);
    gpio_put(clk_gpio, 1);
    gpio_set_dir(clk_gpio, GPIO_OUT);
    gpio_init(dio_gpio);
    gpio_pull_up(dio_gpio);
    dio_release();
    pause_bus();

    if (!write_digits(0, 0, 0, 0)) {
        return false;
    }
    // 0x88 = display ON; low three bits select brightness.
    const uint8_t control[] = {static_cast<uint8_t>(0x88 | brightness)};
    initialized = send_bytes(control, sizeof(control));
    display_brightness = static_cast<uint8_t>(brightness);
    return initialized;
}

bool set_visible(bool visible) {
    if (!initialized) {
        return false;
    }
    const uint8_t control[] = {
        static_cast<uint8_t>((visible ? 0x88 : 0x80) | display_brightness)
    };
    return send_bytes(control, sizeof(control));
}

bool set_number(int digit1, int digit2, int digit3, int digit4) {
    if (!initialized) {
        return false;
    }
    const int digits[] = {digit1, digit2, digit3, digit4};
    for (int digit : digits) {
        if (digit < 0 || digit > 9) {
            return false;  // Validate ALL digits before touching the bus.
        }
    }
    return write_digits(digit1, digit2, digit3, digit4);
}
}  // namespace seven_segment
