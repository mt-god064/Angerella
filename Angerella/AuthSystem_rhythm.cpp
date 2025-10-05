#include <Arduino.h>
#include <Wire.h>
#include "voice.h"

const int addr = 0x68;  // MPU-6050のI2Cアドレス
int16_t Ac_X,Ac_Y,Ac_Z,Tmp_1,Gy_X,Gy_Y,Gy_Z;

// --- 共通の定数 ---
const int VERTICAL_SHAKE_THRESHOLD = 32000;  // 縦振り判定のしきい値
long last_vertical_shaketime = 0; // 最後に振った時刻

// --- リズム認証用 ---
// 正解のリズムパターン（振り間隔ms）
const int PATTERN_SIZE = 11; //リズムを刻む回数
unsigned long correct_pattern[PATTERN_SIZE] = {
  0, 360, 210, 350, 220, 355, 210, 335, 220, 530, 530
}; //正解のリズム間隔
const int TIME_TOLERANCE = 80;  // 許容誤差 ±80ms

// ユーザ入力された時刻の記録
unsigned long detected_times[PATTERN_SIZE];
int detected_index = 0; //何回振ったかのカウント用
const int tolerance  = 2; // Xを何回許すか
int count = 0; // Xを何回許すかカウント

// =======================================================
// I2C バスクリア関数
// =======================================================
void I2C_ClearBus() {
  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);
  delay(250);

  // SCLがLowならクロックを送って開放する
  pinMode(SCL, OUTPUT);
  for (int i = 0; i < 16; i++) {
    digitalWrite(SCL, LOW);
    delayMicroseconds(20);
    digitalWrite(SCL, HIGH);
    delayMicroseconds(20);
  }

  // SDAも開放
  pinMode(SDA, INPUT_PULLUP);
  delay(250);
}


void init_rhythm_auth() {
  I2C_ClearBus();
  Wire.begin();

  // MPU6050初期化シーケンス
  Wire.beginTransmission(addr);
  Wire.write(0x6B); // PWR_MGMT_1
  Wire.write(0x00); // スリープ解除
  Wire.endTransmission(true);
  delay(100);

  // 加速度範囲設定 ±2g
  Wire.beginTransmission(addr);
  Wire.write(0x1C); // ACCEL_CONFIG
  Wire.write(0x00);
  Wire.endTransmission(true);

  // ジャイロ範囲設定 ±250deg/s
  Wire.beginTransmission(addr);
  Wire.write(0x1B); // GYRO_CONFIG
  Wire.write(0x00);
  Wire.endTransmission(true);

  delay(100);
}

// --- MPU6050からデータを読む ---
void read_mpudata() {
  Wire.beginTransmission(addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(addr,14,true);  // request a total of 14 registers
  Ac_X=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  Ac_Y=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  Ac_Z=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp_1=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  Gy_X=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  Gy_Y=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  Gy_Z=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

}

void variable_init() {
  detected_index = 0;
}

// --- 縦振りのリズム入力を検出 ---
int check_rhythm() {
  long current_time = millis();

  // 縦方向の加速度がしきい値を超えたら「振った」と判定
  if (Ac_X > VERTICAL_SHAKE_THRESHOLD) {

    if (detected_index == 0) {
    for (int i = 1; i <= PATTERN_SIZE; i++) {
      Serial.print(i);
      if (i < PATTERN_SIZE) Serial.print(" -> ");
    }
    Serial.println();
  }
  
    // 最後に振ってから200msは次の振りを認めない
    if (current_time - last_vertical_shaketime > 100) { //デバウンス
      last_vertical_shaketime = current_time;

      // 検出した時刻を保存
      if (detected_index < PATTERN_SIZE) {
        detected_times[detected_index] = current_time;

        // 振れたことを示すために♪を出力
        if (detected_index < 9) Serial.print("♪");
        else Serial.print("♪ "); // 2桁のときはスペースを1ついれる
        rhythm_play();//振ったら音が鳴るよ
        detected_index++;
      }

      // 必要な回数分振ったら判定（N+1回の時刻でN個の間隔）
      if (detected_index == PATTERN_SIZE) {
        bool success = true;
        Serial.println();
        
        for (int i = 0; i < PATTERN_SIZE; i++) {
          unsigned long interval;
          if (i == 0) {
            interval = 0; // 1回目は0基準
          } else {
            interval = detected_times[i] - detected_times[i - 1]; // 差分
          }

          if (abs((long)interval - (long)correct_pattern[i]) > TIME_TOLERANCE) {
            //認証用のboolをfalseに変更
            if ( count >= tolerance) success = false;
            count++;
            if (i < 9) Serial.print("X");
            else Serial.print("X "); // 2桁のときはスペースを1ついれる
          } 
          else {
            if (i < 9) Serial.print("O");
            else Serial.print("O "); // 2桁のときはスペースを1ついれる
          }
          if (i < PATTERN_SIZE - 1) Serial.print(" -> ");
        }
        
        if (success) {
          Serial.println();
          Serial.println("==================================");
          Serial.println("✅ Rhythm Authentication Success!");
          Serial.println("==================================");
        } else {
          Serial.println();
          Serial.println("==================================");
          Serial.println("❌ Rhythm Authentication Failed!");
          Serial.println("==================================");
        }
        
        //リズム結果を表示
        /*if (!success){
          Serial.println("--- Result Rhythm ---");
          for (int i = 0; i < PATTERN_SIZE; i++) {
            unsigned long interval;
            if (i == 0) {
              interval = 0; // 1回目は0基準
            } else {
              interval = detected_times[i] - detected_times[i - 1]; // 差分
            }
            Serial.print("Interval ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(interval);
          }
          Serial.println();

          for (int i = 1; i <= PATTERN_SIZE; i++) {
            Serial.print(i);
            if (i < PATTERN_SIZE) Serial.print(" -> ");
          }
          Serial.println();
        }*/
        

        // 次の入力に備えてリセット
        variable_init();
        count = 0;

        // bool値を戻す
        return success ? 1 : 0; //判定済み

      } else {
        Serial.print(" -> ");
      }
    }
  }
}