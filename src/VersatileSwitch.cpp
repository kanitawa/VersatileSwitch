/*
  VersatileSwitch.cpp
*/
#include "Arduino.h"
#include "VersatileSwitch.h"

// コンストラクタ
VersatileSwitch::VersatileSwitch(uint8_t p, uint8_t m) { // p : pin number, m : pin mode [INPUT, INPUT_PULLUP]
  // 時定数と状態の初期化
  time_paralyze = 5; // 麻痺時間は5ミリ秒
  time_press = 500; // 押下から保持まで500ミリ秒
  time_repeat = 500; // 保持中の反復間隔は500ミリ秒
  time_accept = 200; // クリック判定から200ミリ秒以内に押下があれば「CLICK_AND_PRESSED」と判定

  status = RELEASED; // 現在のキーの状態は「解放」

  // 時刻の初期化
  isParalyzing = false; // 麻痺中ではない
  ms_paralyzed = 0; // 麻痺開始時刻
  ms_pressed = 0; // 押下開始時刻
  ms_clicked = 0; // クリック判定時刻

  // コールバック関数の初期化
  is_press_attached = false; // callback_pressedがアタッチされていない
  is_click_attached = false; // callback_clickedがアタッチされていない
  is_repeat_attached = false; // callback_repeatedがアタッチされていない
  is_long_attached = false; // callback_long_clickedがアタッチされていない
  is_double_attached = false; // callback_double_clickedがアタッチされていない
  is_release_attached = false; // callback_releasedがアタッチされていない

  // ピンの初期化と割り当て
  pin = p; // ピン番号をセット
  if (m == INPUT) { // 動作モードによってスイッチオフとオンとなる電圧値を設定する
    mode = INPUT; vol_off = LOW; vol_on = HIGH;
  } else {
    mode = INPUT_PULLUP; vol_off = HIGH; vol_on = LOW;
  }
  pinMode(pin, mode); // ピンを入力としてセット

  // オン・オフ電圧値の設定
  vol_prev = vol_off; // 前回電圧をオフに設定する
}

// publicメンバー関数
// 押下状態遷移時のコールバック関数登録
void VersatileSwitch::attachCallback_Pressed(void(* func)(void)) {
  callback_pressed = func;
  is_press_attached = true;
}

// シングルクリック判定時のコールバック関数登録
void VersatileSwitch::attachCallback_Clicked(void(* func)(void)) {
  callback_clicked = func;
  is_click_attached = true;
}

// 保持状態遷移時のコールバック関数登録
void VersatileSwitch::attachCallback_Holded(void(* func)(void)) {
  callback_holded = func;
  is_hold_attached = true;
}

// リピート発生時のコールバック関数登録
void VersatileSwitch::attachCallback_Repeated(void(* func)(void)) {
  callback_repeated = func;
  is_repeat_attached = true;
}

// 長押し発生時のコールバック関数登録
void VersatileSwitch::attachCallback_LongClicked(void(* func)(void)) {
  callback_long_clicked = func;
  is_long_attached = true;
}

// ダブルクリック判定時のコールバック関数登録
void VersatileSwitch::attachCallback_DoubleClicked(void(* func)(void)) {
  callback_double_clicked = func;
  is_double_attached = true;
}

// キーリリース時のコールバック関数登録
void VersatileSwitch::attachCallback_Released(void(* func)(void)) {
  callback_released = func;
  is_release_attached = true;
}

// 各種時間の設定関数
void VersatileSwitch::setTimeParalyze(uint32_t t) {time_paralyze = t;} // 麻痺時間の設定
void VersatileSwitch::setTimeUntilHold(uint32_t t) {time_press = t;} // 押下から保持までの時間の設定
void VersatileSwitch::setTimeRepeat(uint32_t t) {time_repeat = t;} // 保持後のキーリピート時間の設定
void VersatileSwitch::setTimeAcceptDoubleClick(uint32_t t) {time_accept = t;} // クリック後にダブルクリックを受け付ける時間の設定

// 非同期的な状態の取得
boolean VersatileSwitch::isOn() { // オンかどうか
  switch (status) {
    case PRESSED:
    case HOLDED:
    case CLICK_AND_PRESSED:
      return true;
      break;
    default: return false;
  }
}

boolean VersatileSwitch::isOff() { // オフかどうか
  return !isOn();
}

