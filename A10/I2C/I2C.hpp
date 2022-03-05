#pragma once

#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#define I2C_PORT i2c1

namespace I2C {
void init();
}

#include "I2C/HDC1080/HDC1080.hpp"