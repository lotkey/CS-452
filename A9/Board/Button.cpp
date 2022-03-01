#include "Board/Button.hpp"

#include <hardware/gpio.h>
#include <pico/stdlib.h>

#include <optional>
#include <vector>

namespace Board {
std::map<Button::Position, uint64_t> Button::s_time_last_pressed;
std::map<Button::Position, uint16_t> Button::s_state;

void Button::init() {
   gpio_init((uint)Position::top_left);
   gpio_init((uint)Position::top_right);
   gpio_init((uint)Position::bottom);

   gpio_set_dir((uint)Position::top_left, GPIO_IN);
   gpio_set_dir((uint)Position::top_right, GPIO_IN);
   gpio_set_dir((uint)Position::bottom, GPIO_IN);

   s_time_last_pressed[Position::top_left] = 0;
   s_time_last_pressed[Position::top_right] = 0;
   s_time_last_pressed[Position::bottom] = 0;
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

bool Button::get_debounced(Position pos) {
   uint64_t absolute_time_us = to_us_since_boot(get_absolute_time());
   bool is_on = get(pos);
   s_state[pos] = (s_state[pos] << 1) | is_on | 0xfe00;

   return s_state[pos] == 0xff00;
}

std::optional<Button::Position> Button::get_debounced() {
   std::vector<Position> positions = {Position::top_left, Position::top_right,
                                      Position::bottom};
   for (const auto &position : positions) {
      if (get_debounced(position)) {
         return position;
      }
   }
   return {};
}
} // namespace Board