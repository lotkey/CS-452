#include "Board/Dipswitch.hpp"

#include <pico/stdlib.h>
#include <hardware/gpio.h>

#include <vector>

namespace Board {
const std::vector<uint> Dipswitch::s_pins = {6, 7, 3, 2, 13, 26, 0, 1};

void Dipswitch::init() {
   for (uint pin : s_pins) {
      gpio_init(pin);
      gpio_set_dir(pin, GPIO_IN);
   }
}

bool Dipswitch::get(uint switch_num) {
   return gpio_get(s_pins[switch_num]);
}

std::vector<bool> Dipswitch::get() {
   std::vector<bool> status;
   for (uint pin : s_pins) {
      status.push_back(gpio_get(s_pins[pin]));
   }
}
} // namespace Board