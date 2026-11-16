#include "drawer_io.h"
#include "globals.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

namespace {
constexpr unsigned int WIRE_ADC_GPIO = 26;
constexpr unsigned int POT_ADC_GPIO = 27;
constexpr unsigned int BUZZER_GPIO = 10;
constexpr std::uint8_t BACKLIGHT_PIN = 10; // MCP23017 GPB2, active high.

bool readAdc(unsigned int gpio, std::uint16_t& value) {
    static bool initialized = false;
    if (!initialized) {
        adc_init();
        adc_gpio_init(WIRE_ADC_GPIO);
        adc_gpio_init(POT_ADC_GPIO);
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
    // TODO: Configure each servo GPIO and its calibrated locked/released PWM
    // positions. Command the latch position without waiting for servo travel.
    // Released means commanded open; there are no drawer position sensors yet.
    (void)drawer;
    (void)released;
}

bool screwTouched(unsigned int screw) {
    // TODO: Read calibrated capacitive touch inputs, indexed 0 and 1.
    (void)screw;
    return false;
}

bool readPotentiometer(std::uint16_t& value) {
    return readAdc(POT_ADC_GPIO, value);
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
