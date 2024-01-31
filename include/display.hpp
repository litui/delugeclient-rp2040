#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <SPI.h>

#include "defs.hpp"

namespace Display {

class Driver {
public:
  Driver() {};

  void begin();
  void tick();

  void announce(const char *message);

  void drawOLED(uint8_t *data, size_t length);
  void drawOLEDDelta(uint8_t *data, size_t length);
  void draw7seg(uint8_t *digits, size_t length, uint8_t dots);
private:
  // TFT_eSPI tft = TFT_eSPI();
  Adafruit_SH1107 disp = Adafruit_SH1107(64, 128, &Wire);
  bool ready = false;
  bool showing_remote = false;

  uint8_t oledData[OLED_DATA_LEN];
  void drawOLEDData(uint8_t *data, size_t data_len);
};

extern Driver driver;

} // namespace Display