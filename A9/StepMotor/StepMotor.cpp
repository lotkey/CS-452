#include "StepMotor/StepMotor.hpp"

#include <pico/stdlib.h>

#include <set>

const std::set<StepMotor::Pin> StepMotor::s_pins = {Pin::A, Pin::B, Pin::C,
                                                    Pin::D};
const std::vector<std::set<StepMotor::Pin>> StepMotor::s_sequence = {
    {Pin::D}, {Pin::D, Pin::C}, {Pin::C}, {Pin::C, Pin::B},
    {Pin::B}, {Pin::B, Pin::A}, {Pin::A}, {Pin::A, Pin::D}};
int StepMotor::s_step = 0;

void StepMotor::init() {
   for (const auto &pin : s_pins) {
      gpio_init((int)pin);
      gpio_set_dir((int)pin, GPIO_OUT);
   }
}

void StepMotor::step(bool clockwise) {
   if (clockwise) {
      s_step++;
   } else {
      s_step--;
   }
   s_step %= s_sequence.size();
   set_pins();
}

void StepMotor::set_pins() {
   auto step = s_sequence[s_step];
   for (const auto &pin : s_pins) {
      gpio_put((int)pin, step.contains(pin));
   }
}