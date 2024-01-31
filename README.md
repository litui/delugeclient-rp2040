## RP2040 DelugeClient

This work based on original proof-of-concept by @bfredl at https://github.com/bfredl/delugeclient .

As it currently stands, this only supports OLED messages and will attempt to send sysex requests to any midi device attached. When connecting to a 7seg Deluge, one must "flip" the signal to OLED (on my hardware that means pressing button "C" on the OLED featherwing).

### Todo:

- 7seg rendering
- Specific midi device detection
- Debug messages

