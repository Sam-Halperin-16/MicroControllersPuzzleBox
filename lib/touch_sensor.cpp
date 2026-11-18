#include "touch_sensor.h"
#include <stdint.h>
#include "pico/stdlib.h"

static uint32_t on_threshold1;
static uint32_t off_threshold1;
static uint32_t on_threshold2;
static uint32_t off_threshold2;

static bool touched1 = false;
static bool touched2 = false;

static uint32_t consecutive1 = 0;
static uint32_t consecutive2 = 0;

static uint32_t measure_touch(int pin)
{
    gpio_put(pin, 1);
    gpio_set_dir(pin, GPIO_OUT);
    sleep_us(10);

    uint32_t start = time_us_32();
    gpio_set_dir(pin, GPIO_IN);

    while (gpio_get(pin)) {
        if ((uint32_t)(time_us_32() - start) >= TIMEOUT_US) {
            break;
        }
    }

    uint32_t elapsed = time_us_32() - start;
    if (elapsed > TIMEOUT_US) {
        elapsed = TIMEOUT_US;
    }

    gpio_put(pin, 0);
    gpio_set_dir(pin, GPIO_OUT);
    sleep_us(100);

    return elapsed;
}

static uint32_t read_touch(int pin)
{
    uint32_t total = 0;

    for (uint32_t i = 0; i < SAMPLE_COUNT; ++i) {
        total += measure_touch(pin);
    }

    return total / SAMPLE_COUNT;
}

static void update_touch(uint32_t reading,
                         uint32_t on_threshold,
                         uint32_t off_threshold,
                         bool *touched,
                         uint32_t *consecutive)
{
    bool change = *touched ? reading < off_threshold
                           : reading > on_threshold;

    if (change) {
        (*consecutive)++;

        if (*consecutive >= DEBOUNCE_COUNT) {
            *touched = !*touched;
            *consecutive = 0;
        }
    } else {
        *consecutive = 0;
    }
}

void touch_sensor_init(int pin_number1, int pin_number2)
{
    gpio_init(pin_number1);
    gpio_disable_pulls(pin_number1);
    gpio_put(pin_number1, 0);
    gpio_set_dir(pin_number1, GPIO_OUT);

    gpio_init(pin_number2);
    gpio_disable_pulls(pin_number2);
    gpio_put(pin_number2, 0);
    gpio_set_dir(pin_number2, GPIO_OUT);

    touched1 = false;
    touched2 = false;
    consecutive1 = 0;
    consecutive2 = 0;

    uint32_t total1 = 0;
    uint32_t total2 = 0;

    // Keep both touch pads untouched during calibration.
    for (uint32_t i = 0; i < 64; ++i) {
        total1 += read_touch(pin_number1);
        total2 += read_touch(pin_number2);
        sleep_ms(10);
    }

    uint32_t baseline1 = total1 / 64;
    uint32_t margin1 = baseline1 * MARGIN_PERCENT / 100;
    if (margin1 < MIN_MARGIN_US) {
        margin1 = MIN_MARGIN_US;
    }

    on_threshold1 = baseline1 + margin1;
    off_threshold1 = baseline1 + margin1 / 2;

    uint32_t baseline2 = total2 / 64;
    uint32_t margin2 = baseline2 * MARGIN_PERCENT / 100;
    if (margin2 < MIN_MARGIN_US) {
        margin2 = MIN_MARGIN_US;
    }

    on_threshold2 = baseline2 + margin2;
    off_threshold2 = baseline2 + margin2 / 2;
}

bool touch_sensor_tick(int pin_number1, int pin_number2)
{
    uint32_t reading1 = read_touch(pin_number1);
    uint32_t reading2 = read_touch(pin_number2);

    update_touch(reading1, on_threshold1, off_threshold1,
                 &touched1, &consecutive1);

    update_touch(reading2, on_threshold2, off_threshold2,
                 &touched2, &consecutive2);

    return touched1 && touched2;
}