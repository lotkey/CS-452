#include "BluetoothA2DPSink.h"
#include "Board.hpp"

#include <list>
#include <math.h>
#include <string>

BluetoothA2DPSink a2dp_sink;
float averages[2];
float maxes[2];
float maximum = 0;

void read_stream(const uint8_t* stream, uint32_t len) {
  int16_t *samples = (int16_t *)stream;
  int sample_count = len / 2;
  float sums[2] = {0, 0};
  float maxbuffer[2] = {0, 0};
  
  for (int i = 0; i < sample_count; i++) {
    if (abs(samples[i]) > maxbuffer[i % 2]) {
      maxbuffer[i % 2] = samples[i];
    }
    sums[i % 2] += abs(samples[i]);
  }
  
  if (sample_count == 0) {
    averages[0] = 0;
    averages[1] = 0;
    maxes[0] = 0;
    maxes[1] = 0;
  } else {
    averages[0] = sums[0] / ((float)sample_count / 2);
    averages[1] = sums[1] / ((float)sample_count / 2);
    maxes[0] = maxbuffer[0];
    maxes[1] = maxbuffer[1];
  }
}

void setup() {
  Serial.begin(74480);
  Board::Led::Pixels::init();
  
  
  static const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = 44100, // corrected by info from bluetooth
    .bits_per_sample = (i2s_bits_per_sample_t) 16, /* the DAC module will only take the 8bits from MSB */
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_MSB,
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };

  a2dp_sink.set_i2s_config(i2s_config);
  a2dp_sink.set_stream_reader(read_stream);
  a2dp_sink.start("ESP32");
}

void loop() {
  Board::Led::Pixels::clear();
  float normalized[2] = {maxes[0] / 32767.0,
                          maxes[1] / 32767.0};
  int intensities[4][2];
  intensities[0][0] = normalized[0] * 255;
  intensities[0][1] = normalized[1] * 255;
  intensities[1][0] = (normalized[0] - 0.25) / 0.25 * 255;
  intensities[1][1] = (normalized[1] - 0.25) / 0.25 * 255;
  intensities[2][0] = (normalized[0] - 0.5) / 0.25 * 255;
  intensities[2][1] = (normalized[1] - 0.5) / 0.25 * 255;
  intensities[3][0] = (normalized[0] - 0.75) / 0.25 * 255;
  intensities[3][1] = (normalized[1] - 0.75) / 0.25 * 255;
  Board::Led::Pixels::set(0, {0, intensities[0][0], intensities[0][1]});
  Board::Led::Pixels::set(1, {0, intensities[1][0], intensities[1][1]});
  Board::Led::Pixels::set(2, {0, intensities[2][0], intensities[2][1]});
  Board::Led::Pixels::set(3, {0, intensities[3][0], intensities[3][1]});  
  Board::Led::Pixels::show();
  // delay(1);
}
