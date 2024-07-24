/*
    VersatileSwitch_As_Button_without_Callback.ino
    Example code for switch as a clickable button without callback functions.
*/

#include <VersatileSwitch.h>

#define PIN_SW 7

VersatileSwitch btn(PIN_SW, INPUT_PULLUP);

void setup() {
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("*** demo VersatileSwitchAsButton ***");
}

void loop() {
  // polling switches
  btn.poll();

  // check action of switch
  if (btn.isClicked()) {
    Serial.println("Clicked.");
  } else if (btn.isDoubleClicked()) {
    Serial.println("Double-clicked.");
  } else if (btn.isLongClicked()) {
    Serial.println("Long-clicked.");
  }
}