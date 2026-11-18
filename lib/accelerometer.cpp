
#include "accelerometer.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "cmath"

static absolute_time_t start;
static absolute_time_t end;
static float runningAvg;

static bool write_reg(uint8_t reg, uint8_t value) {
    uint8_t data[] = {reg, value};
    int result = i2c_write_timeout_us(
        I2C_PORT, MPU_ADDR, data, 2, false, TIMEOUT_US);

    if (result != 2) {
        printf("Write register 0x%02X failed: %d\n", reg, result);
        return false;
    }
    return true;
}

static bool read_regs(uint8_t reg, uint8_t *data, size_t count) {
    int result = i2c_write_timeout_us(
        I2C_PORT, MPU_ADDR, &reg, 1, true, TIMEOUT_US);

    if (result != 1) {
        printf("Select register 0x%02X failed: %d\n", reg, result);
        return false;
    }

    result = i2c_read_timeout_us(
        I2C_PORT, MPU_ADDR, data, count, false, TIMEOUT_US);

    if (result != (int)count) {
        printf("Read register 0x%02X failed: %d\n", reg, result);
        return false;
    }
    return true;
}

static int16_t signed_word(const uint8_t *data) {
    return (int16_t)(((uint16_t)data[0] << 8) | data[1]);
}
void accelerometer_init(){
    stdio_init_all();

    i2c_init(I2C_PORT, 100000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    sleep_ms(2000); //idk if necessary

    // Retry so startup diagnostics remain visible after USB connects.
    while (true) {
        uint8_t identity;

        if (read_regs(0x75, &identity, 1)) {
            printf("WHO_AM_I = 0x%02X; expected 0x70\n", identity);
        if (identity == 0x70) {
            printf("MPU-6500 detected\n");
            break;
}
            printf("Unexpected sensor identity; check sensor model.\n");
        } else {
            printf("Cannot communicate with MPU at 0x%02X.\n", MPU_ADDR);
        }

        sleep_ms(1000);
    }
    // Reset, then allow the device to restart.
    // if (!write_reg(0x6B, 0x80)) return 1;
    // sleep_ms(100);

    // Wake, enable all axes, and explicitly select measurement ranges.
    if (!write_reg(0x6B, 0x01) ||
        !write_reg(0x6C, 0x00) ||
        !write_reg(0x1B, 0x00) ||  // Gyro: +/-250 degrees/s
        !write_reg(0x1C, 0x00)) {  // Accel: +/-2 g
    }

    start = get_absolute_time();
    
    runningAvg = 0.0f;
}
bool accelerometer_tick(void) {
    

    
    
    uint8_t data[14];

    end = get_absolute_time();
    int64_t diff_ms = absolute_time_diff_us(start, end) / 1000.0f;
    start = get_absolute_time();
    // printf("Shake detection");
    if (read_regs(0x3B, data, sizeof(data))) {
        float x = signed_word(&data[0]) / 16384.0f;
        float y = signed_word(&data[2]) / 16384.0f;
        float z = signed_word(&data[4]) / 16384.0f;
        
        // printf(
        //     "Accel (g): X=%.2f Y=%.2f Z=%.2f | "
        //     "Gyro (deg/s): X=%.1f Y=%.1f Z=%.1f\n",
        //     x,
        //     y,
        //     z,
        //     signed_word(&data[8]) / 131.0f,
        //     signed_word(&data[10]) / 131.0f,
        //     signed_word(&data[12]) / 131.0f);
        

        float magnitude = x*x+y*y+z*z;
        magnitude = std::sqrt(magnitude); //Get magnitude
        magnitude -= 1; //adjust for gravity (kinda)

        float avg = runningAvg/3000.0f;

        runningAvg = runningAvg + magnitude * diff_ms - avg * diff_ms;
        
        bool shaking = false;
        if (runningAvg >= 500.0f){ //should be 2500
            shaking = true;
                // printf("Avg: %.2f \n",runningAvg);
        }
        return shaking;
        




    }
    else{
        return NULL;
    }

}