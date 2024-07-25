# VersatileSwitch
タクトスイッチのようなモーメンタリ型スイッチをArduinoで容易に扱うためのライブラリです。

このライブラリではスイッチ接点のチャタリング除去（デバウンシング）をおこなうとともに、シングルクリック/ダブルクリックや「長押し（ロングクリック）」のほかに、スイッチを押し続けた場合の「自動連続押下（リピート）」といった様々なスイッチ動作を検出することができます。

これらの検出は、「isClicked()」のような状態取得関数が返す真偽値による検出のほかに、各スイッチ動作にコールバック関数を割り当てることによってイベント-ドリブン的な検出をすることもできるようになっています。

## 使い方
各スイッチについてVersatileSwitchのインスタンスを生成します。引数はピン番号が必須で、すべてのデジタル入力ピンを使用することができます。

```C++
VersatileSwitch mySwitch(4);
```

ピン番号のみを与えた場合、そのスイッチは
- スイッチの接続モードは「 ```INPUT_PULLUP``` 」
- スイッチが「押された（ ```ON``` ）」場合のピン電圧は ```LOW```

であるものと設定されます。

これら「接続モード」と「 ```ON``` 時のピン電圧」はインスタンス生成時にオプション引数として与えることができます。

```C++
VersatileSwitch mySwitch1(3, INPUT_PULLUP);

VersatileSwitch mySwitch2(4, INPUT, LOW);
```

第2引数はスイッチの「接続モード」で、```INPUT``` か ```INPUT_PULLUP``` のどちらかを与えます（省略した場合のデフォルト値は ```INPUT_PULLUP```）。

第3引数はスイッチが「押された（ ```ON``` ）」と判定するピン電圧で、 ```LOW```・```HIGH```・```AUTO``` のいずれかを与えます（省略した場合のデフォルト値は ```AUTO```）。

第3引数が ```AUTO``` の場合、
- 第2引数が ```INPUT``` なら ```HIGH```
- 第2引数が ```INPUT_PULLUP``` なら ```LOW```

と見做されます。

すべてのインスタンスについて、スイッチ状態の確認と更新のために定期的に ```poll()``` を呼び出します。通常の場合、 ```loop()``` 内で呼び出すことになります。

```poll()``` を呼び出してスイッチ状態を確認・更新した後、それらを状態取得関数で取得することができます。

```C++
void loop() {
    mySwitch.poll();

    if (mySwitch.isClicked()) {

        Serial.println("Clicked.");

    } else if (mySwitch.isLongClicked()) {

        Serial.println("Long-clicked.");

    }
}
```

また、 ```setup()``` 内でスイッチ動作にコールバック関数を割り当てておくと、```loop()``` 内で ```poll()```を呼んだ際に、それらの動作がおこなわれていればコールバック関数が自動的に呼び出されます。

```C++
void setup() {

    mySwitch.attachCallback_Clicked(on_switch_clicked);
    
    mySwitch.attachCallback_LongClicked(on_switch_long_clicked);

}

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
```C++
void func() {
    ...
}
```
である関数のポインタとなっています。

各関数がコールバックされるタイミングは、それぞれの動作が確定したと判定された ```poll()``` 内からとなっています。

## 時定数設定関数
### setTimeParalyze(uint32_t)
スイッチのチャタリング除去（デバウンシング）のための「麻痺」時間（ T<sub>Paralyze</sub> ）をミリ秒単位で設定します（デフォルト値は「5」ミリ秒）。

### setTimeUntilHold(uint32_t)
スイッチを押し始めてから「自動連続押下（リピート）」状態に至るまでの時間（ T<sub>Pressing</sub> ）をミリ秒単位で設定します（デフォルト値は「500」ミリ秒）。

### setTimeRepeatInterval(uint32_t)
「自動連続押下（リピート）」状態中の自動押下発生のインターバル時間（ T<sub>Repeat</sub> ）をミリ秒単位で設定します（デフォルト値は「500」ミリ秒）。

### setTimeAcceptDoubleClick(uint32_t)
スイッチ動作が「クリック」と判定された後、ある時間以内にスイッチが押されなければシングルクリックが確定されます。このダブルクリック受付時間（ T<sub>Accept</sub> ）をミリ秒単位で設定します（デフォルト値は「200」ミリ秒）。


## 内部動作について
### スイッチのチャタリング除去（デバウンシング）と ```Pressed``` / ```Released``` コールバック

```poll()``` ごとにスイッチ位置 [ ```ON```, ```OFF``` ] を確認します。その位置が前回 ```poll()``` 時と異なっていた場合は麻痺状態になり、麻痺している「 T<sub>Paralyze</sub> 」時間中の ```poll()``` では何もおこなわれません。

麻痺が終了した最初の ```poll()``` でスイッチ位置を再度確認し、その位置に応じて ```Pressed``` や ```Released``` がコールバックされます。それに伴い、スイッチのステータスが「**RELEASED**」や「**PRESSED**」に変化します。

![Figure_debauncing](https://github.com/kanitawa/VersatileSwitch/blob/images/figure_debauncing.png)

### スイッチの連続押下と ```Held``` / ```Repeated``` / ```LongClicked``` コールバック
スイッチの押下によってステータスが「**PRESSED**」になった後、「 T<sub>Pressing</sub> 」時間を経過した最初の ```poll()``` でまだスイッチが押し続けられていたならば、ステータスを「**HELD**」に変化させ、そのタイミングで ```Held``` と ```Repeated``` がコールバックされます。

その後は、「 T<sub>Repeat</sub> 」を超えた最初の ```poll()``` ごとに ```Repeated``` がコールバックます。

そして、スイッチが離された後の最初の ```poll()``` で ```Released``` と ```LongClicked``` がコールバックされます。

![Figure_repeating](https://github.com/kanitawa/VersatileSwitch/blob/images/figure_repeat.png)

### クリック判定と ```Clicked``` / ```DoubleClicked``` コールバック
スイッチの押下によってステータスが「**PRESSED**」になった後、「 T<sub>Pressing</sub> 」時間以内にスイッチが離された場合、ステータスは「**RELEASED_AFTER_CLICK**」という特殊な状態に変化します。

そして「**RELEASED_AFTER_CLICK**」になってから「 T<sub>Accept</sub> 」時間以内にスイッチが押されなければ、シングルクリックであることが確定したとして ```Clicked``` がコールバックされ、ステータスは「**RELEASED**」に戻ります。

![Figure_click](https://github.com/kanitawa/VersatileSwitch/blob/images/figure_click.png)

またステータスが「**RELEASED_AFTER_CLICK**」になってから「 T<sub>Accept</sub> 」時間以内にスイッチが押された場合、ステータスは「**PRESSED_AFTER_CLICK**」という別の特殊状態に変化します。

そして「**PRESSED_AFTER_CLICK**」なってから「 T<sub>Pressing</sub> 」時間以内にスイッチが離されたなら、ダブルクリックが成立したものとして ```DoubleClicked``` がコールバックされ、ステータスが「**RELEASED**」に戻ります。

![Figure_double_click](https://github.com/kanitawa/VersatileSwitch/blob/images/figure_double_click.png)
