#pragma once

#include <pico/stdlib.h>

#include <string>
#include <tuple>
#include <vector>

namespace Board::Util {
/// Class for RGB colors
class Color {
 public:
   static const std::vector<Color> Rainbow;

   Color(int r = 0, int g = 0, int b = 0, int w = 0);
   Color(const std::tuple<int, int, int, int> &);
   Color(uint32_t);
   operator uint32_t() const;
   /// @returns average of RGB values
   uint8_t brightness() const;
   std::string to_string() const;

   /// Compares brightness
   bool operator>(const Color &) const;
   bool operator<(const Color &) const;
   bool operator<=(const Color &) const;
   bool operator>=(const Color &) const;

   Color operator*(float scalar) const;
   Color operator/(float scalar) const;
   Color operator+(const Color &c) const;
   Color operator-(const Color &c) const;

 private:
   uint8_t m_r;
   uint8_t m_g;
   uint8_t m_b;
   uint8_t m_w;

   static std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
   format(int r = 0, int g = 0, int b = 0, int w = 0);
   static std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
   format(const std::tuple<int, int, int, int> &rgbw);
};
} // namespace Board::Util