#ifndef TOUCH_SENSOR_H
#define TOUCH_SENSOR_H

#include <stdbool.h>

#define SAMPLE_COUNT    16u
#define TIMEOUT_US      2000u
#define MIN_MARGIN_US   5u
#define MARGIN_PERCENT  30u
#define DEBOUNCE_COUNT  3u

void touch_sensor_init(int pin_number1, int pin_number2);
bool touch_sensor_tick(int pin_number1, int pin_number2);

#endif