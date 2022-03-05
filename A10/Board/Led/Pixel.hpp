#pragma once

#include "Board/Util/Util.hpp"

#include <hardware/pio.h>

#include <set>

namespace Board::Led {
class Pixel {
 public:
   static void init();
   static void set(int index, const Util::Color &color);
   static void set(const std::set<int>& indices, const Util::Color &color);
   static void show();

 private:
   static const int s_num_pixels = 4;
   static const int s_pin = 20;
   static const int s_sm = 0;

   static uint32_t s_data[s_num_pixels];
   static PIO s_pio;
};
} // namespace Board::Led