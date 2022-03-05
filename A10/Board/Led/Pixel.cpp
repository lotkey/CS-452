#include "Board/Led/Pixel.hpp"
#include "Board/Led/WS2812.pio.h"
#include "Board/Util/Util.hpp"

#include <hardware/pio.h>
#include <pico/stdlib.h>

#include <set>

namespace Board::Led {
uint32_t Pixel::s_data[s_num_pixels];

void Pixel::init() {
   uint offset = pio_add_program(pio0, &ws2812_program);
   ws2812_program_init(pio0, s_sm, offset, s_pin, 800000, 32);
}

void Pixel::set(int index, const Util::Color &color) {
   if (index < 0 || index >= s_num_pixels) {
      return;
   }

   s_data[index] = color;
}

void Pixel::set(const std::set<int> &indices, const Util::Color &color) {
   for (const auto &index : indices) {
      set(index, color);
   }
}

void Pixel::show() {
   for (const auto &d : s_data) {
      pio_sm_put_blocking(pio0, s_sm, d);
   }
}
} // namespace Board::Led