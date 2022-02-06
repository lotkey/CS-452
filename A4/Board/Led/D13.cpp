#include "Board/Led/D13.hpp"

#include <hardware/gpio.h>
#include <pico/stdlib.h>

namespace Board::Led {
bool D13::s_is_on = false;

void D13::init() {
   gpio_init(s_led_pin);
   gpio_set_dir(s_led_pin, GPIO_OUT);
}

void D13::turn_on() {
   gpio_put(s_led_pin, true);
   s_is_on = true;
}

void D13::turn_off() {
   gpio_put(s_led_pin, false);
   s_is_on = false;
}

bool D13::invert() {
   s_is_on = !s_is_on;
   gpio_put(s_led_pin, s_is_on);
   return s_is_on;
}

void D13::set(bool on) {
   s_is_on = on;
   gpio_put(s_led_pin, s_is_on);
}

bool D13::is_on() { return s_is_on; }
} // namespace Board::Led