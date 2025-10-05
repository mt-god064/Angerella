#ifndef SERVOGUARD_H
#define SERVOGUARD_H

#include <Arduino.h>
#include <Servo.h>

// グローバルなServoオブジェクトの外部宣言
// main.cppや他のファイルからこのオブジェクトを参照できるようにします。
extern Servo penguin;

/**
 * @brief サーボモーターを初期化し、指定されたピンにアタッチします。
 * * @param pin サーボの信号線が接続されているArduinoのピン番号 (PWMピン推奨)。
 */
void initServo(int pin);

/**
 * @brief サーボモーターを指定された角度に動かし、指定時間待機します。
 * * @param degree サーボを動かす角度 (0〜180度)。
 * @param delayTime 角度に達するまで待機する時間 (ミリ秒)。
 */
void moveServo(int degree, int delayTime);

/**
 * @brief スマートロックの「施錠」動作（例として0度）を実行します。
 */
void lock();

/**
 * @brief スマートロックの「開錠」動作（例として90度）を実行します。
 * 元の指示に従い、90度へ動かす関数として定義します。
 */
void unlock();

#endif // SERVOGUARD_H
