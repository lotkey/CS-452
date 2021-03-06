#pragma once

#include <pico/stdlib.h>

#include <optional>

namespace Board {
/// Static class to interface with the buttons
class Button {
 public:
   /// enum for specifying buttons based on position
   enum class Position : uint { top_left = 19, top_right = 9, bottom = 8 };

   /// Initalize all needed IO
   static void init();
   /// @param pos which button to check
   /// @returns true if the button is pressed, false otherwise
   static bool get(Position pos);
   /// @returns The button being pressed, if any are
   static std::optional<Position> get();
};
} // namespace Board