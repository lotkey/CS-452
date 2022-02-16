#pragma once

#include "pico/stdlib.h"

#include <iostream>
#include <map>

namespace I2C {
class HDC1080 {
 public:
   static void init();
   static int ctof(int);

   static int temperatureC();
   static int humidity();
   static uint64_t serialID();
   static int manufacturerID();
   static int deviceID();

   // static void set_heater(bool);
   // static void set_mode_of_acquisition(bool);
   // static void set_battery_status(bool);
   // static void set_temperature_resolution(bool);
   // static void set_humidity_resolution(uint);

   static bool get_reset();
   static bool get_heater();
   static bool get_mode_of_acquisition();
   static bool get_battery_status();
   static bool get_temperature_resolution();
   static uint get_humidity_resolution();

 private:
   enum class Register {
      Temperature = 0x00,
      Humidity = 0x01,
      Configuration = 0x02,
      SerialID1 = 0xFB,
      SerialID2 = 0xFC,
      SerialID3 = 0xFD,
      ManufacturerID = 0xFE,
      DeviceID = 0xFF
   };

   static const int s_address = 0x40;
   static const std::map<Register, int> s_bytes_per_register;
   static const int s_wait_time_ms = 15;

   static uint64_t s_serialID;
   static int s_manufacturerID;
   static int s_deviceID;
   static bool s_rst;
   static bool s_heat;
   static bool s_mode;
   static bool s_btst;
   static bool s_tres;
   static uint s_hres;

   /// Writes the register to be read, waits, and then reads the result.
   /// @param reg Register to read
   /// @param sleep_ms Amount of milliseconds to wait
   /// @param use_sleep_ms Optional argument, if true this function uses
   /// sleep_ms instead of vTaskDelay
   static int read(Register reg);
   static void read_config();
   static uint64_t make_config();
   static void write_config(uint64_t);
};
} // namespace I2C