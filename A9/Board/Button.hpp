#pragma once

#include <optional>
#include <pico/stdlib.h>

#include <map>

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
   static std::optional<Position> get();
   static bool get_debounced(Position pos);
   static std::optional<Position> get_debounced();

 private:
   static const uint64_t s_microsecond_wait = 10000;
   static std::map<Position, uint64_t> s_time_last_pressed;
   static std::map<Position, uint16_t> s_state;
};
} // namespace Board