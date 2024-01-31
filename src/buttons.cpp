#include "buttons.hpp"

namespace Buttons {

bool buttonA = false;
bool buttonB = false;
bool buttonC = false;

Bounce2::Button buttonA_db;
Bounce2::Button buttonB_db;
Bounce2::Button buttonC_db;

void begin() {
  buttonA_db.attach(BUTTON_A_PIN, INPUT_PULLUP);
  buttonB_db.attach(BUTTON_B_PIN, INPUT_PULLUP);
  buttonC_db.attach(BUTTON_C_PIN, INPUT_PULLUP);
}

void tick() {
  buttonA_db.update();
  buttonB_db.update();
  buttonC_db.update();

  if (buttonA_db.pressed()) buttonA = true;
  if (buttonB_db.pressed()) buttonB = true;
  if (buttonC_db.pressed()) buttonC = true;
}

} // namespace Buttons