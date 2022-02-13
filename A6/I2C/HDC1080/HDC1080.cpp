#include "I2C/HDC1080/HDC1080.hpp"
#include "I2C/I2C.hpp"

#include <FreeRTOS.h>
#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <task.h>

#include <map>
#include <math.h>

namespace I2C {
const std::map<HDC1080::Register, int> HDC1080::s_bytes_per_register = {
    {Register::Configuration, 2},  {Register::DeviceID, 2},
    {Register::Humidity, 2},       {Register::SerialID1, 2},
    {Register::SerialID2, 2},      {Register::SerialID3, 2},
    {Register::ManufacturerID, 2}, {Register::Temperature, 2}};

int HDC1080::ctof(int tempC) {
   int tempF = ((float)tempC * (9.0 / 5.0)) + 32;
   return tempF;
}

int HDC1080::temperatureC() {
   int result = read(Register::Temperature);
   result = (result / (float)pow(2, 16)) * 165.0 - 40;
   return result;
}

int HDC1080::humidity() {
   int result = read(Register::Humidity);
   result = (result / (float)pow(2, 16)) * 100.0;
   return result;
}

const HDC1080::Config HDC1080::get_configuration() {
   int configuration = read(Register::Configuration);

   auto get_bit = [](int n, int bit_number) {
      return (n & (1 << bit_number)) >> bit_number;
   };

   bool rst = get_bit(configuration, 15);
   bool heat = get_bit(configuration, 13);
   bool mode = get_bit(configuration, 12);
   bool btst = get_bit(configuration, 11);
   bool tres = get_bit(configuration, 10);
   uint hres = 0;
   hres = (0b0000001100000000 & configuration) >> 8;
   Config config(rst, heat, mode, btst, tres, hres);
   return config;
}

uint64_t HDC1080::serialID() {
   int s1 = read(Register::SerialID1);
   int s2 = read(Register::SerialID2);
   int s3 = 0b0000000111111111 & (read(Register::SerialID3) >> 7);
   uint64_t id = 0;
   id = id << 16 | s1;
   id = id << 16 | s2;
   id = id << 8 | s3;
   return id;
}

int HDC1080::manufacturerID() { return read(Register::ManufacturerID); }

int HDC1080::deviceID() { return read(Register::DeviceID); }

int HDC1080::read(Register reg, int sleep_amount_ms) {
   uint8_t buffer[s_bytes_per_register.at(reg)];
   uint8_t reg_int = (uint8_t)reg;
   int ret = i2c_write_blocking(I2C_PORT, address, &reg_int, 1, false);

   vTaskDelay(sleep_amount_ms / portTICK_PERIOD_MS);

   ret = i2c_read_blocking(I2C_PORT, address, buffer,
                           s_bytes_per_register.at(reg), false);

   int result = 0;
   for (const auto &byte : buffer) {
      result = result << 8 | byte;
   }

   return result;
}
} // namespace I2C