// motion.cpp
#include "Motion.h"
#include <Wire.h>
#include <Arduino.h> // millis()を使うために必要

// ---------------------- 内部変数と定数 ----------------------
const int MPU_ADDR = 0x68;
const float THRESHOLD = 10; // 加速度のしきい値 (m/s^2)

// 状態管理
enum MotionState { M_STANDBY, M_WARNING };
MotionState currentMotionState = M_STANDBY;
unsigned long warningStartTime = 0;
const unsigned long WARNING_TIMEOUT = 600000; // 警告状態が続く時間 (ミリ秒)

// 判定履歴
const int HISTORY_SIZE = 10.5;
bool motionHistory[HISTORY_SIZE];
int historyIndex = 0;

// 非ブロック化のための時間管理
unsigned long lastMotionCheckTime = 0;
const unsigned long CHECK_INTERVAL = 100; // 振動検知ロジックの実行間隔 (ミリ秒)

// ---------------------- 関数実装 ----------------------

void reset_motionstate() {
    currentMotionState = M_STANDBY;
}

// 振動検知モジュールの初期設定
void motion_setup() {
    Wire.begin();
    
    // MPU6050の初期設定
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B); // POWER_MGMT_1 レジスタ
    Wire.write(0);    // センサーをウェイクアップ
    Wire.endTransmission(true);

    // 履歴配列を初期化
    for (int i = 0; i < HISTORY_SIZE; i++) {
        motionHistory[i] = false;
    }
    // 初回実行時刻を設定
    lastMotionCheckTime = millis();
}

// 振動検知のメインロジックを実行し、警告状態を返します
bool is_motion_warning() {
    // 0. 非ブロックで実行間隔をチェック
    unsigned long currentTime = millis();
    if (currentTime - lastMotionCheckTime < CHECK_INTERVAL) {
        // まだ実行すべき時間ではない
        return (currentMotionState == M_WARNING);
    }
    // 実行時間になったので、時間を更新
    lastMotionCheckTime = currentTime;

    // 1. センサーデータの読み取り
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B); // ACCEL_XOUT_H レジスタ
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6, true);
    int16_t accelX = Wire.read() << 8 | Wire.read();
    int16_t accelY = Wire.read() << 8 | Wire.read();
    int16_t accelZ = Wire.read() << 8 | Wire.read();

    // 生の値をm/s^2に変換し、大きさを計算 (MPU6050のFS_SEL=0設定を仮定)
    float acc_x = accelX / 16384.0 * 9.8;
    float acc_y = accelY / 16384.0 * 9.8;
    float acc_z = accelZ / 16384.0 * 9.8;
    // 重力加速度 (9.8 m/s^2) を差し引く処理がないため、静止時でも約9.8 m/s^2になることに注意
    float acceleration_magnitude = sqrt(pow(acc_x, 2) + pow(acc_y, 2) + pow(acc_z, 2));

    // 2. 判定履歴の更新
    motionHistory[historyIndex] = (acceleration_magnitude > THRESHOLD);
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;

    // 3. 過去の履歴をチェックし、動きの有無を判定
    int motionCount = 0;
    for (int i = 0; i < HISTORY_SIZE; i++) {
        if (motionHistory[i]) {
            motionCount++;
        }
    }

    const int MOTION_THRESHOLD_COUNT = 8;
    bool isMoving = (motionCount >= MOTION_THRESHOLD_COUNT);

    // 4. 状態遷移ロジック
    switch (currentMotionState) {
        case M_STANDBY:
            if (isMoving) {
                // Serial.println("待機状態から警告状態に遷移します。");
                warningStartTime = currentTime; // 現在の時刻を使用
                currentMotionState = M_WARNING;
            }
            break;

        case M_WARNING:
            // 警告時間経過チェック
            if (currentTime - warningStartTime > WARNING_TIMEOUT) {
                if (!isMoving) {
                    // Serial.println("警告状態から待機状態に遷移します。");
                    currentMotionState = M_STANDBY;
                }
            }
            break;
    }

    // 5. 現在の警告状態を返す
    return (currentMotionState == M_WARNING);
}