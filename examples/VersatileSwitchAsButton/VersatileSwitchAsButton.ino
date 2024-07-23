/*
    VersatileSwitchAsButton.ino
    Example code for using switch as a clickable button.
*/

#include <VersatileSwitch.h>

#define PIN_SW 7

VersatileSwitch btn(PIN_SW, INPUT_PULLUP);

void setup() {
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("*** demo VersatileSwitchAsButton ***");

  // attach callback functions using as a button
  btn.attachCallback_Clicked(on_button_clicked);
  btn.attachCallback_DoubleClicked(on_button_double_clicked);
  btn.attachCallback_LongClicked(on_button_long_clicked);
}

void loop() {
  // polling switches
  btn.poll();
}

// callback functions
void on_button_clicked() {
  Serial.println("Clicked.");
}

void on_button_double_clicked() {
  Serial.println("Double-Clicked.");
}

void on_button_long_clicked() {
  Serial.println("Long-Clicked.");
}