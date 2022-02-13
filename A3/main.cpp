#include "Board/Board.hpp"

#include <FreeRTOS.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <tusb.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <tuple>

void vTask1(void *);
void vTask2(void *);

int main() {
   // Initialize all IO
   stdio_init_all();

   // Wait until the USB is online
   while (!tud_cdc_connected()) {
      sleep_ms(100);
   }

   try {
      // Create the date
      std::tm xDate;
      // Set the date
      xDate.tm_year = 2022 - 1900;
      xDate.tm_mon = 1;
      xDate.tm_mday = 8;
      xDate.tm_hour = 5;
      xDate.tm_min = 0;
      xDate.tm_sec = 0;

      xTaskCreate((TaskFunction_t)vTask1, "Task 1", 256, (void *)&xDate, 1,
                  NULL);
      xTaskCreate((TaskFunction_t)vTask2, "Task 2", 256, (void *)&xDate, 1,
                  NULL);
      vTaskStartScheduler();

   } catch (std::exception e) {
      /// Display the exception for debugging
      std::cout << e.what() << std::endl;
   }
}

/// @param arg tm*
void vTask1(void *arg) {
   tm xDate = *(tm *)arg;

   while (true) {
      auto xCurrentTime = mktime(&xDate);
      std::cout << "Task 1: " << ctime(&xCurrentTime);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      xDate.tm_sec += 2;
   }
}

/// @param arg tm*
void vTask2(void *arg) {
   tm xDate = *(tm *)arg;

   while (true) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      xDate.tm_sec++;
      auto xCurrentTime = mktime(&xDate);
      std::cout << "Task 2: " << ctime(&xCurrentTime);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      xDate.tm_sec++;
   }
}