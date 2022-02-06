#pragma once

#include <pico/stdlib.h>

#include <vector>

namespace Board {
/// Static class to interface with the dipswitches
class Dipswitch {
 public:
   /// Initialize all IO
   static void init();
   /// @param index which dipswitch to check [0-7]
   /// @returns true if the dipswitch is flipped to on, false otherwise
   static bool get(uint index);
   /// @returns a vector of all dipswitch states
   static std::vector<bool> get();

 private:
   static const std::vector<uint> s_pins;
};
} // namespace Board