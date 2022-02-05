#pragma once

#include <pico/stdlib.h>

namespace Led {
/// "Static" class to control the D13 LED
class D13 {
 public:
   /// Initialize all IO needed
   static void init();
   /// Turn the LED on
   static void turn_on();
   /// Turn the LED off
   static void turn_off();
   /// Invert the LED
   /// ie, turn off if it's on, or on if it's on
   /// @returns true if turned on, false if turned off
   static bool invert();
   /// Set the LED
   /// @param on Sets the LED to on if true, off if false
   static void set(bool on);
   /// @returns true if on, false if off
   static bool is_on();

 private:
   static const uint s_led_pin = 13;
   static bool s_is_on;
};
} // namespace Led