#pragma once

#include <pico/stdlib.h>

#include <string>
#include <tuple>
#include <vector>

namespace Board::Util {
/// Class for RGB colors
class Color {
 public:
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

namespace Palettes {
namespace Rainbow {
static const Color Red = {255, 0, 0};
static const Color Orange = {255, 127, 0};
static const Color Yellow = {255, 255, 0};
static const Color Green = {0, 255, 0};
static const Color Blue = {0, 0, 255};
static const Color Indigo = {75, 0, 130};
static const Color Violet = {148, 0, 211};
static const std::vector<Color> Sequence = {Red,  Orange, Yellow, Green,
                                            Blue, Indigo, Violet};
} // namespace Rainbow

namespace Synthwave {
static const Color Cyan = {109, 241, 216};
static const Color MutedCyan = {93, 164, 166};
static const Color BlueViolet = {60, 52, 92};
static const Color Purple = {92, 33, 109};
static const Color Magenta = {211, 12, 184};
static const std::vector<Color> Sequence = {Cyan, MutedCyan, BlueViolet, Purple,
                                            Magenta};
} // namespace Synthwave

namespace Sunset {
static const Color Gold = {255, 211, 25};
static const Color Orange = {255, 144, 31};
static const Color Pink = {255, 41, 117};
static const Color Magenta = {242, 34, 255};
static const Color Purple = {140, 30, 255};
static const std::vector<Color> Sequence = {Gold, Orange, Pink, Magenta,
                                            Purple};
} // namespace Sunset

namespace Pastel {
static const Color Red = {255, 179, 186};
static const Color Orange = {255, 223, 186};
static const Color Yellow = {255, 255, 186};
static const Color Green = {186, 255, 201};
static const Color Blue = {186, 255, 255};
static const std::vector<Color> Sequence = {Red, Orange, Yellow, Green, Blue};
} // namespace Pastel
} // namespace Palettes
} // namespace Board::Util