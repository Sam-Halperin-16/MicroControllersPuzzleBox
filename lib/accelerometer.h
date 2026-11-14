#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

// #include "../ST7735_TFT.h"
// #include "hw.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1
#define MPU_ADDR 0x68
#define TIMEOUT_US 10000



static bool write_reg(uint8_t reg, uint8_t value);

static bool read_regs(uint8_t reg, uint8_t *data, size_t count);

static int16_t signed_word(const uint8_t *data);

bool tick(void);

#endif
