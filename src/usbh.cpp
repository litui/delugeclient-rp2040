#include "usbh.hpp"
#include "buttons.hpp"

uint8_t midi_dev_addr = 0;
bool flipRequested = false;

uint8_t midiData[512];
size_t midiDataLen = 0;
bool openMsg = false;

namespace USBH {

const uint8_t sysex_get_oled[6] = {0xf0, 0x7d, 0x02, 0x00, 0x01, 0xf7};
const uint8_t sysex_get_7seg[6] = {0xf0, 0x7d, 0x02, 0x01, 0x00, 0xf7};
const uint8_t sysex_get_display[6] = {0xf0, 0x7d, 0x02, 0x00, 0x02, 0xf7};
const uint8_t sysex_get_display_force[6] = {0xf0, 0x7d, 0x02, 0x00, 0x03, 0xf7};
const uint8_t sysex_get_debug[6] = {0xf0, 0x7d, 0x03, 0x00, 0x01, 0xf7};
const uint8_t sysex_flip_screen[6] = {0xf0, 0x7d, 0x02, 0x00, 0x04, 0xf7};

Host host;

void Host::requestImage() {
  const uint32_t interval_ms = 1000;
  static uint32_t start_ms = 0;

  tuh_midi_stream_flush(midi_dev_addr);

  if (millis() - start_ms < interval_ms) {
    return;
  }
  start_ms += interval_ms;

  uint8_t cable = 0;
  tuh_midi_stream_write(midi_dev_addr, cable, sysex_get_display, 6);
  tuh_midi_stream_flush(midi_dev_addr);
}

void Host::requestFlip() {
  if (!flipRequested) {
    uint8_t cable = 0;
    Serial1.println("Flip requested.");
    tuh_midi_stream_flush(midi_dev_addr);
    tuh_midi_stream_write(midi_dev_addr, cable, sysex_flip_screen, 6);
    tuh_midi_stream_flush(midi_dev_addr);
    flipRequested = true;
  }
}

void Host::begin() {
  delay(2000);

  usbh.begin(0);
}

void Host::tick() {
  usbh.task();

  bool connected = midi_dev_addr != 0 && tuh_midi_configured(midi_dev_addr);

  if (connected && tuh_midih_get_num_tx_cables(midi_dev_addr) > 0) {
    if (Buttons::buttonC) {
      Buttons::buttonC = false;
      requestFlip();
    } else if (!openMsg) {
      requestImage();
    }
  }
  yield();
}
} // namespace USBH

void handleSysexMessage(uint8_t *data, size_t length) {
  // for (uint32_t idx = 0; idx < length; idx++) {
  //   Serial1.printf("%02x ", data[idx]);
  //   yield();
  // }
  // Serial1.printf("\nData Length: %u\n", length);

  if (length >= 5 && data[2] == 0x02 && data[3] == 0x40) {
    // Serial1.println("Found OLED!");

    if (data[4] == 1) {
      Display::driver.drawOLED(data, length);
    } else if (data[4] == 2) {
      Display::driver.drawOLEDDelta(data, length);
    }
  } else if (length >= 5 && data[2] == 0x02 && data[3] == 0x41) {
    // Serial1.println("Found 7seg!");

    if (data[4] == 0) {
      Display::driver.draw7seg(data+7, length - 7, data[6]);
    }
  }
}

/* USB Midi Host callbacks */

void tuh_midi_mount_cb(uint8_t dev_addr, uint8_t in_ep, uint8_t out_ep, uint8_t num_cables_rx, uint16_t num_cables_tx)
{
  Serial1.printf("MIDI device address = %u, IN endpoint %u has %u cables, OUT endpoint %u has %u cables\r\n",
      dev_addr, in_ep & 0xf, num_cables_rx, out_ep & 0xf, num_cables_tx);

  if (midi_dev_addr == 0) {
    // then no MIDI device is currently connected
    midi_dev_addr = dev_addr;
  }
  else {
    Serial1.printf("A different USB MIDI Device is already connected.\r\nOnly one device at a time is supported in this program\r\nDevice is disabled\r\n");
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_midi_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  if (dev_addr == midi_dev_addr) {
    midi_dev_addr = 0;
    Serial1.printf("MIDI device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);

  }
  else {
    Serial1.printf("Unused MIDI device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
  }
}

void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t num_packets)
{
  if (midi_dev_addr == dev_addr) {
    if (num_packets != 0) {
      uint8_t cable_num;
      uint8_t buffer[128];
      while (1) {
        uint32_t bytes_read = tuh_midi_stream_read(dev_addr, &cable_num, buffer, sizeof(buffer));
        if (bytes_read == 0)
          return;
        if (cable_num != 0)
          return;
        flipRequested = false;

        for (uint32_t cur = 0; cur < bytes_read; cur++) {
          if (!openMsg && buffer[cur] == 0xf0) {
            openMsg = true;
            midiDataLen = 0;
          }

          if (openMsg) {
            midiDataLen++;
            midiData[midiDataLen - 1] = buffer[cur];

            if (buffer[cur] == 0xf7) {
              handleSysexMessage(midiData, midiDataLen);
              openMsg = false;
            }
          }
          yield();
        }
        tuh_midi_stream_flush(midi_dev_addr);
        yield();
      }
    }
  }
}

void tuh_midi_tx_cb(uint8_t dev_addr)
{
    (void)dev_addr;
}

void parse_configuration_descriptor_cb(uint8_t drv_id, uint32_t ptr) {
  Serial1.printf("Parsing for driver ID %u of %p\r\n", drv_id, ptr);
}
