/*
  VersatileSwitch.cpp
*/

#include "Arduino.h"
#include "VersatileSwitch.h"

// constructor
VersatileSwitch::VersatileSwitch(uint8_t p, uint8_t m, uint8_t v, uint32_t t_pa, uint32_t t_pr, uint32_t t_re, uint32_t t_ac)
    :pin(p), mode(m), value(v), time_paralyze(t_pa), time_press(t_pr), time_repeat(t_re), time_accept(t_ac) {
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
  is_hold_attached = false;
  is_repeat_attached = false;
  is_long_attached = false;
  is_double_attached = false;
  is_release_attached = false;
  is_finalize_attached = false;

  // all one-time variables are not detected.
  is_clicked = false;
  is_long_clicked = false;
  is_double_clicked = false; 

  // decide polarity and assign pin mode
  switch (value) {
    case LOW:
    case HIGH:
      vol_on = value;
      break;
    default:
      vol_on = (mode == INPUT)?HIGH:LOW;
      break;
  }
  vol_off = !vol_on;
  pinMode(pin, mode);

  // initialize values of pin position
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

void VersatileSwitch::attachCallback_Held(void(* func)(void)) {
  callback_held = func;
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

void VersatileSwitch::attachCallback_Finalized(void(* func)(void)) {
  callback_finalized = func;
  is_finalize_attached = true;
}

// setter for time constants
void VersatileSwitch::setTimeParalyze(uint32_t t) {time_paralyze = t;}
void VersatileSwitch::setTimeUntilHold(uint32_t t) {time_press = t;}
void VersatileSwitch::setTimeRepeatInterval(uint32_t t) {time_repeat = t;}
void VersatileSwitch::setTimeAcceptDoubleClick(uint32_t t) {time_accept = t;}

// getter for switch value [ON, OFF]
boolean VersatileSwitch::isOff() {
  switch (status) {
    case RELEASED:
    case RELEASED_AFTER_CLICK:
      return true;
      break;
    default: return false;
  }
}

boolean VersatileSwitch::isOn() {return !isOff();}

boolean VersatileSwitch::isReleased() {return isOff();}
boolean VersatileSwitch::isPressed() {return !isOff();}
boolean VersatileSwitch::isHeld() {return (status == HELD);}

boolean VersatileSwitch::isClicked() {return is_clicked;}
boolean VersatileSwitch::isLongClicked() {return is_long_clicked;}
boolean VersatileSwitch::isDoubleClicked() {return is_double_clicked;}

// polling switch status and callback
void VersatileSwitch::poll() {
  // all one-time variables set to false;
  is_clicked = false; is_long_clicked = false; is_double_clicked = false;

  // poll status of switch
  if (isParalyzing) { // in paralyzing
    if (millis() - ms_paralyzed > time_paralyze) { // time over paralyzing
      vol_curr = digitalRead(pin);
      if (vol_prev != vol_curr) { // current value is different from before paralyzing 
        if (vol_curr == vol_on) { // current value is ON, switch is pressing
          if (is_press_attached) callback_pressed(); // callback "pressed" if attached
          ms_pressed = millis(); // update starting time of pressing
          switch (status) {
            case RELEASED:
              status = PRESSED;
              break;
            case RELEASED_AFTER_CLICK:
              status = PRESSED_AFTER_CLICK;
              break;
            default: break;
          }
        } else { // current value is OFF, switch is released
          if (is_release_attached) callback_released(); // callback "released" if attached
          switch (status) {
            case PRESSED:
              status = RELEASED_AFTER_CLICK;
              ms_clicked = millis(); // update completed time of clicking
              break;
            case PRESSED_AFTER_CLICK:
              if (is_double_attached) callback_double_clicked(); // callback "double_clicked" if attached
              if (is_finalize_attached) callback_finalized();
              is_double_clicked = true;
              ms_pressed = 0; ms_clicked = 0;
              status = RELEASED;
              break;
            case HELD:
              if (is_long_attached) callback_long_clicked(); // callback "long_clicked" if attached
              if (is_finalize_attached) callback_finalized();
              is_long_clicked = true;
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
        case PRESSED:
          if (millis() - ms_pressed > time_press) { // if time over pressing,
            if (is_hold_attached) callback_held(); // callback "holded" if attached
            if (is_repeat_attached) callback_repeated(); // callback "repeated" if attached
            status = HELD;
            ms_pressed = millis(); // update starting time of pressing
          }
          break;
        case HELD:
          if (millis() - ms_pressed > time_repeat) { // if time over repeating,
            if (is_repeat_attached) callback_repeated(); // callback "repeated" if attached
            ms_pressed = millis(); // update starting time of pressing
          }
          break;
        case RELEASED_AFTER_CLICK:
          if (millis() - ms_clicked > time_accept) { // if status is NOT changed during time_accept
            if (is_click_attached) callback_clicked(); // callback "clicked" if attached
            if (is_finalize_attached) callback_finalized();
            is_clicked = true;
            status = RELEASED;
          }
          break;
        case PRESSED_AFTER_CLICK:
          if (millis() - ms_pressed > time_press) { // if status is NOT changed during time_press
            if (is_click_attached) callback_clicked(); // callback "clicked" if attached
            // this sequence is ** CLICK AND HOLD **, don't callback finalized after clicked!
            is_clicked = true;
            if (is_hold_attached) callback_held(); // callback "holded" if attached
            if (is_repeat_attached) callback_repeated(); // リcallback "repeated" if attached
            status = HELD;
            ms_pressed = millis(); // update starting time of pressing
            ms_clicked = 0;
          }
          break;
        default: break;
      }
    }
  }
}








