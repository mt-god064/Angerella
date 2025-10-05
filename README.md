# Angerella

## 概要 (Overview)

このリポジトリは、**6軸加速度センサー**によってデバイスが移動していることを検知したり、特定のリズムの振動を認識させたり、**指紋認証センサー**によるユーザー認証システムや、それら検知、認証の結果に応じて**サーボモーター**を**Arduino UNO R4 WiFi**によって制御するシステムです。


---

## 特徴 (Features)

* 指紋の照合機能
* 6軸加速度センサーのデータ取得（振動、加速度）
* 指紋認証成功時にサーボモーターを特定角度に動かす制御
* 認証の結果に応じて、反応を音声やサーボモーターの動きを出力

---

## 必要なもの (Hardware Requirements)

このプロジェクトを再現するためには、以下のハードウェアが必要です。

* **メインボード:** Arduino UNO R4 WiFi
* **センサー:**
    * 指紋認証センサー [UART Fingerprint Sensor (F)](https://www.waveshare.com/wiki/UART_Fingerprint_Sensor_(F))
    * 6軸加速度センサー  MPU-6050
    * サーボモーター Tower Pro SG90
* **その他:**
    * ブレッドボード
    * ジャンパーワイヤー
    * スピーカー
    * 9Vバッテリー

---

## セットアップ (Setup)

    * `Servo` (標準ライブラリ)
    * `Wire` (標準ライブラリ)
    * `SoftwareSerial` (標準ライブラリ)
    * `DFPlayer_Mini_Mp3` (https://github.com/DFRobot/DFPlayer-Mini-mp3)
---

## 参考・謝辞 (Credits)

このプロジェクトの指紋認証センサーのコードの一部は、Waveshareの提供する以下のデモコードを参考にしています。

**UART Fingerprint Sensor (F) Wiki:** [https://www.waveshare.com/wiki/UART_Fingerprint_Sensor_(F)]([https://www.waveshare.com/wiki/UART_Fingerprint_Sensor_(F)](https://www.waveshare.com/wiki/UART_Fingerprint_Sensor_(F)))
(Resources->Demo)

また、このプロジェクトは DFPlayer Mini MP3 ライブラリ (LGPL v3) を使用しています。

---

## ライセンス (License)

このプロジェクトは [MIT License](LICENSE) のもとで公開されています。
