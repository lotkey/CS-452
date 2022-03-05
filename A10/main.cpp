#include "Board/Board.hpp"
#include "I2C/I2C.hpp"
#include "Step_motor/Step_motor.hpp"

#include <FreeRTOS.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <tusb.h>

#include <iomanip>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>
#include <tuple>
#include <vector>

/// Set to true for vPrintTaskList
constexpr bool Debug = false;

void vControl(void *args);
void vPrintTaskList();
int round(float f);

int main() {
   // Initialize all IO
   stdio_init_all();
   Board::Led::Pixel::init();

   const float Brightness = 0.1;
   const float Rainbow_increment = 1.0 / 255.0;
   float rainbow_index = 0;

   while (true) {
      Board::Util::Color c1 = Board::Util::Color::Rainbow[(int)rainbow_index];
      Board::Util::Color c2 =
          Board::Util::Color::Rainbow[((int)rainbow_index + 1) %
                                      Board::Util::Color::Rainbow.size()];

      float blend = rainbow_index - (int)rainbow_index;
      float blend1 = 1.0 - blend;

      c1 = c1 * blend1;
      c2 = c2 * blend;
      Board::Util::Color c3 = (c1 + c2) * Brightness;

      std::cout << c3.to_string() << std::endl;

      Board::Led::Pixel::set({0, 1, 2, 3}, c3);
      Board::Led::Pixel::show();

      rainbow_index += Rainbow_increment;
      if (rainbow_index >= Board::Util::Color::Rainbow.size()) {
         rainbow_index = 0 + Board::Util::Color::Rainbow.size() - rainbow_index;
      }
      sleep_ms(1);
   }

   /// w b r g
   // Board::Led::Pixel::set(0, {10, 0, 0, 0});
   // Board::Led::Pixel::set(1, {0, 10, 0, 0});
   // Board::Led::Pixel::set(2, {0, 0, 10, 0});
   // Board::Led::Pixel::set(3, {0, 0, 0, 10});
   // Board::Led::Pixel::show();

   // try {
   //    Step_motor::init();
   //    Board::Led::Seven_segment::init();
   //    Board::Led::Seven_segment::clear();
   //    Board::Button::init();

   //    QueueHandle_t xDisplayQueue = xQueueCreate(11, sizeof(uint8_t));

   //    std::tuple<QueueHandle_t> xQueues = {xDisplayQueue};

   //    xTaskCreate((TaskFunction_t)vControl, "vControl", 256, (void
   //    *)&xQueues,
   //                1, nullptr);

   //    if (Debug) {
   //       xTaskCreate((TaskFunction_t)vPrintTaskList, "vTaskList", 1000,
   //       nullptr,
   //                   2, nullptr);
   //    }

   //    vTaskStartScheduler();
   // } catch (std::exception &e) {
   //    while (true) {
   //       std::cout << "There has been an error: " << e.what() << std::endl;
   //       sleep_ms(1000);
   //    }
   // }
}

