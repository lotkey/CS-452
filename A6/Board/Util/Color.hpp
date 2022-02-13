#pragma once

#include <pico/stdlib.h>

#include <cinttypes>

namespace Board::Util {
/// Class for RGB colors
class Color {
 public:
   std::uint8_t r;
   std::uint8_t g;
   std::uint8_t b;

   Color(std::uint8_t r = 0, std::uint8_t g = 0, std::uint8_t b = 0);
   /// @returns average of RGB values
   std::uint8_t brightness() const;
   
   /// Compares brightness
   bool operator>(const Color &) const;
   bool operator<(const Color &) const;
   bool operator<=(const Color &) const;
   bool operator>=(const Color &) const;

 private:
};
} // namespace Board::Util