/*
  VersatileSwitch.h
*/

#ifndef __VERSATILE_SWITCH_H_INCLUDED__
#define __VERSATILE_SWITCH_H_INCLUDED__

#include "Arduino.h"

// Enum status of switch
enum SWITCH_STATUS {RELEASED, PRESSED, HOLDED, CLICKED, CLICK_AND_PRESSED};

class VersatileSwitch {
  private:
    uint8_t pin; // pin number for switch
    uint8_t mode; // mode of input [INPUT, INPUT_PULLUP]

    uint8_t vol_off, vol_on; // [HIGH, LOW] value for OFF and ON
    uint8_t vol_prev, vol_curr; // [HIGH, LOW] value of switch pin in prevoius and current polling

    uint32_t time_paralyze; // msec. of paralyzing for debouncing
    uint32_t time_press; // msec. from press start to transition hold
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

    boolean is_hold_attached; // true if callback_clicked is attached
    void(* callback_holded)(void); // callback function for hold

    boolean is_repeat_attached; // true if callback_repeated is attached
    void(* callback_repeated)(void); // callback function for repeat

    boolean is_long_attached; // true if callback_long_clicked is attached
    void(* callback_long_clicked)(void); // callback function for long-click

    boolean is_double_attached; // true if callback_double_clicked is attached
    void(* callback_double_clicked)(void); // callback function for double-click
    
    boolean is_release_attached; // true if callback_released is attached
    void(* callback_released)(void); // callback function for releasing switch

  public:
    VersatileSwitch(uint8_t, uint8_t); // constructor

    void attachCallback_Pressed(void(* func)(void));
    void attachCallback_Clicked(void(* func)(void));
    void attachCallback_Holded(void(* func)(void));
    void attachCallback_Repeated(void(* func)(void));
    void attachCallback_LongClicked(void(* func)(void));
    void attachCallback_DoubleClicked(void(* func)(void));
    void attachCallback_Released(void(* func)(void));

    void setTimeParalyze(uint32_t);
    void setTimeUntilHold(uint32_t); 
    void setTimeRepeat(uint32_t);
    void setTimeAcceptDoubleClick(uint32_t);

    boolean isOn(void); 
    boolean isOff(void);
    
    void poll(void);
};

#endif
