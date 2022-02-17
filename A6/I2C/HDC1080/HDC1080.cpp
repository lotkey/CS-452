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
const std::map<HDC1080::Register, uint> HDC1080::s_bytes_per_register = {
    {Register::Configuration, 2},  {Register::DeviceID, 2},
    {Register::Humidity, 2},       {Register::SerialID1, 2},
    {Register::SerialID2, 2},      {Register::SerialID3, 2},
    {Register::ManufacturerID, 2}, {Register::Temperature, 2}};

uint64_t HDC1080::s_serialID;
int HDC1080::s_manufacturerID;
int HDC1080::s_deviceID;
bool HDC1080::s_rst;
bool HDC1080::s_heat;
bool HDC1080::s_mode;
bool HDC1080::s_btst;
bool HDC1080::s_tres;
uint HDC1080::s_hres;

void HDC1080::init() {
   s_manufacturerID = read(Register::ManufacturerID);
   s_deviceID = read(Register::DeviceID);

   int s1 = read(Register::SerialID1);
   int s2 = read(Register::SerialID2);
   int s3 = 0b0000000111111111 & (read(Register::SerialID3) >> 7);
   uint64_t id = 0;
   id = id << 16 | s1;
   id = id << 16 | s2;
   id = id << 8 | s3;
   s_serialID = id;

   read_config();
}

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

void HDC1080::read_config() {
   int configuration = read(Register::Configuration);

   auto get_bit = [](int n, int bit_number) {
      return (n & (1 << bit_number)) >> bit_number;
   };

   s_rst = get_bit(configuration, 15);
   s_heat = get_bit(configuration, 13);
   s_mode = get_bit(configuration, 12);
   s_btst = get_bit(configuration, 11);
   s_tres = get_bit(configuration, 10);
   s_hres = 0;
   s_hres = (0b0000001100000000 & configuration) >> 8;
}

const uint64_t &HDC1080::serialID() { return s_serialID; }

const int &HDC1080::manufacturerID() { return s_manufacturerID; }

const int &HDC1080::deviceID() { return s_deviceID; }

int HDC1080::read(Register reg) {
   uint8_t buffer[s_bytes_per_register.at(reg)];
   uint8_t reg_int = (uint8_t)reg;
   int ret = i2c_write_blocking(I2C_PORT, s_address, &reg_int, 1, false);
   if (ret < 0) {
      std::cout << "Error: Write blocking returned " << ret << std::endl;
   }

   vTaskDelay(s_wait_time_ms / portTICK_PERIOD_MS);

   ret = i2c_read_blocking(I2C_PORT, s_address, buffer,
                           s_bytes_per_register.at(reg), false);

   if (ret < 0) {
      std::cout << "Error: Read blocking returned " << ret << std::endl;
   }

   int result = 0;
   for (const auto &byte : buffer) {
      result = result << 8 | byte;
   }

   return result;
}

void HDC1080::set_heater(bool b) {
   if (s_heat == b)
      return;

   s_heat = b;
   refresh_config();
}

void HDC1080::set_mode_of_acquisition(bool b) {
   if (s_mode == b)
      return;

   s_mode = b;
   refresh_config();
}

void HDC1080::set_battery_status(bool b) {
   if (s_btst == b)
      return;

   s_btst = b;
   refresh_config();
}

void HDC1080::set_temperature_resolution(bool b) {
   if (s_tres == b)
      return;

   s_tres = b;
   refresh_config();
}

void HDC1080::set_humidity_resolution(uint ui) {
   if (s_hres == ui)
      return;

   if (ui > 2)
      return;

   s_hres = ui;
   refresh_config();
}

const bool &HDC1080::get_reset() { return s_rst; }

const bool &HDC1080::get_heater() { return s_heat; }

const bool &HDC1080::get_mode_of_acquisition() { return s_mode; }

const bool &HDC1080::get_battery_status() { return s_btst; }

const bool &HDC1080::get_temperature_resolution() { return s_tres; }

const uint &HDC1080::get_humidity_resolution() { return s_hres; }

void HDC1080::refresh_config() {
   write_config();
   read_config();
}

uint64_t HDC1080::make_config() {
   uint64_t config = 0;
   config = config | (s_rst << 15);
   config = config | (s_heat << 13);
   config = config | (s_mode << 12);
   config = config | (s_btst << 11);
   config = config | (s_tres << 10);
   config = config | (s_hres << 8);

   return config;
}

void HDC1080::write_config() {
   uint64_t config = make_config();
   uint8_t buffer[3] = {(uint8_t)Register::Configuration,
                        (uint8_t)((config >> 8) & 0xFF),
                        (uint8_t)(config >> 8)};
   int ret = i2c_write_blocking(I2C_PORT, s_address, buffer, 3, false);
   if (ret < 0) {
      std::cout << "Error: Write blocking returned " << ret << std::endl;
   }
}
} // namespace I2C