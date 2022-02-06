#pragma once

#include <pico/stdlib.h>

#include <cinttypes>

namespace Board::Util {
class Color {
 public:
   Color(std::uint8_t r = 0, std::uint8_t g = 0, std::uint8_t b = 0);
   std::uint8_t r;
   std::uint8_t g;
   std::uint8_t b;

   std::uint8_t brightness() const;
   bool operator>(const Color&) const;
   bool operator<(const Color&) const;
   bool operator<=(const Color&) const;
   bool operator>=(const Color&) const;

 private:
};
} // namespace Board::Util