// スイッチのポーリングと動作状態遷移によるコールバックの呼び出し
void VersatileSwitch::poll() {
  if (isParalyzing) { // 麻痺中であれば
    if (millis() - ms_paralyzed > time_paralyze) { // 麻痺時間を超過していた
      vol_curr = digitalRead(pin); // 現在の電圧値を取得
      if (vol_prev != vol_curr) { // 麻痺開始時と現在の電圧値が異なる
        if (vol_curr == vol_on) { // 現在の電圧値がオンなので押下と判断する
          if (is_press_attached) callback_pressed(); // 押下時コールバックが設定されてれば呼ぶ
          ms_pressed = millis(); // 押下開始時刻を更新
          switch (status) {
            case RELEASED: // 解放->押下なので状態を押下中に更新
              status = PRESSED;
              break;
            case CLICKED: // クリック後次押下受付時間中に押下されたのでクリック後押下中に更新
              status = CLICK_AND_PRESSED;
              break;
            default: break;
          }
        } else { // 現在の電圧値がオンではないので解放と判断する
          if (is_release_attached) callback_released(); // 解放時コールバックが設定されてれば呼ぶ
          switch (status) { // スイッチ状態が
            case PRESSED: // 押下中だったのでシングルクリックと判定する
              status = CLICKED; // 状態をシングルクリックに更新
              ms_clicked = millis(); // クリック判定時刻を更新
              break;
            case CLICK_AND_PRESSED: // クリック後押下中だったのでダブルクリックと判定する
              if (is_double_attached) callback_double_clicked(); // ダブルクリック時コールバックが設定されてれば呼ぶ
              ms_pressed = 0; ms_clicked = 0;
              status = RELEASED; // 状態を解放中に更新
              break;
            case HOLDED: // 保持中だったので長押しと判断する
              if (is_long_attached) callback_long_clicked(); // ロングクリック時コールバックが設定されてれば呼ぶ
              ms_pressed = 0; ms_clicked = 0;
              status = RELEASED; // 状態を解放中に更新
              break;
            default: break;
          }
        }
        vol_prev = vol_curr; // 前回電圧値を更新
      } // 麻痺開始時と現在の電圧値が同じなので、この麻痺はグリッチによるものと判断し何もしない
      isParalyzing = false; // 麻痺を解除
    } // 麻痺時間以内であれば何もしない
  } else { // 麻痺中でなければ
    vol_curr = digitalRead(pin); // 現在の電圧値を取得
    if (vol_prev != vol_curr) { // 前回と今回の電圧値が異なる
      isParalyzing = true; // 麻痺させる
      ms_paralyzed = millis();
    } else { // 前回と今回の電圧値が同じである
      switch (status) {
        case PRESSED: // 押下中だった
          if (millis() - ms_pressed > time_press) { // 保持時間を超過していた
            if (is_hold_attached) callback_holded(); // 保持開始時コールバックが設定されてれば呼ぶ
            if (is_repeat_attached) callback_repeated(); // リピート時コールバックが設定されてれば呼ぶ
            status = HOLDED; // 状態を保持に更新
            ms_pressed = millis(); // 押下開始時刻を保持開始時刻に更新
          } // 押下中だがまだ保持時間に達していないので何もしない
          break;
        case HOLDED: // 保持中だった
          if (millis() - ms_pressed > time_repeat) { // 反復時間を超えて押下され続けてた
            if (is_repeat_attached) callback_repeated(); // キーリピート時コールバックが設定されてれば呼ぶ
            ms_pressed = millis(); // 押下開始時刻を保持開始時刻に更新
          } // 押下中だがまだ保持時間に達していない
          break;
        case CLICKED: // クリック後の次押下受付中だった
          if (millis() - ms_clicked > time_accept) { // クリック後の次押下受付時間を超過していたのでシングルクリックが確定
            if (is_click_attached) callback_clicked(); // クリック時コールバックが設定されてれば呼ぶ
            status = RELEASED; // 状態を解放に更新
          }
          break;
        case CLICK_AND_PRESSED: // クリック後押下中だった
          if (millis() - ms_pressed > time_press) { // 保持時間を超過していた
            if (is_click_attached) callback_clicked(); // クリック時コールバックが設定されてれば呼ぶ
            if (is_hold_attached) callback_holded(); // 保持開始時コールバックが設定されてれば呼ぶ
            if (is_repeat_attached) callback_repeated(); // リピート時コールバックが設定されてれば呼ぶ
            status = HOLDED; // 状態を保持に更新
            ms_pressed = millis(); // 押下開始時刻を保持開始時刻に更新
            ms_clicked = 0;
          } // 押下中だがまだ保持時間に達していないので何もしない
          break;
        default: break;
      }
    }
  }
}








