/*
  VersatileSwitch.h
*/

#ifndef __VERSATILE_SWITCH_H_INCLUDED__
#define __VERSATILE_SWITCH_H_INCLUDED__

#include "Arduino.h"

// Enum status of switch
enum SWITCH_STATUS {RELEASED, PRESSED, HELD, RELEASED_AFTER_CLICK, PRESSED_AFTER_CLICK};

// macros
#define AUTO 0xff

class VersatileSwitch {
  private:
    const uint8_t pin; // pin number for switch
    const uint8_t mode; // mode of input [INPUT, INPUT_PULLUP]
    const uint8_t value; // [LOW, HIGH] of switch ON, if (value != LOW && value != HIGH) then decided from mode.

    uint8_t vol_off, vol_on; // [LOW, HIGH] for OFF and ON
    uint8_t vol_prev, vol_curr; // [LOW, HIGH] of switch pin in prevoius and current polling

    uint32_t time_paralyze; // msec. of paralyzing for debouncing
    uint32_t time_press; // msec. from start of pressing to transition to hold
    uint32_t time_repeat; // msec. for repeat interval in hold state
    uint32_t time_accept; // msec. for accepting double-click after first-click 

    SWITCH_STATUS status; // current status of switch

    boolean isParalyzing; // true in paralyzing for debouncing
    unsigned long ms_paralyzed; // starting time of paralyzing
    unsigned long ms_pressed; // starting time of pressing
    unsigned long ms_clicked; // completed time of clicking

    boolean is_press_attached; // true if callback_pressed is attached
    void(* callback_pressed)(void); // callback function for pressing switch

    boolean is_click_attached; // true if callback_clicked is attached
    void(* callback_clicked)(void); // callback function for single-click

    boolean is_hold_attached; // true if callback_held is attached
    void(* callback_held)(void); // callback function for held

    boolean is_repeat_attached; // true if callback_repeated is attached
    void(* callback_repeated)(void); // callback function for repeat

    boolean is_long_attached; // true if callback_long_clicked is attached
    void(* callback_long_clicked)(void); // callback function for long-click

    boolean is_double_attached; // true if callback_double_clicked is attached
    void(* callback_double_clicked)(void); // callback function for double-click
    
    boolean is_release_attached; // true if callback_released is attached
    void(* callback_released)(void); // callback function for releasing switch

    boolean is_clicked; // one-time variable for detect click
    boolean is_long_clicked; // one-time variable for detect click
    boolean is_double_clicked; // one-time variable for detect click

  public:
    // constructor
    VersatileSwitch(uint8_t p, uint8_t m = INPUT_PULLUP, uint8_t v = AUTO,
      uint32_t t_pa = 5, uint32_t t_pr = 500, uint32_t t_re = 500, uint32_t t_ac = 200);

    void attachCallback_Pressed(void(* func)(void));
    void attachCallback_Clicked(void(* func)(void));
    void attachCallback_Held(void(* func)(void));
    void attachCallback_Repeated(void(* func)(void));
    void attachCallback_LongClicked(void(* func)(void));
    void attachCallback_DoubleClicked(void(* func)(void));
    void attachCallback_Released(void(* func)(void));

    void setTimeParalyze(uint32_t);
    void setTimeUntilHold(uint32_t); 
    void setTimeRepeatInterval(uint32_t);
    void setTimeAcceptDoubleClick(uint32_t);

    boolean isOn(void); 
    boolean isOff(void);
    
    boolean isPressed(void);
    boolean isReleased(void);
    boolean isHeld(void);

    boolean isClicked(void);
    boolean isLongClicked(void);
    boolean isDoubleClicked(void);
    
    void poll(void);
};

#endif
