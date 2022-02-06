#include "Board/Util/Color.hpp"

namespace Board::Util {
Color::Color(std::uint8_t r, std::uint8_t g, std::uint8_t b)
    : r(r), g(g), b(b) {}

std::uint8_t Color::brightness() const {
   int sum = r + g + b;
   std::uint8_t average = sum / 3;
   return average;
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
} // namespace Board::Util