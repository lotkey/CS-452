#pragma once

#include "Board/Util/Util.hpp"

#include <hardware/pio.h>

#include <optional>
#include <set>
#include <vector>

namespace Board::Led {
class Pixel {
 public:
   static void init();
   static void set_brightness(float);
   static void set(int index, const Util::Color &color);
   static void set(const std::set<int> &indices, const Util::Color &color);
   static void set_sequence(int index, const std::vector<Util::Color> &sequence,
                            float sequence_index = 0);
   static void set_sequence(const std::set<int> &indices,
                            const std::vector<Util::Color> &sequence,
                            float sequence_index = 0);
   static void step_sequence(int index, float step_size);
   static void step_sequence(const std::set<int> &indices, float step_size);
   static void show();

 private:
   static const int s_num_pixels = 4;
   static const int s_pin = 20;
   static const int s_sm = 0;
   static const std::vector<Util::Color> s_default_sequence;

   static uint32_t s_data[s_num_pixels];
   static PIO s_pio;
   static std::vector<std::vector<Util::Color>> s_sequences;
   static float s_sequence_indices[s_num_pixels];
   static float s_brightness;
};
} // namespace Board::Led