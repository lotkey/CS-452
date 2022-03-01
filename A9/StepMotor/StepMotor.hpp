#pragma once

#include <set>
#include <vector>

class StepMotor {
 public:
   static void init();
   static void step(bool clockwise = true);
   static void turn_off();

 private:
   enum class Pin : int { A = 0, B = 1, C = 2, D = 3 };

   static const std::set<Pin> s_pins;
   static const std::vector<std::set<Pin>> s_sequence;
   static int s_step;

   static void set_pins();
};