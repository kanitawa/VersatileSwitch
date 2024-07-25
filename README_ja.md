# VersatileSwitch
タクトスイッチのようなモーメンタリ型スイッチをArduinoで容易に扱うためのライブラリです。

このライブラリではスイッチ接点のチャタリング除去（デバウンシング）をおこなうとともに、シングルクリック/ダブルクリックや「長押し（ロングクリック）」のほかに、スイッチを押し続けた場合の「自動連続押下（リピート）」といった様々なスイッチ動作を検出することができます。

これらの検出は、「isClicked()」のような状態取得関数が返す真偽値による検出のほかに、各スイッチ動作にコールバック関数を割り当てることによってイベント-ドリブン的な検出をすることもできるようになっています。

## 使い方
各スイッチについて、ピン番号とモードを引数としてVersatileSwitchのインスタンスを生成します。

```
VersatileSwitch mySwitch(4, INPUT_PULLUP); // pin 4, and using internal pull-up
```

スイッチ信号を接続するピンとして、すべてのデジタル入力ピンを使用することができます。

モードには``` INPUT ```か``` INPUT_PULLUP ```を選択できます。

モードに``` INPUT ```を与えた場合、ピン電圧が

- ``` HIGH ```なら、スイッチが「押された（ ```ON``` ）」
- ``` LOW ```ならスイッチが「離れた（ ```OFF``` ）」

と判定します。

また、モードに``` INPUT_PULLUP ```を与えた場合では、

- ``` LOW ```なら、スイッチが「押された（ ```ON``` ）」
- ``` HIGH ```ならスイッチが「離れた（ ```OFF``` ）」

と判定します。

すべてのインスタンスについて、スイッチ状態の確認のために定期的に ```poll()``` を呼び出します。通常の場合、 ```loop()``` 内で呼び出すことになります。

```
mySwitch.poll();
```

```poll()``` を呼び出してスイッチ状態を確認・更新した後、それらを関数で取得することができます。

```
if (mySwitch.isClicked()) {

    Serial.println("Clicked.");

} else if (mySwitch.isLongClicked()) {

    Serial.println("Long-clicked.");

}
```

また、 ```setup()``` 内でスイッチ動作にコールバック関数を割り当てておくと、

```
void setup() {

    mySwitch.attachCallback_Clicked(on_switch_clicked);
    
    mySwitch.attachCallback_LongClicked(on_switch_long_clicked);

}
```

```loop()``` 内で ```poll()```を呼んだ際に、それらの動作がおこなわれていればコールバック関数が自動的に呼び出されます。

```
void loop() {

    mySwitch.poll();

}

void on_switch_clicked() {

    Serial.println("Clicked.");

}

void on_switch_long_clicked() {

    Serial.println("Long-clicked.");

}
```

## 状態取得関数
### isOn()
### isPressed()
スイッチが「押された（ ```ON``` ）」状態にあれば "true" を返します。

### isOff()
### isReleased()
スイッチが「離れた（ ```OFF``` ）」状態にあれば "true" を返します。

### isHeld()
スイッチが押され続けて「自動連続押下（リピート）」状態にあれば "true" を返します。この状態の場合、 ```isPressed()``` は "false" を返します。

### isClicked()
### isDoubleClicked()
### isLongClicked()
この関数を呼ぶ直前の ```poll()``` にてスイッチの「クリック / ダブルクリック / 長押し（ロングクリック）」動作が判定されている場合に "true" を返します。これらの関数の返り値は一過性で、"true" を返した ```poll()``` のさらに次の ```poll()``` では "false" を返します。

## コールバック割当て関数
### attachCallback_Pressed()
### attachCallback_Clicked()
### attachCallback_Held()
### attachCallback_Repeated()
### attachCallback_LongClicked()
### attachCallback_DoubleClicked()
### attachCallback_Released()
スイッチの各動作についてコールバック関数を割り当てます。引数の型は ```void(*)(void)``` であり、いわゆる
```
void func() {
    ...
}
```
である関数のポインタとなっています。

各関数がコールバックされるタイミングは、それぞれの動作が確定したと判定された ```poll()``` 内からとなっています。

## 時定数設定関数
### setTimeParalyze(uint32_t);
スイッチのチャタリング除去（デバウンシング）のための「麻痺」時間（ T<sub>paralyze</sub> ）をミリ秒単位で設定します。デフォルト値は「5」ミリ秒です。

### setTimeUntilHold(uint32_t);
スイッチを押し始めてから「自動連続押下（リピート）」状態に至るまでの時間（ T<sub>pressing</sub> ）をミリ秒単位で設定します。デフォルト値は「500」ミリ秒です。

### setTimeRepeatInterval(uint32_t);
「自動連続押下（リピート）」状態中の自動押下発生のインターバル時間（ T<sub>repeat</sub> ）をミリ秒単位で設定します。デフォルト値は「500」ミリ秒です

### setTimeAcceptDoubleClick(uint32_t);
スイッチ動作が「クリック」と判定された後、ある時間以内にスイッチが押されなければシングルクリックが確定されます。このダブルクリック受付時間（ T<sub>accept</sub> ）をミリ秒単位で設定します。デフォルト値は「200」ミリ秒です



## 内部動作について
### スイッチのチャタリング除去（デバウンシング）と ```Pressed / Released``` コールバック

```poll()``` ごとにスイッチ位置 [ ```ON```, ```OFF``` ] を確認します。その位置が前回 ```poll()``` 時と異なっていた場合は麻痺状態になり、麻痺している「 T<sub>Paralyze</sub> 」時間中の ```poll()``` では何もおこなわれません。

麻痺が終了した最初の ```poll()``` でスイッチ位置を再度確認し、その位置に応じて ```Pressed``` や ```Released``` がコールバックされます。それに伴い、スイッチのステータスが「**RELEASED**」や「**PRESSED**」に変化します。


### スイッチの連続押下と ```Held / Repeated / LongClicked``` コールバック
スイッチの押下によってステータスが「**PRESSED**」になった後、「 T<sub>Pressing</sub> 」時間を経過した最初の ```poll()``` でまだスイッチが押し続けられていたならば、ステータスを「**HELD**」に変化させ、そのタイミングで ```Held``` と ```Repeated``` がコールバックされます。

その後は、「 T<sub>Repeat</sub> 」を超えた最初の ```poll()``` ごとに ```Repeated``` がコールバックます。

そして、スイッチが離された後の最初の ```poll()``` で ```Released``` と ```LongClicked``` がコールバックされます。


### クリック判定と ```Clicked / DoubleClicked``` コールバック
スイッチの押下によってステータスが「**PRESSED**」になった後、「 T<sub>Pressing</sub> 」時間以内にスイッチが離された場合、ステータスは「**RELEASED_AFTER_CLICK**」という特殊な状態に変化します。

そして「**RELEASED_AFTER_CLICK**」になってから「 T<sub>Accept</sub> 」時間以内にスイッチが押されなければ、シングルクリックであることが確定したとして ```Clicked``` がコールバックされ、ステータスは「**RELEASED**」に戻ります。

またステータスが「**RELEASED_AFTER_CLICK**」になってから「 T<sub>Accept</sub> 」時間以内にスイッチが押された場合、ステータスは「**PRESSED_AFTER_CLICK**」という別の特殊状態に変化します。

そして「**PRESSED_AFTER_CLICK**」なってから「 T<sub>Pressing</sub> 」時間以内にスイッチが離されたなら、ダブルクリックが成立したものとして ```DoubleClicked``` がコールバックされ、ステータスが「**RELEASED**」に戻ります。
