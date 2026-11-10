#include "ST7735_TFT.h"
#include "hw.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include "hardware/i2c.h"


// Some Pico toolchain configurations do not expose snprintf from stdio.h.
extern int snprintf(char *restrict, size_t, const char *restrict, ...);
extern int rand(void);


#define MPU6050_ADDR         0x68
#define MPU6050_REG_PWR1     0x6B //power manage reg//wake up
#define MPU6050_REG_ACCEL_XH 0x3B// x, y, z acce data
#define MPU6050_REG_CONFIG   0x1A // for low pass filter
#define MPU6050_REG_ACCELCFG 0x1C //g config reg
#define MPU6050_REG_SMPLRT   0x19  //sample rate divider




#define I2C_PORT i2c0
#define I2C_SDA  4
#define I2C_SCL  5


#define SCREEN_W 128
#define SCREEN_H 160
#define BALL_R   4


static void hw_init_i2c(void){
    i2c_init(I2C_PORT, 400 * 1000);              // start i2c0 at 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);   // GPIO4 = SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);   // GPIO5 = SCL
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}


static bool mpu_write_byte(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = { reg, value };
    return i2c_write_blocking(I2C_PORT, MPU6050_ADDR, buf, 2, false) == 2;
}


static bool mpu_read_bytes(uint8_t reg, uint8_t *buf, size_t len) {
    if (i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true) != 1) {
        return false;
    }
    return i2c_read_blocking(I2C_PORT, MPU6050_ADDR, buf, len, false) == (int)len;
}


static bool mpu_init(void) {
    sleep_ms(100);
    if (!mpu_write_byte(MPU6050_REG_PWR1, 0x00)) return false;   // wake up
    sleep_ms(10);
    if (!mpu_write_byte(MPU6050_REG_CONFIG, 0x03)) restatic bool mpu_read_accel(int16_t *ax, int16_t *ay, int16_t *az)turn false; // small low-pass filter
    if (!mpu_write_byte(MPU6050_REG_SMPLRT, 0x07)) return false; // sample divider
    if (!mpu_write_byte(MPU6050_REG_ACCELCFG, 0x00)) return false; // +/-2g
    return true;
}


static bool mpu_read_accel(int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t buf[6];
    if (!mpu_read_bytes(MPU6050_REG_ACCEL_XH, buf, 6)) {
        return false;
    }
    *ax = (int16_t)((buf[0] << 8) | buf[1]);
    *ay = (int16_t)((buf[2] << 8) | buf[3]);
    *az = (int16_t)((buf[4] << 8) | buf[5]);
    return true;
}






static void init_hw(void) {
    stdio_init_all();


    spi_init(SPI_PORT, 1000000);
    gpio_set_function(SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX, GPIO_FUNC_SPI);


    tft_spi_init();
}


int ballGameTest(void) {
    init_hw();
    hw_init_i2c();
    TFT_BlackTab_Initialize();
    setTextWrap(true);


    fillScreen(ST7735_BLACK);
    drawText(10, 20, "Lab7 I2C", ST7735_WHITE, ST7735_BLACK, 1);
    drawText(10, 40, "zjm and spsh", ST7735_WHITE, ST7735_BLACK, 1);
    drawRectWH(0, 80, SCREEN_W, SCREEN_H - 80, ST7735_WHITE);


    if (!mpu_init()) {
        drawText(8, 50, "MPU6050 init fail", ST7735_RED, ST7735_BLACK, 1);
        while (true) {
            sleep_ms(1000);
        }
    }




 // Simple startup calibration while board is kept still.
    int32_t ax_sum = 0;
    int32_t ay_sum = 0;
    int32_t az_sum = 0;
    int16_t ax, ay, az;


    drawText(8, 50, "Keep board still...", ST7735_YELLOW, ST7735_BLACK, 1);


    for (int k = 0; k < 100; k++) {    //take 100 samples and add them up
        if (mpu_read_accel(&ax, &ay, &az)) {
            ax_sum += ax;
            ay_sum += ay;
            az_sum += az;
        }
        sleep_ms(10);
    }


    fillRect(8, 50, 120, 10, ST7735_BLACK);


// average 100 xyz raw values
    float ax_offset = ax_sum / 100.0f;  //average 100 samples
    float ay_offset = ay_sum / 100.0f;
    float az_offset = az_sum / 100.0f;


    float x = SCREEN_W / 2.0f;    //put ball in the center
    float y = (SCREEN_H + 34) / 2.0f;  
    float vx = 0.0f;  // volecity will change with tilt
    float vy = 0.0f;


    int old_x = (int)x;
    int old_y = (int)y;


    int coin_x = SCREEN_W / 2;
    int coin_y = (SCREEN_H + 34) / 1.5;


    fillCircle(old_x, old_y, BALL_R, ST7735_WHITE);


    while (true) {
        if (mpu_read_accel(&ax, &ay, &az)) {
            // Convert to g units for +/-2g mode
            float ax_g = (ax - ax_offset) / 16384.0f; //16384=1g gravity
            float ay_g = (ay - ay_offset) / 16384.0f;
            (void)az_offset; //just say not being used


            // Adjust these two lines if screen motion feels reversed.
            float tilt_x = ay_g;
            float tilt_y = ax_g;


            // "Water drop" style motion: acceleration + damping
            vx = 0.95f * vx + 2.50f * tilt_x;
            vy = 0.95f * vy + 2.50f * tilt_y;


            x += vx;
            y += vy;


            // Keep the ball inside the play area with a soft bounce
            if (x < BALL_R + 1) {
                x = BALL_R + 1;
                vx *= -0.35f;
            }
            if (x > SCREEN_W - BALL_R - 2) {
                x = SCREEN_W - BALL_R - 2;
                vx *= -0.35f;
            }
            if (y < 81 + BALL_R) {
                y = 81 + BALL_R;
                vy *= -0.35f;
            }
            if (y > SCREEN_H - BALL_R - 2) {
                y = SCREEN_H - BALL_R - 2;
                vy *= -0.35f;
            }


            int new_x = (int)x;
            int new_y = (int)y;


            if (new_x != old_x || new_y != old_y) {
                fillCircle(old_x, old_y, BALL_R, ST7735_BLACK);
                fillCircle(new_x, new_y, BALL_R, ST7735_WHITE);
                old_x = new_x;
                old_y = new_y;
            }


            char msg[32];
            float az_g = (az - az_offset) / 16384.0f;


            snprintf(msg, sizeof(msg), "X:%1.2f   ", ax_g);
            drawText(4, 50, msg, ST7735_GREEN, ST7735_BLACK, 1);


            snprintf(msg, sizeof(msg), "Y:%1.2f   ", ay_g);
            drawText(4, 60, msg, ST7735_GREEN, ST7735_BLACK, 1);


            snprintf(msg, sizeof(msg), "Z:%1.2f   ", az_g);
            drawText(4, 70, msg, ST7735_GREEN, ST7735_BLACK, 1);
        }


        sleep_ms(20);


    }
   
    return 0;
}
