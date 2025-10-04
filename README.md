# Angerella

## 概要 (Overview)

このリポジトリは、**Arduino UNO R4 WiFi** を使用して、**指紋認証センサー**、**6軸加速度センサー**、**サーボモーター**を制御するためのプロジェクトです。特定の指紋が認証されるとサーボモーターが作動したり、センサーの値を読み取ったりすることができます。



---

## 特徴 (Features)

* 指紋の登録、照合、削除機能
* 6軸加速度センサー（ジャイロ・加速度）のデータ取得
* 指紋認証成功時にサーボモーターを特定角度に動かす制御
* (もしあれば、その他プロジェクトのユニークな特徴をここに書く)

---

## 必要なもの (Hardware Requirements)

このプロジェクトを再現するためには、以下のハードウェアが必要です。

* **メインボード:** Arduino UNO R4 WiFi
* **センサー:**
    * [UART Fingerprint Sensor (F)](https://www.waveshare.com/wiki/UART_Fingerprint_Sensor_(F))
    * 6軸加速度センサー (具体的な型番、例えば MPU-6050 など)
* **アクチュエーター:** サーボモーター (具体的な型番、例えば SG90 など)
* **その他:**
    * ブレッドボード
    * ジャンパーワイヤー

---

## セットアップ (Setup)

1.  **Arduino IDEの準備:**
    * [Arduino IDE](https://www.arduino.cc/en/software) をダウンロードし、インストールしてください。
    * ボードマネージャから「Arduino UNO R4 Boards」をインストールし、IDEで `Arduino UNO R4 WiFi` を選択してください。

2.  **ライブラリのインストール:**
    Arduino IDEのライブラリマネージャ（`スケッチ` > `ライブラリをインクルード` > `ライブラリを管理`）から、以下のライブラリを検索してインストールしてください。

    * `Servo` (標準ライブラリ)
    * `Wire` (標準ライブラリ)
    * `SoftwareSerial` (標準ライブラリ)
    * `DFPlayer_Mini_Mp3` (もしMP3プレイヤー機能を使っている場合)

3.  **配線:**
    各コンポーネントを以下のようにArduinoに接続してください。
    (ここに簡単な配線図や接続ピンの情報を書くと、とても親切です)

    * **指紋センサー**
        * VCC -> 5V
        * GND -> GND
        * TX -> D2 (SoftwareSerial)
        * RX -> D3 (SoftwareSerial)
    * **6軸加速度センサー**
        * VCC -> 3.3V
        * GND -> GND
        * SDA -> A4 (or SDA)
        * SCL -> A5 (or SCL)
    * **サーボモーター**
        * Signal -> D9
        * VCC -> 5V
        * GND -> GND
    
    *(※ピン番号は実際のスケッチに合わせて修正してください)*

---

## 使い方 (Usage)

1.  このリポジトリをクローンまたはダウンロードします。
2.  `(あなたのスケッチファイル名).ino` をArduino IDEで開きます。
3.  Arduinoボードにスケッチを書き込みます。
4.  シリアルモニタを開き、ボーレートを `9600` に設定します。
5.  (シリアルモニタの指示に従って指紋を登録・認証する、などの操作方法をここに書きます)

---

## 参考・謝辞 (Credits)

このプロジェクトの指紋認証センサーのコードの一部は、Waveshareの提供する以下のデモコードを参考にしています。

* **UART Fingerprint Sensor (F) Wiki:** [https://www.waveshare.com/wiki/UART_Fingerprint_Sensor_(F)](https://www.waveshare.com/wiki/UART_Fingerprint_Sensor_(F))

---

## ライセンス (License)

このプロジェクトは [MIT License](LICENSE) のもとで公開されています。

*(ライセンスは任意ですが、設定しておくと他の人が使いやすくなります。MITライセンスはシンプルで分かりやすいのでおすすめです)*
