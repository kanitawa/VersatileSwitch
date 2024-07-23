/*
  VersatileSwitch.cpp
*/

#include "Arduino.h"
#include "VersatileSwitch.h"

// constructor
VersatileSwitch::VersatileSwitch(uint8_t p, uint8_t m) {
  // initial values of time constants
  time_paralyze = 5;
  time_press = 500;
  time_repeat = 500;
  time_accept = 200;

  // initial value of status
  status = RELEASED;

  // initialize times
  isParalyzing = false;
  ms_paralyzed = 0;
  ms_pressed = 0;
  ms_clicked = 0;

  // initialize callbackes
  // all callback functtions are not attached.
  is_press_attached = false;
  is_click_attached = false;
  is_repeat_attached = false;
  is_long_attached = false;
  is_double_attached = false;
  is_release_attached = false;

  // asign pin mode
  if (m == INPUT) { // when mode is INPUT, OFF = LOW and ON = HIGH.
    mode = INPUT; vol_off = LOW; vol_on = HIGH;
  } else { // when mode is INPUT_PULLUP, OFF = HIGH and ON = LOW.
    mode = INPUT_PULLUP; vol_off = HIGH; vol_on = LOW;
  }
  pin = p;
  pinMode(pin, mode);

  // initialize values of pin voltage
  vol_prev = vol_off;
}

// public member functions
// attach callback funtions
void VersatileSwitch::attachCallback_Pressed(void(* func)(void)) {
  callback_pressed = func;
  is_press_attached = true;
}

void VersatileSwitch::attachCallback_Clicked(void(* func)(void)) {
  callback_clicked = func;
  is_click_attached = true;
}

void VersatileSwitch::attachCallback_Holded(void(* func)(void)) {
  callback_holded = func;
  is_hold_attached = true;
}

void VersatileSwitch::attachCallback_Repeated(void(* func)(void)) {
  callback_repeated = func;
  is_repeat_attached = true;
}

void VersatileSwitch::attachCallback_LongClicked(void(* func)(void)) {
  callback_long_clicked = func;
  is_long_attached = true;
}

void VersatileSwitch::attachCallback_DoubleClicked(void(* func)(void)) {
  callback_double_clicked = func;
  is_double_attached = true;
}

void VersatileSwitch::attachCallback_Released(void(* func)(void)) {
  callback_released = func;
  is_release_attached = true;
}

// setter for time constants
void VersatileSwitch::setTimeParalyze(uint32_t t) {time_paralyze = t;}
void VersatileSwitch::setTimeUntilHold(uint32_t t) {time_press = t;}
void VersatileSwitch::setTimeRepeat(uint32_t t) {time_repeat = t;}
void VersatileSwitch::setTimeAcceptDoubleClick(uint32_t t) {time_accept = t;}

// getter for switch value [ON, OFF]
boolean VersatileSwitch::isOn() {
  switch (status) {
    case PRESSED:
    case HOLDED:
    case CLICK_AND_PRESSED:
      return true;
      break;
    default: return false;
  }
}

boolean VersatileSwitch::isOff() {return !isOn();}

// polling switch status and callback
void VersatileSwitch::poll() {
  if (isParalyzing) { // in paralyzing
    if (millis() - ms_paralyzed > time_paralyze) { // time over paralyzing
      vol_curr = digitalRead(pin);
      if (vol_prev != vol_curr) { // current value is different from before paralyzing 
        if (vol_curr == vol_on) { // current value is ON, switch is pressing
          if (is_press_attached) callback_pressed(); // callback "pressed" if attached
          ms_pressed = millis(); // update starting time of pressing
          switch (status) {
            case RELEASED: // status RELEASED -> PRESSED = PRESSED
              status = PRESSED;
              break;
            case CLICKED: // status CLICKED -> PRESSED = CLICK_AND_PRESSED
              status = CLICK_AND_PRESSED;
              break;
            default: break;
          }
        } else { // current value is OFF, switch is released
          if (is_release_attached) callback_released(); // callback "released" if attached
          switch (status) {
            case PRESSED: // status PRESSED -> RELEASED = CLICKED
              status = CLICKED;
              ms_clicked = millis(); // update completed time of clicking
              break;
            case CLICK_AND_PRESSED: // status CLICK_AND_PRESSED -> RELEASED = double-clicked
              if (is_double_attached) callback_double_clicked(); // callback "double_clicked" if attached
              ms_pressed = 0; ms_clicked = 0;
              status = RELEASED; // status update to RELEASED
              break;
            case HOLDED: // status HOLDED -> RELEASED = long_clicked
              if (is_long_attached) callback_long_clicked(); // callback "long_clicked" if attached
              ms_pressed = 0; ms_clicked = 0;
              status = RELEASED; // status update to RELEASED
              break;
            default: break;
          }
        }
        vol_prev = vol_curr; // update previous value
      }
      isParalyzing = false;
    }
  } else { // not in paralyzing
    vol_curr = digitalRead(pin);
    if (vol_prev != vol_curr) { // switch pin voltage change from previous polling
      isParalyzing = true; // move into paralyze
      ms_paralyzed = millis(); // update starting time of paralyzing
    } else { // switch pin voltage is same with previous polling
      switch (status) {
        case PRESSED: // continue to PRESSED
          if (millis() - ms_pressed > time_press) { // if time over pressing,
            if (is_hold_attached) callback_holded(); // callback "holded" if attached
            if (is_repeat_attached) callback_repeated(); // callback "repeated" if attached
            status = HOLDED;
            ms_pressed = millis(); // update starting time of pressing
          }
          break;
        case HOLDED: // continue to HOLDED
          if (millis() - ms_pressed > time_repeat) { // if time over repeating,
            if (is_repeat_attached) callback_repeated(); // callback "repeated" if attached
            ms_pressed = millis(); // update starting time of pressing
          }
          break;
        case CLICKED: // continue to CLICKED
          if (millis() - ms_clicked > time_accept) { // if status is NOT changed during time_accept
            if (is_click_attached) callback_clicked(); // callback "clicked" if attached
            status = RELEASED;
          }
          break;
        case CLICK_AND_PRESSED: // continue to CLICK_AND_PRESSED
          if (millis() - ms_pressed > time_press) { // if status is NOT changed during time_press
            if (is_click_attached) callback_clicked(); // callback "clicked" if attached
            if (is_hold_attached) callback_holded(); // callback "holded" if attached
            if (is_repeat_attached) callback_repeated(); // リcallback "repeated" if attached
            status = HOLDED;
            ms_pressed = millis(); // update starting time of pressing
            ms_clicked = 0;
          }
          break;
        default: break;
      }
    }
  }
}








