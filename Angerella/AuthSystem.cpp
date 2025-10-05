#include <Arduino.h>
#include <Wire.h>

// プロトタイプ宣言
extern void auth_setup();
extern void auth_loop();

const int MPU_addr = 0x68;  // MPU-6050のI2Cアドレス
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

// --- 認証ステップの状態を定義 ---
enum AuthState {
  STEP_RESET,       // 認証開始前（初期状態）
  STEP_1_COMPLETE,  // ステップ1（縦振り）完了
  STEP_2_COMPLETE   // ステップ2（横振り）完了
};

AuthState authentication_state = STEP_RESET;

// --- 共通の定数 ---
const int SHAKE_TIMEOUT = 1000;            // 連続動作のタイムアウト (1000ms)
unsigned long lastLoopTime = 0;
const unsigned long loopInterval = 300;  // 300msごとに処理したい

// --- 縦振り (Vertical Shake) 検知用の定数/変数 ---
const int VERTICAL_SHAKE_THRESHOLD = 32000; 
const int VERTICAL_SHAKE_TARGET = 5;       // 目標回数
int vertical_shake_count = 0;               
long last_vertical_shake_time = 0;          

// --- 横振り (Horizontal Shake) 検知用の定数/変数 ---
const int HORIZONTAL_SHAKE_THRESHOLD = 32000; 
const int HORIZONTAL_SHAKE_TARGET = 5;     // 目標回数
int horizontal_shake_count = 0;             
long last_horizontal_shake_time = 0;        

// --- 円運動 (Circle Gesture) 検知用の定数/変数 ---
const long CIRCLE_SUM_THRESHOLD = 20000; 
const int CIRCLE_TARGET = 5;                // 目標回数
int circle_count = 0;                       
long last_circle_time = 0;                  
float GyZ_current_sum = 0.0;                


void auth_setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  Serial.println("System Ready. Please perform the gesture sequence:");
  Serial.print("STEP 1: Vertical Shake x");
  Serial.println(VERTICAL_SHAKE_TARGET);
}

// MPU-6050からデータを読み取る関数
void read_mpu_data() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}

// 認証失敗時に呼び出され、状態をリセットする関数
void authentication_failed() {
  Serial.println("==================================");
  Serial.println("❌ AUTHENTICATION FAILED: Incorrect gesture or sequence.");
  Serial.println("==================================");
  authentication_state = STEP_RESET;
  vertical_shake_count = 0;
  horizontal_shake_count = 0;
  circle_count = 0;
  GyZ_current_sum = 0.0;
  Serial.print("Please start again: STEP 1: Vertical Shake x");
  Serial.println(VERTICAL_SHAKE_TARGET);
}


// --- 個別ジェスチャー検知ロジック ---

// 縦振り検知のコアロジック
bool check_vertical_shake(int target_count) {
  long current_time = millis();
  
  // タイムアウト処理
  if (current_time - last_vertical_shake_time > SHAKE_TIMEOUT) {
    if (vertical_shake_count > 0) {
      Serial.print("Vertical timeout. Count ");
      Serial.print(vertical_shake_count);
      Serial.println(" Reset");
    }
    vertical_shake_count = 0;
  }

  // 縦振り検知
  if (abs(AcZ) > VERTICAL_SHAKE_THRESHOLD) {
    if (current_time - last_vertical_shake_time > 100) {
      vertical_shake_count++;
      last_vertical_shake_time = current_time;
      
      // デバッグ出力: 検知
      Serial.print("Vertical Shake detected: ");
      Serial.print(vertical_shake_count);
      Serial.println(" times.");
      
      if (vertical_shake_count == target_count) {
        vertical_shake_count = 0; 
        return true;
      }
    }
  }
  return false;
}

// 横振り検知のコアロジック
bool check_horizontal_shake(int target_count) {
  long current_time = millis();
  
  // タイムアウト処理
  if (current_time - last_horizontal_shake_time > SHAKE_TIMEOUT) {
    if (horizontal_shake_count > 0) {
      Serial.print("Horizontal timeout. Count ");
      Serial.print(horizontal_shake_count);
      Serial.println(" Reset");
    }
    horizontal_shake_count = 0;
  }

  // 横振り検知
  if (abs(AcX) > HORIZONTAL_SHAKE_THRESHOLD || abs(AcY) > HORIZONTAL_SHAKE_THRESHOLD) {
    if (current_time - last_horizontal_shake_time > 100) {
      horizontal_shake_count++;
      last_horizontal_shake_time = current_time;
      
      // デバッグ出力: 検知
      Serial.print("Horizontal Shake detected: ");
      Serial.print(horizontal_shake_count);
      Serial.println(" times.");

      if (horizontal_shake_count == target_count) {
        horizontal_shake_count = 0; 
        return true;
      }
    }
  }
  return false;
}

