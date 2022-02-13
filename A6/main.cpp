#include "Board/Board.hpp"
#include "I2C/I2C.hpp"

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

void vRead();

int main() {
   // Initialize all IO
   stdio_init_all();
   Board::Led::Seven_segment::init();
   I2C::init();

   while (!tud_cdc_connected()) {
      sleep_ms(100);
   }

   xTaskCreate((TaskFunction_t)vRead, "Read", 256, NULL, 1, NULL);
   vTaskStartScheduler();
}

void vRead() {
   while (true) {
      I2C::HDC1080::Config config(I2C::HDC1080::get_configuration());
      std::cout << "Software reset: " << config.software_reset << std::endl;
      std::cout << "Heater: " << config.heater << std::endl;
      std::cout << "Mode of acquisition: " << config.mode_of_acquisition
                << std::endl;
      std::cout << "Battery: " << config.battery_status << std::endl;
      std::cout << "Temperature resolution: " << config.temperature_resolution
                << std::endl;
      std::cout << "Humidity resolution: " << config.humidity_resolution
                << std::endl;
      std::cout << std::endl;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
   }
}