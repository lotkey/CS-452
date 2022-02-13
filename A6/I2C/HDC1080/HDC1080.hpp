#pragma once

#include "pico/stdlib.h"

#include <iostream>
#include <map>

namespace I2C {
class HDC1080 {
 public:
   class Config {
    public:
      Config(bool sw_rst, bool heater, bool acq_mode, bool batt_stat,
             bool temp_res, uint hum_res)
          : software_reset(sw_rst), heater(heater),
            mode_of_acquisition(acq_mode), battery_status(batt_stat),
            temperature_resolution(temp_res), humidity_resolution(hum_res) {
         if (hum_res > 3) {
            std::cerr << "Humidity resolution must be 00, 01, or 10."
                      << std::endl;
            throw std::runtime_error("Humidity resolution was: " + hum_res);
         }
      }

      const bool software_reset;
      const bool heater;
      const bool mode_of_acquisition;
      const bool battery_status;
      const bool temperature_resolution;
      const uint humidity_resolution;
   };

   static int ctof(int);

   static int temperatureC();
   static int humidity();
   static const Config get_configuration();
   static uint64_t serialID();
   static int manufacturerID();
   static int deviceID();

   /// @param rst If true, then the software will be reset.
   /// Otherwise, normal operation.
   static void set_software_reset(bool rst); /// DANGER ZONE
   static void set_heater(bool);
   static void set_mode_of_acquisition(bool);
   static void set_battery_status(bool);
   static void set_temperature_resolution(bool);
   static void set_humidity_resolution(uint);

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

   static const int address = 0x40;
   static const std::map<Register, int> s_bytes_per_register;

   /// Writes the register to be read, waits, and then reads the result.
   /// @param reg Register to read
   /// @param sleep_ms Amount of milliseconds to wait
   /// @param use_sleep_ms Optional argument, if true this function uses
   /// sleep_ms instead of vTaskDelay
   static int read(Register reg, int sleep_amount_ms = 9);
};
} // namespace I2C