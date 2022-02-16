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
      std::cout << "Serial ID: " << I2C::HDC1080::serialID << std::endl;
      std::cout << "Heater: " << I2C::HDC1080::get_heater() << std::endl;
      std::cout << "Mode of acquisition: " << I2C::HDC1080::get_mode_of_acquisition()
                << std::endl;
      std::cout << "Battery: " << I2C::HDC1080::get_battery_status() << std::endl;
      std::cout << "Temperature resolution: " << I2C::HDC1080::get_temperature_resolution()
                << std::endl;
      std::cout << "Humidity resolution: " << I2C::HDC1080::get_humidity_resolution()
                << std::endl;
      std::cout << std::endl;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
   }
}