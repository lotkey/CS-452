#include "Step_motor/Step_motor.hpp"

#include <pico/stdlib.h>

#include <iostream>
#include <set>

const std::set<Step_motor::Pin> Step_motor::s_pins = {Pin::A, Pin::B, Pin::C,
                                                      Pin::D};
const std::vector<std::set<Step_motor::Pin>> Step_motor::s_sequence = {
    {Pin::D}, {Pin::D, Pin::C}, {Pin::C}, {Pin::C, Pin::B},
    {Pin::B}, {Pin::B, Pin::A}, {Pin::A}, {Pin::A, Pin::D}};
int Step_motor::s_step = 0;
int Step_motor::s_tick = 0;
Step_motor::State Step_motor::s_state = State::Off;
bool Step_motor::s_alternating_is_cw = true;

void Step_motor::init() {
   for (const auto &pin : s_pins) {
      gpio_init((int)pin);
      gpio_set_dir((int)pin, GPIO_OUT);
   }
}

int Step_motor::min_us_between_steps() { return s_min_us_between_steps; }

int Step_motor::max_us_between_steps() { return s_max_us_between_steps; }

void Step_motor::set_state(State state) { s_state = state; }

float Step_motor::percent_through_revolution() {
   float percent = s_tick / (float)s_seqs_per_revolution;

   switch (s_state) {
   case State::Clockwise: {
      return percent;
   }
   case State::CounterClockwise: {
      return 1.0 - percent;
   }
   case State::Alternating: {
      if (s_alternating_is_cw) {
         return percent;
      } else {
         return 1.0 - percent;
      }
   }
   default: {
      return 0.0;
   }
   }
}

bool Step_motor::step() {
   bool b = false;

   switch (s_state) {
   case State::Clockwise: {
      std::cout << "stepped clockwise" << std::endl;
      b = step(true);
      break;
   }
   case State::CounterClockwise: {
      b = step(false);
      std::cout << "stepped counterclockwise" << std::endl;
      break;
   }
   case State::Alternating: {
      b = step(s_alternating_is_cw);
      std::cout << "stepped alternating" << std::endl;
      break;
   }
   }

   if (b) {
      s_tick++;
      if (s_tick >= s_seqs_per_revolution) {
         s_tick = 0;
         s_alternating_is_cw = !s_alternating_is_cw;
      }
   }

   return b;
}

bool Step_motor::step(bool clockwise) {
   if (clockwise) {
      s_step++;
   } else {
      s_step--;
   }
   s_step %= s_sequence.size();
   set_pins();
   return s_step == 0;
}

void Step_motor::set_pins() {
   auto step = s_sequence[s_step];
   for (const auto &pin : s_pins) {
      gpio_put((int)pin, step.contains(pin));
   }
}