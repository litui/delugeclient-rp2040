#pragma once

#include <Adafruit_TinyUSB.h>
#include <usb_midi_host.h>

#include "display.hpp"

namespace USBH {

class Host {
public:
  Host() {};

  void begin();
  void tick();

private:
  Adafruit_USBH_Host usbh;
  tusb_desc_device_t desc_device;

  void requestImage();
  void requestFlip();

}; // class Host

extern Host host;

} // namespace USBH