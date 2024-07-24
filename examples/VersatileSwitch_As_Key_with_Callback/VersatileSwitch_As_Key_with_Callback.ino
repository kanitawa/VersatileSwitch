/*
    VersatileSwitch_As_Key_with_Callback.ino
    Example code for switch as a key using callback functions.
*/

#include <VersatileSwitch.h>

#define PIN_SW 7

VersatileSwitch key(PIN_SW, INPUT_PULLUP);

void setup() {
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("*** demo VersatileSwitch_As_Key_with_Callback ***");

  // attach callback functions using as a key
  key.attachCallback_Pressed(on_key_pressed);
  key.attachCallback_Repeated(on_key_repeated);
}

void loop() {
  // polling switches
  key.poll();
}

// callback functions
void on_key_pressed() {
  Serial.println("Pressed.");
}

void on_key_repeated() {
  Serial.println("Repeated.");
}
