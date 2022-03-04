#pragma once

#include <set>
#include <vector>

class Step_motor {
 public:
   enum class State { Clockwise, CounterClockwise, Alternating, Off };

   /// Initialize all IO needed for the step motor
   static void init();
   /// Set the state of the step motor. step() will automatically step in that
   /// direction.
   /// @param state State to set the motor to
   static void set_state(State state);
   /// Returns what portion of the revolution the step motor is in
   /// @returns Float from [0-1)
   static float percent_through_revolution();
   /// @returns Minimum wait between steps in microseconds
   static int min_us_between_steps();
   /// @returns Maximum wait between steps in microseconds
   static int max_us_between_steps();
   /// Steps the motor in a direction depending on its state
   /// @returns True if a sequence is completed
   static bool step();
   /// @returns True if going clockwise
   static bool going_clockwise();

 private:
   enum class Pin : int { A = 0, B = 1, C = 12, D = 13 };

   static const int s_seqs_per_revolution = 512;
   static const int s_min_us_between_steps = 800;
   static const int s_max_us_between_steps = 5000;
   static const std::set<Pin> s_pins;
   static const std::vector<std::set<Pin>> s_sequence;
   static int s_step;
   static int s_tick;
   static bool s_alternating_is_cw;
   static State s_state;

   /// Steps in the direction provided
   /// @param clockwise If true, the motor will step clockwise
   /// @returns True if a sequence is completed
   static bool step(bool clockwise);
   /// Sets the pins to the current sequence
   static void set_pins();
};