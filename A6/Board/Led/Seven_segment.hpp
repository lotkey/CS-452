#pragma once

#include <pico/stdlib.h>

#include <map>
#include <set>
#include <tuple>

namespace Board::Led {
/// "Static" class to control the 7-segment LED
class Seven_segment {
 public:
   enum class Side : uint { neither = 00, left = 10, right = 01, both = 11 };

   enum class Segment : uint {
      north = 26,
      northeast = 27,
      southeast = 29,
      south = 18,
      southwest = 25,
      northwest = 7,
      center = 28,
      cc1 = 11,
      cc2 = 10,
      decimal = 24
   };

   /// Initializes all IO needed for the LED
   static void init();

   /// Turns off all LEDs/clears display
   static void clear();

   /// Converts from uint to a char
   /// @param ui single-digit unsigned int to convert from
   /// @returns the char representation of the int: returns '1' when given 1
   static char uint_to_char(uint ui);

   /// Displays a character to the LED
   /// @param c the char to display
   /// @param side which side(s) to display to
   /// @returns true if successful, false if there is no 7-segment
   /// representation of the char
   static bool display_char(char c, Side side);

   /// Displays the two rightmost digits of an unsigned int
   /// Displays 14 when given 14, 00 when given 100, 01 when given 1, etc.
   /// @param ui uint to display
   static void display_uint(uint ui);

 private:
   static const std::set<Segment> s_segments;
   static const std::map<char, std::set<Segment>> s_chars_as_segments;
   static const std::map<Side, std::tuple<bool, bool>> s_side_to_bool;

};
} // namespace Led