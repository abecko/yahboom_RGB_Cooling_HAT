#ifndef COOLING_HAT_I2C_H
#define COOLING_HAT_I2C_H

#include <stdbool.h>

enum i2c_register {
    i2c_red_register = 0x00,
    i2c_green_register = 0x01,
    i2c_blue_register = 0x02,
    i2c_rgb_effect_register = 0x04,
    i2c_rgb_register = 0x07,
    i2c_fan_register = 0x08,
};

bool i2c_init();
void i2c_write(enum i2c_address reg, uint8_t data);

#endif //COOLING_HAT_I2C_H
