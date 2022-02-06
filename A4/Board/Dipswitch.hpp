#pragma once

#include <pico/stdlib.h>

#include <vector>

namespace Board {
class Dipswitch {
 public:
   static void init();
   static bool get(uint);
   static std::vector<bool> get();
 private:
   static const std::vector<uint> s_pins;
};
} // namespace Board