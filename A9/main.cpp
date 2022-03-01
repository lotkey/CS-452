#include "Board/Board.hpp"
#include "I2C/HDC1080/HDC1080.hpp"
#include "StepMotor/StepMotor.hpp"

#include <FreeRTOS.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <tusb.h>

#include <iostream>
#include <optional>
#include <set>
#include <tuple>
#include <vector>

void vControl(void *args);
void vDisplay(void *args);

int main() {
   // Initialize all IO
   stdio_init_all();

   while (!tud_cdc_connected()) {
      sleep_ms(10);
   }

   try {
      StepMotor::init();
      Board::Led::Seven_segment::init();
      Board::Button::init();

      // QueueHandle_t xDisplayQueue = xQueueCreate(11, sizeof(uint8_t));

      // std::tuple<QueueHandle_t> xQueues = {xDisplayQueue};

      xTaskCreate((TaskFunction_t)vControl, "vControl", 256, nullptr, 2,
                  nullptr);
      // xTaskCreate((TaskFunction_t)vDisplay, "vDisplay", 256, xDisplayQueue,
      // 1,
      //             nullptr);
      vTaskStartScheduler();
   } catch (std::exception &e) {
      while (true) {
         std::cout << "There has been an error: " << e.what() << std::endl;
      }
   }
}

void vControl(void *args) {
   std::optional<Board::Button::Position> buttonPressed;
   int buttonPressCount = 0;
   uint64_t timePressedSeconds;
   // auto [xDisplayQueue] = *((std::tuple<QueueHandle_t> *)args);

   uint8_t move = 0;
   uint8_t direction = 0;
   uint8_t display = 0;

   while (true) {

      auto currentButtonPressed = Board::Button::get_debounced();
      uint64_t currentTimeSeconds =
          to_us_since_boot(get_absolute_time()) * 1e-6;

      if (currentButtonPressed.has_value()) {
         // button was pressed

         if (!buttonPressed.has_value() ||
             buttonPressed.value() != currentButtonPressed.value()) {
            buttonPressed = currentButtonPressed;
            buttonPressCount = 0;
            timePressedSeconds = currentTimeSeconds;
         } else {
            buttonPressCount++;
         }

         std::cout << buttonPressCount << std::endl;

      } else {
         // button wasn't :(
      }

      // if (currentTimeSeconds - timePressedSeconds >= 2) {

      //    switch (buttonPressed.value()) {
      //    case Board::Button::Position::top_left: {

      //       if (buttonPressCount == 1) {
      //          // move on humidity
      //          move = 0;
      //       } else if (buttonPressCount == 2) {
      //          // move on temperature
      //          move = 1;
      //       } else if (buttonPressCount == 3) {
      //          // display EE and stop
      //          move = 2;
      //       }

      //       break;
      //    }
      //    case Board::Button::Position::top_right: {

      //       if (buttonPressCount == 1) {
      //          // move cw
      //          direction = 0;
      //       } else if (buttonPressCount == 2) {
      //          // move ccw
      //          direction = 1;
      //       } else if (buttonPressCount == 3) {
      //          // move cw one revolution, then ccw one revolution
      //          direction = 2;
      //       }

      //       break;
      //    }
      //    case Board::Button::Position::bottom: {

      //       if (buttonPressCount == 1) {
      //          // display temperature
      //          display = 0;
      //       } else if (buttonPressCount == 2) {
      //          // display humidity
      //          display = 1;
      //       } else if (buttonPressCount == 3) {
      //          // show stepper motor status
      //          display = 2;
      //       }

      //       break;
      //    }
      //    default: {
      //       break;
      //    }
      //    }

      //    buttonPressed = {};
      // }

      // send to all the queues
      // xQueueSend(xDisplayQueue, (void *)&display, 0);
      // vTaskDelay(10 / portTICK_PERIOD_MS);
   }
}

void vDisplay(void *arg) {
   I2C::HDC1080::init();
   QueueHandle_t xQueue = (QueueHandle_t)arg;
   uint8_t state;
   uint numberToDisplay = 0;

   while (true) {
      while (xQueueReceive(xQueue, (void *)&state, portMAX_DELAY)) {
      }

      switch (state) {
      case 0: {
         Board::Led::Seven_segment::display_uint(I2C::HDC1080::temperatureC());
         break;
      }
      case 1: {
         Board::Led::Seven_segment::display_uint(I2C::HDC1080::humidity());
         break;
      }
      case 2: {
         Board::Led::Seven_segment::display_char(
             'E', Board::Led::Seven_segment::Side::both);
         break;
      }
      }
   }
}