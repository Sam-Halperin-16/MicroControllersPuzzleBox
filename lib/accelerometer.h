#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1
#define MPU_ADDR 0x68
#define TIMEOUT_US 10000

void accelerometer_init();
bool accelerometer_tick();

#endif