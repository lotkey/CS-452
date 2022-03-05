#include "I2C/I2C.hpp"

#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/binary_info/code.h"
#include "pico/stdlib.h"

namespace I2C {
void init() {
   i2c_init(I2C_PORT, 100 * 1000);
   gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
   gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
   gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
   gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
   bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN,
                              PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
}
} // namespace I2C