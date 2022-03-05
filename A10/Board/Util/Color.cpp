#include "Board/Util/Color.hpp"

#include <string>
#include <tuple>
#include <vector>

namespace Board::Util {
const std::vector<Color> Color::Rainbow = {
    {148, 0, 211}, {75, 0, 130},  {0, 0, 255}, {0, 255, 0},
    {255, 255, 0}, {255, 127, 0}, {255, 0, 0}};

Color::Color(uint32_t color)
    : m_r(color & 0xFF), m_g((color & 0xFF00) >> 8),
      m_b((color & 0xFF0000) >> 16), m_w((color & 0xFF000000) >> 24) {}

Color::Color(int r, int g, int b, int w) {
   auto [rf, gf, bf, wf] = format(r, g, b, w);

   m_r = rf;
   m_g = gf;
   m_b = bf;
   m_w = wf;
}

Color::Color(const std::tuple<int, int, int, int> &rgbw) {
   auto [r, g, b, w] = format(rgbw);

   m_r = r;
   m_g = g;
   m_b = b;
   m_w = w;
}

uint8_t Color::brightness() const {
   int sum = m_r + m_g + m_b + m_w;
   uint8_t average = sum / 4;
   return average;
}

std::string Color::to_string() const {
   return "(" + std::to_string(m_r) + ", " + std::to_string(m_g) + ", " +
          std::to_string(m_b) + ", " + std::to_string(m_w) + ")";
}

bool Color::operator>(const Color &c2) const {
   return brightness() > c2.brightness();
}

bool Color::operator<(const Color &c2) const {
   return brightness() < c2.brightness();
}

bool Color::operator>=(const Color &c2) const {
   return brightness() >= c2.brightness();
}

bool Color::operator<=(const Color &c2) const {
   return brightness() <= c2.brightness();
}

Color Color::operator*(float scalar) const {
   return Color(
       format({m_r * scalar, m_g * scalar, m_b * scalar, m_w * scalar}));
}

Color Color::operator/(float scalar) const {
   return Color(
       format({m_r / scalar, m_g / scalar, m_b / scalar, m_w / scalar}));
}

Color Color::operator+(const Color &c) const {
   return Color(format({m_r + c.m_r, m_g + c.m_g, m_b + c.m_b, m_w + c.m_w}));
}

Color Color::operator-(const Color &c) const {
   return Color(format({m_r - c.m_r, m_g - c.m_g, m_b - c.m_b, m_w - c.m_w}));
}

Color::operator uint32_t() const {
   return (uint32_t)m_r << 24 | (uint32_t)m_g << 16 | (uint32_t)m_b << 8 |
          (uint32_t)m_w;
}

std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> Color::format(int r, int g,
                                                             int b, int w) {
   std::vector<int> rgbw = {r, g, b, w};

   for (auto &p : rgbw) {
      if (p > 255) {
         p = 255;
      } else if (p < 0) {
         p = 0;
      }
   }

   return {rgbw[0], rgbw[1], rgbw[2], rgbw[3]};
}

std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
Color::format(const std::tuple<int, int, int, int> &rgbw) {
   auto [r, g, b, w] = rgbw;
   return format(r, g, b, w);
}
} // namespace Board::Util