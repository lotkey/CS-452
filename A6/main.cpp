#include "Board/Board.hpp"
#include "I2C/I2C.hpp"

#include <FreeRTOS.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <tusb.h>

#include <bitset>
#include <chrono>
#include <iostream>
#include <tuple>

void vRead(void *);
void vShow(void *);

int main() {
   // Initialize all IO
   stdio_init_all();
   Board::Led::Seven_segment::init();
   I2C::init();

   while (!tud_cdc_connected()) {
      sleep_ms(100);
   }

   QueueHandle_t xQueue = xQueueCreate(10, sizeof(uint));

   xTaskCreate((TaskFunction_t)vRead, "Read", 10000, (void *)xQueue, 2, NULL);
   xTaskCreate((TaskFunction_t)vShow, "Show", 1000, (void *)xQueue, 1, NULL);
   vTaskStartScheduler();
}

void vRead(void *arg) {
   QueueHandle_t xQueue = (QueueHandle_t)arg;
   bool bShowingTemp = true;
   uint uNumToShow;
   I2C::HDC1080::init();
   I2C::HDC1080::set_mode_of_acquisition(true);

   while (true) {
      if (bShowingTemp) {
         uNumToShow = I2C::HDC1080::temperatureC();
         uNumToShow = I2C::HDC1080::ctof(uNumToShow);

         for (unsigned i = 0; i < 100; i++) {
            std::cout << std::endl;
         }

         std::cout << "================== IDs =================="
                   << std::endl;
         std::cout << "      Serial ID:\t" << I2C::HDC1080::serialID()
                   << std::endl;
         std::cout << "      Device ID:\t" << I2C::HDC1080::deviceID()
                   << std::endl;
         std::cout << "Manufacturer ID:\t" << I2C::HDC1080::manufacturerID()
                   << std::endl;
         std::cout << std::endl;
         std::cout << "================== Config =================="
                   << std::endl;
         std::cout << "                 Reset:\t" << I2C::HDC1080::get_reset()
                   << std::endl;
         std::cout << "                Heater:\t" << I2C::HDC1080::get_heater()
                   << std::endl;
         std::cout << "               Battery:\t"
                   << I2C::HDC1080::get_battery_status() << std::endl;
         std::cout << "                  Mode:\t"
                   << I2C::HDC1080::get_mode_of_acquisition() << std::endl;
         std::cout << "Temperature Resolution:\t"
                   << I2C::HDC1080::get_temperature_resolution() << std::endl;
         std::cout << "   Humidity Resolution:\t"
                   << I2C::HDC1080::get_humidity_resolution() << std::endl;
         std::cout << std::endl;
         std::cout << "================== Readings =================="
                   << std::endl;
         std::cout << "Temperature:\t" << uNumToShow << "F" << std::endl;
         std::cout << "   Humidity:\t" << I2C::HDC1080::humidity() << "%"
                   << std::endl;
         std::cout << std::endl;

      } else {
         uNumToShow = I2C::HDC1080::humidity();
      }

      xQueueSend(xQueue, (void *)&uNumToShow, 0);
      bShowingTemp = !bShowingTemp;
      vTaskDelay(5000 / portTICK_PERIOD_MS);
   }
}

void vShow(void *arg) {
   QueueHandle_t xQueue = (QueueHandle_t)arg;
   uint uNumToShow = 0;

   while (true) {
      while (xQueueReceive(xQueue, (void *)&uNumToShow, 0)) {
      }

      Board::Led::Seven_segment::display_uint(uNumToShow);
   }
}