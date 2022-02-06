#pragma once

#include <pico/stdlib.h>

namespace Board {
class Button {
 public:
   enum class Position : uint {
      top_left = 19,
      top_right = 9,
      bottom = 8
   };

   static void init();
   static bool get(Position);
};
} // namespace Board