// 円運動検知のコアロジック
bool check_circle_count(int target_count) {
  long current_time = millis();
  
  // タイムアウト処理
  if (current_time - last_circle_time > SHAKE_TIMEOUT) {
    if (circle_count > 0) {
      Serial.print("Circle timeout. Count ");
      Serial.print(circle_count);
      Serial.println(" Reset");
    }
    circle_count = 0;
    GyZ_current_sum = 0.0;
  }

  GyZ_current_sum += GyZ;

  // 円運動検知
  if (GyZ_current_sum < -CIRCLE_SUM_THRESHOLD) {
    if (current_time - last_circle_time > 500) {
      circle_count++;
      last_circle_time = current_time;
      
      // デバッグ出力: 検知
      Serial.print("Clockwise Circle detected: ");
      Serial.print(circle_count);
      Serial.println(" times.");
      
      if (circle_count == target_count) {
        circle_count = 0; 
        GyZ_current_sum = 0.0;
        return true;
      }
      GyZ_current_sum = 0.0;
    }
  } else if (GyZ_current_sum > 0) {
    GyZ_current_sum = 0.0;
  }
  return false;
}

// --- ステップごとの実行関数 ---

void run_step_1_vertical() {
  // ① 縦振り目標達成: ステップ成功
  if (check_vertical_shake(VERTICAL_SHAKE_TARGET)) {
    Serial.println("✅ STEP 1 COMPLETE: Vertical Shake.");
    Serial.print("Now perform STEP 2: Horizontal Shake x");
    Serial.println(HORIZONTAL_SHAKE_TARGET);
    authentication_state = STEP_1_COMPLETE;
    return;
  }
  
  // ② 横振り不正検知: ステップ失敗
  if (check_horizontal_shake(HORIZONTAL_SHAKE_TARGET)) {
    authentication_failed();
    return;
  }
  
  // ③ 円運動不正検知: ステップ失敗
  if (check_circle_count(CIRCLE_TARGET)) {
    authentication_failed();
    return;
  }
}

void run_step_2_horizontal() {
  // ① 横振り目標達成: ステップ成功
  if (check_horizontal_shake(HORIZONTAL_SHAKE_TARGET)) {
    Serial.println("✅ STEP 2 COMPLETE: Horizontal Shake.");
    Serial.print("Now perform STEP 3: Clockwise Circle x");
    Serial.println(CIRCLE_TARGET);
    authentication_state = STEP_2_COMPLETE;
    return;
  }
  
  // ② 縦振り不正検知: ステップ失敗
  if (check_vertical_shake(VERTICAL_SHAKE_TARGET)) {
    authentication_failed();
    return;
  }
  
  // ③ 円運動不正検知: ステップ失敗
  if (check_circle_count(CIRCLE_TARGET)) {
    authentication_failed();
    return;
  }
}

void run_step_3_circle() {
  // ① 円運動目標達成: 最終認証成功
  if (check_circle_count(CIRCLE_TARGET)) {
    Serial.println("==================================");
    Serial.println("✅ AUTHENTICATION SUCCESS: All steps completed.");
    Serial.println("==================================");
    authentication_state = STEP_RESET; // 認証完了、初期状態に戻す
    return;
  }
  
  // ② 縦振り不正検知: ステップ失敗
  if (check_vertical_shake(VERTICAL_SHAKE_TARGET)) {
    authentication_failed();
    return;
  }
  
  // ③ 横振り不正検知: ステップ失敗
  if (check_horizontal_shake(HORIZONTAL_SHAKE_TARGET)) {
    authentication_failed();
    return;
  }
}


void auth_loop() {
  unsigned long currentMillis = millis();
  
  // 前回の処理からloopIntervalミリ秒以上経過していたら実行
  if (currentMillis - lastLoopTime >= loopInterval) {
    lastLoopTime = currentMillis;

    read_mpu_data(); // センサーからデータを取得

    // 認証状態に基づき、対応するステップの実行関数を呼び出す
    if (authentication_state == STEP_RESET) {
      run_step_1_vertical(); 
    } else if (authentication_state == STEP_1_COMPLETE) {
      run_step_2_horizontal(); 
    } else if (authentication_state == STEP_2_COMPLETE) {
      run_step_3_circle(); 
    }
  }
}
