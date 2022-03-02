#pragma once

#include <set>
#include <vector>

class Step_motor {
 public:
   enum class State { Clockwise, CounterClockwise, Alternating, Off };

   static void init();
   static void set_state(State);
   static float percent_through_revolution();
   static int min_us_between_steps();
   static int max_us_between_steps();
   static bool step();

 private:
   enum class Pin : int { A = 0, B = 1, C = 12, D = 13 };

   static const int s_seqs_per_revolution = 525;
   static const int s_min_us_between_steps = 800;
   static const int s_max_us_between_steps = 5000;
   static const std::set<Pin> s_pins;
   static const std::vector<std::set<Pin>> s_sequence;
   static int s_step;
   static int s_tick;
   static bool s_alternating_is_cw;
   static State s_state;

   static bool step(bool clockwise);
   static void set_pins();
};