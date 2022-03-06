#include "Board/Led/Pixel.hpp"
#include "Board/Led/WS2812.pio.h"
#include "Board/Util/Util.hpp"

#include <hardware/pio.h>
#include <pico/stdlib.h>

#include <set>

namespace Board::Led {
PIO Pixel::s_pio;
std::vector<std::vector<Util::Color>> Pixel::s_sequences;
float Pixel::s_sequence_indices[s_num_pixels];
uint32_t Pixel::s_data[s_num_pixels];
float Pixel::s_brightness;
const std::vector<Util::Color> Pixel::s_default_sequence = {{0, 0, 0, 0}};

void Pixel::init() {
   uint offset = pio_add_program(pio0, &ws2812_program);
   ws2812_program_init(pio0, s_sm, offset, s_pin, 800000, 32);

   for (int i = 0; i < s_num_pixels; i++) {
      s_sequences.push_back(s_default_sequence);
   }
}

void Pixel::set_brightness(float brightness) { s_brightness = brightness; }

void Pixel::set(int index, const Util::Color &color) {
   if (index < 0 || index >= s_num_pixels) {
      return;
   }

   s_data[index] = color * s_brightness;
}

void Pixel::set(const std::set<int> &indices, const Util::Color &color) {
   for (const auto &index : indices) {
      set(index, color);
   }
}

void Pixel::set_sequence(int index, const std::vector<Util::Color> &sequence,
                         float sequence_index) {
   if (index < 0 || index >= s_num_pixels) {
      return;
   }

   s_sequences[index] = sequence;
   s_sequence_indices[index] = sequence_index;
}

void Pixel::set_sequence(const std::set<int> &indices,
                         const std::vector<Util::Color> &sequence,
                         float sequence_index) {
   for (const auto &index : indices) {
      set_sequence(index, sequence, sequence_index);
   }
}

void Pixel::step_sequence(int index, float step_size) {
   if (index < 0 || index >= s_num_pixels) {
      return;
   }

   s_sequence_indices[index] += step_size;
   while (s_sequence_indices[index] > s_sequences[index].size()) {
      s_sequence_indices[index] =
          s_sequences[index].size() - s_sequence_indices[index];
   }

   float c2_blend = s_sequence_indices[index] - (int)s_sequence_indices[index];
   float c1_blend = 1.0 - c2_blend;
   Util::Color c1 = s_sequences[index][(int)s_sequence_indices[index]];
   Util::Color c2 = s_sequences[index][((int)s_sequence_indices[index] + 1) %
                                       s_sequences[index].size()];
   Util::Color c3 = c1 * c1_blend + c2 * c2_blend;
   set(index, c3);
}

void Pixel::step_sequence(const std::set<int> &indices, float step_size) {
   for (const auto &index : indices) {
      step_sequence(index, step_size);
   }
}

void Pixel::show() {
   for (const auto &d : s_data) {
      pio_sm_put_blocking(pio0, s_sm, d);
   }
}
} // namespace Board::Led