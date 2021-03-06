#include "Board/Board.hpp"

#include <FreeRTOS.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <tusb.h>

#include <chrono>
#include <iostream>
#include <tuple>

void vCount(void *);
void vShowCounter(void *);
void vTurnD13On(void *);
void vTurnD13Off(void *);

int main() {
   // Initialize all IO
   stdio_init_all();
   Board::Led::Seven_segment::init();
   Board::Led::D13::init();

   try {
      SemaphoreHandle_t xCountSem = xSemaphoreCreateBinary();
      SemaphoreHandle_t xBlinkOnSem = xSemaphoreCreateBinary();
      SemaphoreHandle_t xBlinkOffSem = xSemaphoreCreateBinary();
      // To communicate the count between vCount and vShowCounter
      QueueHandle_t xCountQueue = xQueueCreate(100, sizeof(uint));

      std::tuple<SemaphoreHandle_t, QueueHandle_t, SemaphoreHandle_t,
                 SemaphoreHandle_t>
          xCountArgs = {xCountSem, xCountQueue, xBlinkOnSem, xBlinkOffSem};

      std::tuple<SemaphoreHandle_t, QueueHandle_t> xShowCounterArgs = {
          xCountSem, xCountQueue};

      // // Create a task from print_time_every_second
      xTaskCreate((TaskFunction_t)vCount, "Counts and gives semaphores", 256,
                  (void *)&xCountArgs, 1, NULL);
      xTaskCreate((TaskFunction_t)vShowCounter,
                  "Shows the current count on the 7-seg LED", 256,
                  (void *)&xShowCounterArgs, 2, NULL);
      xTaskCreate((TaskFunction_t)vTurnD13On, "Turns D13 LED on", 256,
                  (void *)xBlinkOnSem, 2, NULL);
      xTaskCreate((TaskFunction_t)vTurnD13Off, "Turns D13 LED off", 256,
                  (void *)xBlinkOffSem, 2, NULL);
      // Start the scheduler
      vTaskStartScheduler();

      // Loop indefinitely
      while (true) {
      }

   } catch (std::exception e) {
      /// Display the exception for debugging
      std::cout << e.what() << std::endl;
   }
}

/// Counts from 0 to 42, then from 42 to 0.
/// Gives semaphores to other tasks to display the count and to blink.
/// @param arg tuple {xCountSem, xCountQueue, xBlinkOnSem, xBlinkOffSem}
void vCount(void *arg) {
   auto [xCountSem, xCountQueue, xBlinkOnSem, xBlinkOffSem] =
       *((std::tuple<SemaphoreHandle_t, QueueHandle_t, SemaphoreHandle_t,
                     SemaphoreHandle_t> *)arg);

   bool bGoingUp = true;
   uint uCounter = 0;
   uint uLastHalfSecond = to_us_since_boot(get_absolute_time()) * 2e-6;

   while (true) {
      uint uCurrentHalfSecond = to_us_since_boot(get_absolute_time()) * 2e-6;
      if (Board::Button::get(Board::Button::Position::top_left)) {
         uCounter = 0;
         bGoingUp = true;
      } else if (Board::Button::get(Board::Button::Position::top_right)) {
         uCounter = 42;
         bGoingUp = false;
      }

      if (!Board::Button::get(Board::Button::Position::bottom) &&
          uLastHalfSecond != uCurrentHalfSecond) {

         // Either update the counter, or change the direction
         // This way, the counter stays at the end-points for two loops
         // So it loops from 0-42, then from 42-0

         if (Board::Button::get(Board::Button::Position::top_left)) {
            uCounter = 0;
         } else if (Board::Button::get(Board::Button::Position::top_right)) {
            uCounter = 42;
         } else if (bGoingUp) {
            if (uCounter == 42) {
               bGoingUp = false;
            } else {
               uCounter++;
            }
         } else {
            if (uCounter == 0) {
               bGoingUp = true;
            } else {
               uCounter--;
            }
         }

         uLastHalfSecond = uCurrentHalfSecond;
         xSemaphoreGive(xBlinkOnSem);
         xQueueSend(xCountQueue, (void *)&uCounter, 0);
      }

      xSemaphoreGive(xCountSem);
      xSemaphoreGive(xBlinkOffSem);
   }
}

/// Displays the latest number from the queue when given the semaphore.
/// Loops for a bit before waiting for the semaphore again.
/// @param arg tuple {xCountSem, xCountQueue}
void vShowCounter(void *arg) {
   auto [xCountSem, xCountQueue] =
       *((std::tuple<SemaphoreHandle_t, QueueHandle_t> *)arg);

   uint uCount = 0;
   // Number of loops to do before waiting for the semaphore again
   const uint uNumLoops = 500;
   // Current loop
   uint uLoop = 0;

   while (true) {
      if (uLoop > 0) {
         Board::Led::Seven_segment::display_uint(uCount);
         uLoop--;
      } else if (xSemaphoreTake(xCountSem, portMAX_DELAY)) {

         while (xQueueReceive(xCountQueue, (void *)&uCount, 0)) {
         }

         Board::Led::Seven_segment::display_uint(uCount);
         uLoop = uNumLoops;
      }
   }
}

/// Turns the D13 LED on when given the semaphore.
/// @param arg a SemaphoreHandle_t
void vTurnD13On(void *arg) {
   SemaphoreHandle_t xBlinkOnSem = (SemaphoreHandle_t)arg;

   while (true) {
      if (xSemaphoreTake(xBlinkOnSem, portMAX_DELAY)) {
         Board::Led::D13::turn_on();
      }
   }
}

/// Turns the D13 LED off when given the semaphore.
/// @param arg a SemaphoreHandle_t
void vTurnD13Off(void *arg) {
   SemaphoreHandle_t xBlinkOffSem = (SemaphoreHandle_t)arg;

   while (true) {
      if (xSemaphoreTake(xBlinkOffSem, portMAX_DELAY)) {
         Board::Led::D13::turn_off();
      }
   }
}