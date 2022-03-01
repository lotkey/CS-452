#include "Board/Button.hpp"

#include <hardware/gpio.h>

#include <optional>
#include <vector>

namespace Board {
void Button::init() {
   gpio_init((uint)Position::top_left);
   gpio_init((uint)Position::top_right);
   gpio_init((uint)Position::bottom);

   gpio_set_dir((uint)Position::top_left, GPIO_IN);
   gpio_set_dir((uint)Position::top_right, GPIO_IN);
   gpio_set_dir((uint)Position::bottom, GPIO_IN);
}

bool Button::get(Position pos) { return gpio_get((uint)pos); }

std::optional<Button::Position> Button::get() {
   std::vector<Position> positions = {Position::top_left, Position::top_right,
                                      Position::bottom};
   for (const auto &position : positions) {
      if (get(position)) {
         return position;
      }
   }
   return {};
}
} // namespace Board