#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H
static void hw_init_i2c(void);

static bool mpu_write_byte(uint8_t reg, uint8_t value);

static bool mpu_read_bytes(uint8_t reg, uint8_t *buf, size_t len);

static bool mpu_init(void) ;

static bool mpu_read_accel(int16_t *ax, int16_t *ay, int16_t *az)

static void init_hw(void);

int ballGameTest(void);


#endif
