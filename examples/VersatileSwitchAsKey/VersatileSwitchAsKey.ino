/*
    VersatileSwitchAsKey.ino
    Example code for using switch as a key.
*/

#include <VersatileSwitch.h>

#define PIN_SW 7

VersatileSwitch sw(PIN_SW, INPUT_PULLUP);

void setup() {
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("*** demo VersatileSwitchAsKey ***");

  // attach callback functions using as a key
  sw.attachCallback_Pressed(on_switch_pressed);
  sw.attachCallback_Repeated(on_switch_repeated);
}

void loop() {
  // polling switches
  sw.poll();
}

// callback functions
void on_switch_pressed() {
  Serial.println("Pressed.");
}

void on_switch_repeated() {
  Serial.println("Repeated.");
}
