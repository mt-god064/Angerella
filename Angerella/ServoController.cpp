#include "ServoController.h"

// Servoオブジェクトの定義
// extern宣言されたオブジェクトをここで実体化します。
Servo penguin;

// サーボを初期化する関数の実装
void initServo(int pin) {
    // 既にアタッチされているかを確認する処理は通常不要ですが、
    // attach()を呼び出すことでサーボをピンに接続します。
    penguin.attach(pin); 
    // 初期位置を設定
    penguin.write(205);
    delay(1000); // 初期化のための待機
}

// サーボを動かす関数の実装
void moveServo(int degree, int delayTime) {
    penguin.write(degree);
    delay(delayTime);
}

// 施錠アクション
void lock() {
    // 施錠位置（例: 0度）
    moveServo(205, 500); 
    Serial.println("Smart Lock: Locked (0 degrees)");
}

// 開錠アクション（ご要望の90度へ動かす処理）
void unlock() {
    // 開錠位置（例: 90度）
    moveServo(90, 400); 
    Serial.println("Smart Lock: Unlocked (90 degrees)");
}
