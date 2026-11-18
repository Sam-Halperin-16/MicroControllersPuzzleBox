#include "drawer_io.h"
#include "globals.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <cstdio>

namespace {
constexpr unsigned int WIRE_ADC_GPIO = 26;
constexpr unsigned int BUZZER_GPIO = 10;
constexpr std::uint8_t BACKLIGHT_PIN = 10; // MCP23017 GPB2, active high.
constexpr unsigned int MAGNET_SERVO_GPIO = 7;
constexpr unsigned int WIRES_SERVO_GPIO = 8;
constexpr unsigned int SERVO_PERIOD_US = 20000; // 50 Hz.
// Nominal 0..180 degree pulse range; calibrate for the installed servos.
constexpr unsigned int SERVO_MIN_PULSE_US = 500;
constexpr unsigned int SERVO_MAX_PULSE_US = 2500;
constexpr unsigned int SERVO_RELEASED_DEGREES = 140;
constexpr unsigned int SERVO_LOCKED_DEGREES = 50;

void setServoPosition(unsigned int gpio, unsigned int degrees, bool& initialized) {
    const auto pulseUs = SERVO_MIN_PULSE_US +
        ((SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US) * degrees + 90) / 180;
    const auto slice = pwm_gpio_to_slice_num(gpio);
    if (!initialized) {
        // A 1 MHz counter makes each PWM count one microsecond.
        // GP7 and GP8 use separate slices, also separate from the GP10 buzzer.
        auto config = pwm_get_default_config();
        pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / 1000000.0f);
        pwm_config_set_wrap(&config, SERVO_PERIOD_US - 1);
        pwm_init(slice, &config, false);
        // Load the target before connecting and enabling the PWM output.
        pwm_set_gpio_level(gpio, pulseUs);
        gpio_set_function(gpio, GPIO_FUNC_PWM);
        pwm_set_enabled(slice, true);
        initialized = true;
    } else {
        pwm_set_gpio_level(gpio, pulseUs);
    }
}

bool readAdc(unsigned int gpio, std::uint16_t& value) {
    static bool initialized = false;
    if (!initialized) {
        adc_init();
        adc_gpio_init(WIRE_ADC_GPIO);
        initialized = true;
    }
    adc_select_input(gpio - 26);
    adc_read(); // Discard the first conversion after switching ADC channels.
    value = adc_read();
    return true;
}
}

namespace drawer_io {
void setReleased(Drawer drawer, bool released) {
    static bool magnetInitialized = false;
    static bool wiresInitialized = false;
    const auto degrees = released ? SERVO_RELEASED_DEGREES : SERVO_LOCKED_DEGREES;
    switch (drawer) {
    case Drawer::Magnet:
        setServoPosition(MAGNET_SERVO_GPIO, degrees, magnetInitialized);
        break;
    case Drawer::Wires:
        setServoPosition(WIRES_SERVO_GPIO, degrees, wiresInitialized);
        break;
    default:
        return;
    }
    // Released means commanded open; there are no drawer position sensors yet.
    printf("drawer_io::setReleased() - Drawer: %s, Released: %s\n",
           drawer == Drawer::Magnet ? "Magnet" : "Wires",
           released ? "true" : "false");
}

bool readWireNetwork(std::uint16_t& value) {
    return readAdc(WIRE_ADC_GPIO, value);
}

void setWireBacklight(bool enabled) {
    gpio_expander.setPin(BACKLIGHT_PIN, enabled);
    gpio_expander.update();
}

void playTone(unsigned int frequencyHz) {
    if (frequencyHz == 0) {
        stopTone();
        return;
    }
    // Divide the system clock to keep the 400–1600 Hz range within 16-bit PWM.
    constexpr float divider = 64.0f;
    const auto period = static_cast<std::uint32_t>(clock_get_hz(clk_sys) / (divider * frequencyHz));
    if (period < 2 || period > 65536) {
        stopTone();
        return;
    }
    gpio_set_function(BUZZER_GPIO, GPIO_FUNC_PWM);
    const auto slice = pwm_gpio_to_slice_num(BUZZER_GPIO);
    auto config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, divider);
    pwm_config_set_wrap(&config, static_cast<std::uint16_t>(period - 1));
    pwm_init(slice, &config, false);
    pwm_set_gpio_level(BUZZER_GPIO, period / 2);
    pwm_set_enabled(slice, true);
}

void stopTone() {
    pwm_set_enabled(pwm_gpio_to_slice_num(BUZZER_GPIO), false);
    // Drive low even if PWM stopped while its output was high.
    gpio_init(BUZZER_GPIO);
    gpio_set_dir(BUZZER_GPIO, GPIO_OUT);
    gpio_put(BUZZER_GPIO, false);
}
}
