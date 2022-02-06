#include "Board/Button.hpp"

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
} // namespace Board