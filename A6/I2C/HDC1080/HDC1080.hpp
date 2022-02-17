#pragma once

#include <pico/stdlib.h>

#include <iostream>
#include <map>

namespace I2C {
class HDC1080 {
 public:
   /// Read IDs:
   /// IDs are static, they only need to be read once and stored. This is more
   /// efficient because there is a ~20 ms wait every time something is read
   /// from the HDC1080 that is avoided this way.
   ///
   /// Read configuration:
   /// The configuration only needs to be read every time it is changed. Every
   /// time it is changed is through this class, so it can be stored as well
   /// until it is changed. Then it can be reread.
   static void init();
   /// Convert from Celsius to Fahrenheit
   /// @param tempc Temperature in Celsius
   /// @returns Temperature in Fahrenheit
   static int ctof(int tempc);
   /// @returns Temperature reading in Celsius
   static int temperatureC();
   /// @returns Humidity reading in relative%
   static int humidity();
   /// @returns Serial ID
   static const uint64_t &serialID();
   /// @returns Manufacturer ID
   static const int &manufacturerID();
   /// @returns Device ID
   static const int &deviceID();

   /// @param b False: On.
   /// True: Off.
   static void set_heater(bool b);
   /// @param b False: Temperature or Humidity is acquired.
   /// True: Temperature and Humidity are acquired in sequence, Temperature
   /// first.
   static void set_mode_of_acquisition(bool b);
   /// @param b False: Battery voltage > 2.8V (read only).
   /// True: Battery voltage < 2.8V (read only).
   static void set_battery_status(bool b);
   /// @param b False: 14 bit.
   /// True: 11 bit.
   static void set_temperature_resolution(bool b);
   /// @param ui 00: 14 bit.
   /// 01: 11 bit.
   /// 11: 8 bit.
   static void set_humidity_resolution(uint ui);

   /// @returns False: Normal Operation, this bit self clears.
   /// True: Software Reset.
   static const bool &get_reset();
   /// @returns False: On.
   /// True: Off.
   static const bool &get_heater();
   /// @returns False: Temperature or Humidity is acquired.
   /// True: Temperature and Humidity are acquired in sequence, Temperature
   /// first.
   static const bool &get_mode_of_acquisition();
   /// @returns False: Battery voltage > 2.8V (read only).
   /// True: Battery voltage < 2.8V (read only).
   static const bool &get_battery_status();
   /// @returns False: 14 bit.
   /// True: 11 bit.
   static const bool &get_temperature_resolution();
   /// @returns 00: 14 bit.
   /// 01: 11 bit.
   /// 11: 8 bit.
   static const uint &get_humidity_resolution();

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

   static const uint s_address = 0x40;
   static const std::map<Register, uint> s_bytes_per_register;
   static const uint s_wait_time_ms = 20;

   static uint64_t s_serialID;
   static int s_manufacturerID;
   static int s_deviceID;
   static bool s_rst;
   static bool s_heat;
   static bool s_mode;
   static bool s_btst;
   static bool s_tres;
   static uint s_hres;

   static int read(Register reg);
   static void refresh_config();
   static void read_config();
   static uint64_t make_config();
   static void write_config();
};
} // namespace I2C