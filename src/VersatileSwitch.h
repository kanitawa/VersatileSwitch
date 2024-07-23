/*
  VersatileSwitch.h
*/
#ifndef __VERSATILE_SWITCH_H_INCLUDED__
#define __VERSATILE_SWITCH_H_INCLUDED__

#include "Arduino.h"

// 状態の列挙 {解放, 押下, 保持, シングルクリック, シングルクリック後押下, ダブルクリック}
enum SWITCH_STATUS {RELEASED, PRESSED, HOLDED, CLICKED, CLICK_AND_PRESSED};

class VersatileSwitch {
  private:
    // スイッチのオン・オフ値
    uint8_t vol_off, vol_on; // スイッチがオフとオンとなる電圧値 [HIGH, LOW]
    uint8_t vol_prev, vol_curr; // 前回poll時と今回poll時のスイッチ電圧

    // 時定数
    uint32_t time_paralyze; // 麻痺するミリ秒
    uint32_t time_press; // 押下開始から保持に遷移するまでのミリ秒
    uint32_t time_repeat; // 保持中における反復間隔のミリ秒
    uint32_t time_accept; // クリック完了後に次押下を受け付けるミリ秒

    SWITCH_STATUS status; // 現在のキーの状態

    boolean isParalyzing; // 麻痺中かどうか
    unsigned long ms_paralyzed; // 麻痺開始時刻
    unsigned long ms_pressed; // 押下開始時刻
    unsigned long ms_clicked; // クリック判定時刻

    boolean is_press_attached; // callback_pressedがアタッチされてるかどうか
    void(* callback_pressed)(void); // press時のコールバック関数

    boolean is_click_attached; // callback_clickedがアタッチされてるかどうか
    void(* callback_clicked)(void); // シングルクリック時のコールバック関数

    boolean is_hold_attached; // callback_clickedがアタッチされてるかどうか
    void(* callback_holded)(void); // hold時のコールバック関数

    boolean is_repeat_attached; // callback_repeatedがアタッチされてるかどうか
    void(* callback_repeated)(void); // repeat時のコールバック関数

    boolean is_long_attached; // callback_long_clickedがアタッチされてるかどうか
    void(* callback_long_clicked)(void); // long_click時のコールバック関数

    boolean is_double_attached; // callback_double_clickedがアタッチされてるかどうか
    void(* callback_double_clicked)(void); // ダブルクリック時のコールバック関数
    
    boolean is_release_attached; // callback_releasedがアタッチされてるかどうか
    void(* callback_released)(void); // release時のコールバック関数

  public:
    uint8_t pin; // 割り当てられてるピン番号
    uint8_t mode; // 割り当てられたピンの動作モード [INPUT, INPUT_PULLUP]

    // コンストラクタ
    VersatileSwitch(uint8_t, uint8_t); // ピン番号, ピンモード

    // キー動作のコールバック登録
    void attachCallback_Pressed(void(* func)(void));
    void attachCallback_Clicked(void(* func)(void));
    void attachCallback_Holded(void(* func)(void));
    void attachCallback_Repeated(void(* func)(void));
    void attachCallback_LongClicked(void(* func)(void));
    void attachCallback_DoubleClicked(void(* func)(void));
    void attachCallback_Released(void(* func)(void));

    // 各種時間の設定
    void setTimeParalyze(uint32_t); // 麻痺時間の設定
    void setTimeUntilHold(uint32_t); // 押下から保持までの時間の設定
    void setTimeRepeat(uint32_t); // 保持後のリピート時間の設定
    void setTimeAcceptDoubleClick(uint32_t); // クリック後にダブルクリックを受け付ける時間の設定

    // 非同期的な状態の取得
    boolean isOn(void); // オンかどうか
    boolean isOff(void); // オフかどうか
    
    // スイッチのポーリングと動作状態遷移によるコールバックの呼び出し
    void poll(void);
};

#endif
