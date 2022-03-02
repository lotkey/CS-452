#include "Board/Led/Seven_segment.hpp"

#include <hardware/gpio.h>
#include <pico/stdlib.h>

#include <set>

namespace Board::Led {
bool Seven_segment::s_tick = false;

const std::set<Seven_segment::Segment> Seven_segment::s_segments = {
    Segment::north,  Segment::northeast, Segment::southeast,
    Segment::south,  Segment::southwest, Segment::northwest,
    Segment::center, Segment::decimal};

const std::map<char, std::set<Seven_segment::Segment>>
    Seven_segment::s_chars_as_segments{
        {' ', {}},
        {'-', {Segment::center}},
        {'.', {Segment::decimal}},
        {'_', {Segment::south}},
        {',', {{Segment::southeast, Segment::south}}},
        {'\'', {{Segment::northwest}}},
        {'"', {{Segment::northeast, Segment::northwest}}},
        {'!', {{Segment::northeast, Segment::decimal}}},
        {'(',
         {Segment::north, Segment::northwest, Segment::southwest,
          Segment::south}},
        {')',
         {Segment::north, Segment::northeast, Segment::southeast,
          Segment::south}},

        {'0',
         {Segment::north, Segment::northeast, Segment::northwest,
          Segment::southeast, Segment::southwest, Segment::south}},
        {'1', {Segment::northeast, Segment::southeast}},
        {'2',
         {Segment::north, Segment::northeast, Segment::center,
          Segment::southwest, Segment::south}},
        {'3',
         {Segment::north, Segment::northeast, Segment::center,
          Segment::southeast, Segment::south}},
        {'4',
         {Segment::northeast, Segment::southeast, Segment::center,
          Segment::northwest}},
        {'5',
         {Segment::north, Segment::northwest, Segment::center,
          Segment::southeast, Segment::south}},
        {'6',
         {Segment::north, Segment::northwest, Segment::center,
          Segment::southeast, Segment::southwest, Segment::south}},
        {'7', {Segment::northeast, Segment::southeast, Segment::north}},
        {'8',
         {Segment::north, Segment::northeast, Segment::northwest,
          Segment::center, Segment::southeast, Segment::southwest,
          Segment::south}},
        {'9',
         {Segment::north, Segment::northeast, Segment::northwest,
          Segment::center, Segment::southeast, Segment::south}},

        {'A',
         {Segment::north, Segment::northeast, Segment::northwest,
          Segment::center, Segment::southeast, Segment::southwest}},
        {'B',
         {Segment::northwest, Segment::center, Segment::southeast,
          Segment::southwest, Segment::south}},
        {'C',
         {Segment::north, Segment::northwest, Segment::southwest,
          Segment::south}},
        {'D',
         {Segment::northeast, Segment::southeast, Segment::southwest,
          Segment::south}},
        {'E',
         {Segment::north, Segment::northwest, Segment::center,
          Segment::southwest, Segment::south}},
        {'F',
         {Segment::north, Segment::northwest, Segment::center,
          Segment::southwest}},
        {'G',
         {Segment::north, Segment::northwest, Segment::southeast,
          Segment::southwest, Segment::south}},
        {'H',
         {Segment::northwest, Segment::center, Segment::southeast,
          Segment::southwest}},
        {'I', {Segment::northwest, Segment::southwest}},
        {'J',
         {Segment::northeast, Segment::southeast, Segment::southwest,
          Segment::south}},
        {'K',
         {Segment::north, Segment::northwest, Segment::center,
          Segment::southeast, Segment::southwest}},
        {'L', {Segment::northwest, Segment::southwest, Segment::south}},
        {'M', {Segment::north, Segment::southeast, Segment::southwest}},
        {'N',
         {Segment::north, Segment::northeast, Segment::northwest,
          Segment::southeast, Segment::southwest}},
        {'O',
         {Segment::north, Segment::northeast, Segment::northwest,
          Segment::southeast, Segment::southwest, Segment::south}},
        {'P',
         {Segment::north, Segment::northeast, Segment::northwest,
          Segment::center, Segment::southwest}},
        {'R',
         {Segment::north, Segment::northeast, Segment::northwest,
          Segment::southwest}},
        {'S',
         {Segment::north, Segment::northwest, Segment::center,
          Segment::southeast, Segment::south}},
        {'T',
         {Segment::northwest, Segment::center, Segment::southwest,
          Segment::south}},
        {'U',
         {Segment::northeast, Segment::northwest, Segment::southeast,
          Segment::southwest, Segment::south}},
        {'V',
         {Segment::northeast, Segment::northwest, Segment::southeast,
          Segment::south}},
        {'W', {Segment::northeast, Segment::northwest, Segment::south}},
        {'X',
         {Segment::northeast, Segment::northwest, Segment::center,
          Segment::southeast, Segment::southwest}},
        {'Y',
         {Segment::northeast, Segment::northwest, Segment::center,
          Segment::southeast, Segment::south}},
        {'Z',
         {Segment::north, Segment::northeast, Segment::center,
          Segment::south}}};

const std::map<Seven_segment::Side, std::tuple<bool, bool>>
    Seven_segment::s_side_to_bool = {
        {Seven_segment::Side::neither, {false, false}},
        {Seven_segment::Side::left, {true, false}},
        {Seven_segment::Side::right, {false, true}},
        {Seven_segment::Side::both, {false, false}}};

void Seven_segment::init() {
   for (const auto &seg : s_segments) {
      gpio_init((uint)seg);
      gpio_set_dir((uint)seg, GPIO_OUT);
   }

   gpio_init((uint)Segment::cc1);
   gpio_init((uint)Segment::cc2);
   gpio_set_dir((uint)Segment::cc1, GPIO_OUT);
   gpio_set_dir((uint)Segment::cc2, GPIO_OUT);
}

void Seven_segment::clear() {
   gpio_put((uint)Segment::cc1, true);
   gpio_put((uint)Segment::cc2, true);
   for (const auto &segment : s_segments) {
      gpio_put((uint)segment, 0);
   }
}

char Seven_segment::uint_to_char(uint i) { return (i % 10) + 48; }

bool Seven_segment::set(char c, Side side) {
   if (!s_chars_as_segments.contains(c)) {
      return false;
   }

   auto [left_side, right_side] = s_side_to_bool.at(side);
   gpio_put((uint)Segment::cc1, right_side);
   gpio_put((uint)Segment::cc2, left_side);

   for (const auto &segment : s_chars_as_segments.at(c)) {
      gpio_put((uint)segment, 1);
   }

   return true;
}

void Seven_segment::display_uint(uint ui) {
   clear();

   if (s_tick) {
      uint right_digit = ui % 10;
      set(uint_to_char(right_digit), Side::right);
   } else {
      uint left_digit = ui % 100 / 10;
      set(uint_to_char(left_digit), Side::left);
   }

   s_tick = !s_tick;
}

void Seven_segment::set(Segment seg, Side side) {
   auto [left_side, right_side] = s_side_to_bool.at(side);
   gpio_put((uint)Segment::cc1, right_side);
   gpio_put((uint)Segment::cc2, left_side);

   gpio_put((uint)seg, 1);
}
} // namespace Board::Led