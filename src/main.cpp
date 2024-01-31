#include <Arduino.h>

#include "display.hpp"
#include "usbh.hpp"
#include "buttons.hpp"

void setup() {
  Serial1.begin(115200);
  Display::driver.begin();
  USBH::host.begin();
}

void setup1() {
  Buttons::begin();
}

void loop() {
  Display::driver.tick();
  USBH::host.tick();
}

void loop1() {
  Buttons::tick();
}