void vControl(void *args) {
   I2C::init();
   I2C::HDC1080::init();
   Step_motor::set_state(Step_motor::State::Clockwise);
   std::optional<Board::Button::Position> xButtonLastPressed;
   std::vector<Board::Led::Seven_segment::Segment> xMotorStateSegments = {
       Board::Led::Seven_segment::Segment::north,
       Board::Led::Seven_segment::Segment::northeast,
       Board::Led::Seven_segment::Segment::southeast,
       Board::Led::Seven_segment::Segment::south,
       Board::Led::Seven_segment::Segment::southwest,
       Board::Led::Seven_segment::Segment::northwest};

   const std::vector<char> xMoveStatusChars = {'H', 'T', 'E'};
   const std::vector<char> xDirectionStatusChars = {')', '(', '0'};
   const uint uiRead1080DelayUs = 100 * 1e3;
   const uint uiButtonDelayUs = 2000 * 1e3;
   const uint ui1080PrimeDelayUs = 50 * 1e3;
   const uint uiMinTemperature = 15;
   const uint uiMaxTemperature = 35;
   const uint uiMinHumidity = 35;
   const uint uiMaxHumidity = 75;

   uint uiButtonPressCount = 0;
   uint64_t uiLastButtonPressTimeUs = 0;
   uint64_t uiLast1080ReadTimeUs = 0;
   uint64_t uiLastStepTimeUs = 0;
   uint64_t uiTimeBetweenStepsUs = 1e6; // big number to start with
   bool b1080Primed = false;
   bool bReadingHumidity = true;
   uint uiHumidity = 0;
   uint uiTemperature = 0;

   uint8_t uiMove = 0;
   uint8_t uiDirection = 0;
   uint8_t uiDisplay = 0;

   while (true) {

#pragma region Get button input
      auto xCurrentButtonPressed = Board::Button::get_debounced();
      uint64_t uiCurrentTimeUs = to_us_since_boot(get_absolute_time());

      if (xCurrentButtonPressed.has_value()) {
         // button was pressed

         if (!xButtonLastPressed.has_value() ||
             xButtonLastPressed.value() != xCurrentButtonPressed.value()) {
            xButtonLastPressed = xCurrentButtonPressed;
            uiButtonPressCount = 1;
            uiLastButtonPressTimeUs = uiCurrentTimeUs;
         } else {
            uiButtonPressCount++;
         }
      }
#pragma endregion

#pragma region Handle button input
      if (uiCurrentTimeUs - uiLastButtonPressTimeUs >= uiButtonDelayUs &&
          xButtonLastPressed.has_value()) {

         switch (xButtonLastPressed.value()) {
         case Board::Button::Position::top_left: {

            if (uiButtonPressCount <= 3) {
               uiMove = uiButtonPressCount - 1;
            }

            if (uiMove == 2) {
               Board::Led::Seven_segment::clear();
               Board::Led::Seven_segment::set(
                   'E', Board::Led::Seven_segment::Side::both);
               Step_motor::set_state(Step_motor::State::Off);
            } else {
               switch (uiDirection) {
               case 0: {
                  Step_motor::set_state(Step_motor::State::Clockwise);
                  break;
               }
               case 1: {
                  Step_motor::set_state(Step_motor::State::CounterClockwise);
                  break;
               }
               case 2: {
                  Step_motor::set_state(Step_motor::State::Alternating);
                  break;
               }
               }
            }

            break;
         }
         case Board::Button::Position::top_right: {

            if (uiButtonPressCount <= 3 && uiMove != 2) {
               if (uiDirection != uiButtonPressCount - 1) {
                  uiDirection = uiButtonPressCount - 1;
                  switch (uiDirection) {
                  case 0: {
                     Step_motor::set_state(Step_motor::State::Clockwise);
                     break;
                  }
                  case 1: {
                     Step_motor::set_state(Step_motor::State::CounterClockwise);
                     break;
                  }
                  case 2: {
                     Step_motor::set_state(Step_motor::State::Alternating);
                     break;
                  }
                  }
               }
            }

            break;
         }
         case Board::Button::Position::bottom: {

            if (uiButtonPressCount <= 3 && uiMove != 2) {
               uiDisplay = uiButtonPressCount - 1;
            }

            break;
         }
         default: {
            break;
         }
         }

         xButtonLastPressed = {};
      }
#pragma endregion

#pragma region Priming and reading the HDC1080
      if (uiRead1080DelayUs - (uiCurrentTimeUs - uiLast1080ReadTimeUs) >=
              ui1080PrimeDelayUs &&
          !b1080Primed) {
         if (bReadingHumidity) {
            I2C::HDC1080::prime_register(I2C::HDC1080::Register::Humidity);
         } else {
            I2C::HDC1080::prime_register(I2C::HDC1080::Register::Temperature);
         }
         b1080Primed = true;
      }

      if (uiCurrentTimeUs - uiLast1080ReadTimeUs >= ui1080PrimeDelayUs) {
         if (bReadingHumidity) {
            uiHumidity = I2C::HDC1080::humidity(b1080Primed);
         } else {
            uiTemperature = I2C::HDC1080::temperatureC(b1080Primed);
         }
         bReadingHumidity = !bReadingHumidity;
         uiLast1080ReadTimeUs = uiCurrentTimeUs;
         b1080Primed = false;

         if (uiMove == 0) {
            /// Humidity
            float percent = (float)(uiHumidity - uiMinHumidity) /
                            (float)(uiMaxHumidity - uiMinHumidity);
            percent = 1.0 - percent;
            uiTimeBetweenStepsUs =
                percent * (Step_motor::max_us_between_steps() -
                           Step_motor::min_us_between_steps());
            uiTimeBetweenStepsUs += Step_motor::min_us_between_steps();
         } else if (uiMove == 1) {
            /// Temperature
            float percent = (float)(uiTemperature - uiMinTemperature) /
                            (float)(uiMaxTemperature - uiMinTemperature);
            percent = 1.0 - percent;
            uiTimeBetweenStepsUs =
                percent * (Step_motor::max_us_between_steps() -
                           Step_motor::min_us_between_steps());
            uiTimeBetweenStepsUs += Step_motor::min_us_between_steps();
         }
      }
#pragma endregion

#pragma region Step motor
      if (uiCurrentTimeUs - uiLastStepTimeUs >= uiTimeBetweenStepsUs) {
         Step_motor::step();
         uiLastStepTimeUs = uiCurrentTimeUs;
      }
#pragma endregion

#pragma region Display

      if (uiMove != 2) {

         if (uiDisplay == 0 || uiDisplay == 1) {
            std::stringstream xSs;
            if (uiDisplay == 0) {
               xSs << std::hex << uiTemperature;
            } else if (uiDisplay == 1) {
               xSs << std::hex << uiHumidity;
            }
            std::string xHexString = xSs.str();
            while (xHexString.length() < 2) {
               xHexString += '0';
            }
            Board::Led::Seven_segment::set_combo<char, char>(xHexString[0],
                                                             xHexString[1]);
         } else if (uiDisplay == 2) {
            std::set<Board::Led::Seven_segment::Segment> xSegments;
            int index = round(Step_motor::percent_through_revolution() *
                              (float)xMotorStateSegments.size()) %
                        xMotorStateSegments.size();

            xSegments.insert(xMotorStateSegments[index]);
            if (Step_motor::going_clockwise()) {
               xSegments.insert(Board::Led::Seven_segment::Segment::decimal);
            }

            Board::Led::Seven_segment::set_combo<
                char, const std::set<Board::Led::Seven_segment::Segment> &>(
                xMoveStatusChars[uiMove], xSegments);
         }
      }
#pragma endregion
   }
}

void vPrintTaskList() {
   char pTaskList[500];
   while (true) {
      vTaskList(pTaskList);
      std::cout << "**********************************" << std::endl;
      std::cout << "Task  State   Prio    Stack    Num" << std::endl;
      std::cout << "**********************************" << std::endl;
      std::cout << pTaskList << std::endl;
      std::cout << "**********************************" << std::endl;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
   }
}

int round(float f) {
   int i = f;
   if (f - i >= 0.5) {
      i++;
   }
   return i;
}