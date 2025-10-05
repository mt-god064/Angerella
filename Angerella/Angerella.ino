#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Motion.h" //振動検知モジュールのヘッダをインクルード
//#include "sample01.h"
#include "ServoController.h"
#include "finger.h"
#include "AuthSystem_rhythm.h"
#include "voice.h"

const int buttonPin = 2;   // ボタンピン
const int ledPin = 13;     // LEDピン
const int SERVO_PIN = 6;


// FSMの状態を定義
enum State {
  WAITING,
  EXECUTING,
  LONG_PRESSING,
  AUTH_WAITING_RTYTHM,
  AUTH_WAITING_FINGER
};

// enum M_State {
//   fail_authentication
// };

State currentState = WAITING;

// ボタンの状態管理用変数
int lastButtonState = LOW;
unsigned long buttonPressTime = 0;
unsigned long buttonPressTime_lock = 0;
//const unsigned long shortPressTime = 500; // 短押しのしきい値 (0.5秒)
const unsigned long longPressTime = 1500; // 長押しのしきい値 (2秒)
//const unsigned long longPressTime2 = 5000;//長押しのs閾値(5秒)指紋とリズムの判定用(仮)
const unsigned long longPressTime_lock = 5000;
const unsigned long authTimeout = 50000;   // 認証待ちのタイムアウト時間 (50秒)
unsigned long authWaitStartTime = 0;
unsigned long pressedDuration = 0;
unsigned long pressedDuration_lock = 0;

unsigned long previousMillis = 0;
unsigned long currentMillis =0;
const long interval = 500; // 点滅周期 (500ms)
const long voiceInt = 500;
int ledState = LOW;

bool warningActive = false;

unsigned long Boice_sec = 0;

int lockState = HIGH;

// リズム認証用
unsigned int result = 0;

//ボタンの連続クリック処理用
// ボタンのクリック回数
int clickCount = 0;
// 前回のクリック（ボタンが離された瞬間）からの経過時間を測定するための変数
unsigned long lastClickTime = 0;
// ダブルクリックと見なすための最大間隔 (例: 500ミリ秒)
const unsigned long doubleClickTime = 100;

//アラート音声のレベル管理用の変数
int alert_cnt = 0;
//認証失敗回数保存
int fail_cnt = 0;
bool move_check = false;


void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  pinMode(Finger_RST_Pin  , OUTPUT);
  CMD_Init();

  Serial.begin(9600);
  Serial.println("現在の状態: WAITING");

  init_rhythm_auth();

  motion_setup();

  initMP3();

  initServo(SERVO_PIN);

  lock();
}

void loop() {
  int currentButtonState = digitalRead(buttonPin);

//----------------------------------------------------
//------------ONでもOFFでも行うものかここ--------------






//----------------------------------------------------


  switch (currentState) {
//-------------------------------------------
//------------待機状態（OFF状態）-------------
    case WAITING:
      digitalWrite(ledPin, LOW);
      // if (currentButtonState == LOW && lastButtonState == HIGH) {
      //   currentState = EXECUTING; 
      //   delay(50);//チャタリング対策
      //   Serial.println("実行に！");
      // }

      if (currentButtonState == HIGH && lastButtonState == LOW) {
        // 実行中にボタンが押されたら長押し判定を開始
        buttonPressTime_lock = millis();
        delay(50);//チャタリング対策
      }

      if (currentButtonState == LOW && lastButtonState == HIGH) { // ボタンが離された瞬間
        pressedDuration_lock = millis() - buttonPressTime_lock;
        delay(50);//チャタリング対策

        if (pressedDuration_lock > longPressTime_lock) {//今は2秒以上
          if(lockState==HIGH){
            unlock();
            lockState = LOW;
          }else{
            lock();
            lockState = HIGH;
          }
        }else{
          currentState = EXECUTING; 
          //delay(50);//チャタリング対策
          Serial.println("実行に！");
          start_voice();//システム起動音声
          lock();
        }
      }



      break;


//-------------------------------------------
//------------監視状態（ON状態）--------------
    case EXECUTING:
      digitalWrite(ledPin, HIGH);

//-----------------------------------------------------
//----------------振動検知はここに-----------------
//delayは望まない。
//millis()でloopを止めずにdelayみたいに使う方法がある（監視状態のLED点滅で使っている）
      if(fail_cnt >= 1 && !move_check) {
        currentMillis = millis();
        if (currentMillis - previousMillis >= voiceInt) {
          previousMillis = currentMillis;
          // Serial.println("u-m");
          alert_cnt = alert_voice(alert_cnt);//警告音声の再生
        }
      }

      warningActive = is_motion_warning();
      if (warningActive) {
        move_check = true;
          // 警告が有効な場合の追加の処理（ブザーなど）をここに記述
          //Serial.println("警告状態継続中...");
          //↓ここで音声流すよ！！！！！
          currentMillis = millis();
          if (currentMillis - previousMillis >= voiceInt) {
            previousMillis = currentMillis;
            // Serial.println("wa-i");
            alert_cnt = alert_voice(alert_cnt);//警告音声の再生
          }
          //alert_cnt = alert_voice(alert_cnt);//警告音声の再生
          //unsigned long currentSeconds = millis() / 1000;
          //playSoundBySecondsMP3(currentSeconds, "start");
          //delay(2000);
          //updateMP3_start();//ここで判定している今回はstartのみ
          // playSoundBySecondsMP3(Boice_sec,"fail_authentication");
          // Boice_sec = playSoundBySecondsMP3();
      }


//-----------------------------------------------------
      if (currentButtonState == HIGH && lastButtonState == LOW) {
        // ボタンが押された瞬間（立ち上がりエッジ）
        buttonPressTime = millis();
        // 前回のクリックからの時間をチェック
        if (millis() - lastClickTime < doubleClickTime) {
          // ダブルクリックの可能性がある
          clickCount++;
        } else {
          // 間隔が空いたため、クリック回数をリセットして1回目とする
          clickCount = 1;
        }
        
        // currentState = LONG_PRESSING;
        // Serial.println("長押し判定へ！"); // デバッグ用
        delay(50);//チャタリング対策
      }

      if (currentButtonState == LOW && lastButtonState == HIGH) { // ボタンが離された瞬間
        pressedDuration = millis() - buttonPressTime;
        lastClickTime = millis(); // ボタンが離された時間を記録
        
        if (pressedDuration > longPressTime) {//2秒以上でリズム認証へ
          // 5秒以上の長押し: リズム認証
          init_rhythm_auth();
          currentState = AUTH_WAITING_RTYTHM;
          authWaitStartTime = millis();
          start_auth_voice();
          Serial.println("リズム認証へ！");
        } else if (clickCount >= 2 && pressedDuration < longPressTime) { 
          // 連続クリック回数が2回以上で、かつ短押しだった場合: 指紋認証
          // (長押し判定のlongPressTime: 1500msより短いことを確認)
          currentState = AUTH_WAITING_FINGER;
          authWaitStartTime = millis();
          start_auth_voice();
          Serial.println("連続クリック 指紋認証へ！");
          clickCount = 0; // 認証に移行したのでリセット
        } else {
          // 短押し (longPressTime未満)
          // 連続クリックが未完了または1回目の短押しだった場合、EXECUTINGに戻る
          currentState = EXECUTING;
          Serial.println("短押し 実行状態へ");
          //clickCount = 0; // 短押しとして扱われたのでリセット
        }
      }
      
      break;


//-------------------------------------------
//------------長押し判定---------------------
    // case LONG_PRESSING:
    //   if (currentButtonState == LOW && lastButtonState == HIGH) { // ボタンが離された瞬間
    //     pressedDuration = millis() - buttonPressTime;

    //     if (pressedDuration > longPressTime2) {//5秒以上でリズム認証へ
    //       currentState = AUTH_WAITING_RTYTHM;
    //       authWaitStartTime = millis();
    //       Serial.println("リズム認証へ！");
    //     }else if(pressedDuration > longPressTime){//2秒以上5秒以内で指紋認証へ
    //       currentState = AUTH_WAITING_FINGER;
    //       authWaitStartTime = millis();
    //       Serial.println("指紋認証へ！");
    //     } else {
    //       currentState = EXECUTING;
    //       Serial.println("長押し足りない。待機に！");
    //       clickCount = 0;
    //     }
    //   }
    //   break;

//------------------------------------------------------------
//---------------ダブルクリック追加の長押し判定処理-------------
      case LONG_PRESSING:
      // if (currentButtonState == LOW && lastButtonState == HIGH) { // ボタンが離された瞬間
      //   pressedDuration = millis() - buttonPressTime;
      //   lastClickTime = millis(); // ボタンが離された時間を記録
        
      //   if (pressedDuration > longPressTime) {//2秒以上でリズム認証へ
      //     // 5秒以上の長押し: リズム認証
      //     init_rhythm_auth();
      //     currentState = AUTH_WAITING_RTYTHM;
      //     authWaitStartTime = millis();
      //     start_auth_voice();
      //     Serial.println("リズム認証へ！");
      //   } else if (clickCount >= 2 && pressedDuration < longPressTime) { 
      //     // 連続クリック回数が2回以上で、かつ短押しだった場合: 指紋認証
      //     // (長押し判定のlongPressTime: 1500msより短いことを確認)
      //     currentState = AUTH_WAITING_FINGER;
      //     authWaitStartTime = millis();
      //     start_auth_voice();
      //     Serial.println("連続クリック 指紋認証へ！");
      //     clickCount = 0; // 認証に移行したのでリセット
      //   } else {
      //     // 短押し (longPressTime未満)
      //     // 連続クリックが未完了または1回目の短押しだった場合、EXECUTINGに戻る
      //     currentState = EXECUTING;
      //     Serial.println("短押し 実行状態へ");
      //     //clickCount = 0; // 短押しとして扱われたのでリセット
      //   }
      // }
      break;


//------------------------------------------------------------
//------------認証状態（長押し話離したらここに来る）--------------
    case AUTH_WAITING_RTYTHM:
      // 実行状態の処理（LED点滅）
      currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        digitalWrite(ledPin, ledState);
      }

      // 認証待ちタイムアウトのチェック
      if (millis() - authWaitStartTime > authTimeout) {
        Serial.println("認証タイムアウト。実行状態に戻ります。");
        currentState = EXECUTING;
        fail_play();
        delay(1500);
        fail_cnt = fail_auth_voice(fail_cnt);
        init_rhythm_auth();
        variable_init();
      }

//-----------------------------------------------------
//----------------認証システムはここに-----------------
//認証成功したらcurrentStateをWAITINGに変更（currentState = WAITING;）

      read_mpudata();
      // 出力の表示は全部cppファイルの方で行っている
      result = check_rhythm();
      //Serial.println(result);

      if (currentButtonState == LOW && lastButtonState == HIGH) { // ボタンが離された瞬間
        delay(50);//チャタリング対策
        init_rhythm_auth();
        variable_init();
        Serial.println();
        Serial.println("認証リセット");
        retry_play();
      }

      // 認証成功
      if (result == 1) {
        suc_play();
        delay(3500);
        sucess_auth_voice();
        init_rhythm_auth();
        currentState = WAITING;
        lockState = HIGH; // サーボモータリセット
        //以下音声の回数リセット
        alert_cnt = 0;
        fail_cnt = 0;
        move_check = false;
        warningActive = false;
        reset_motionstate();
      }
      //認証失敗
      else if (result == 0) {
        currentState = EXECUTING;
        fail_play();
        delay(1500);
        fail_cnt = fail_auth_voice(fail_cnt);
        init_rhythm_auth();
        variable_init();
      }
      break;

    case AUTH_WAITING_FINGER:
      //Serial.println("wa-i");
      // 実行状態の処理（LED点滅）
      currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        digitalWrite(ledPin, ledState);
      }

      // 認証待ちタイムアウトのチェック
      if (millis() - authWaitStartTime > authTimeout) {
        Serial.println("認証タイムアウト。実行状態に戻ります。");
          currentState = EXECUTING;
        }

      //以下指紋認証処理
      // 指紋認証
        uint8_t return_num;
        //Serial.println("いざまいる");
        return_num = ScopeVerifyUser( ) ;
        //Serial.println("かえったでござる");

        // 指紋認証結果
        if(return_num == 0)
        {
          Serial.println("指紋成功!");
          currentState = WAITING;
          //認証成功音声
          suc_play();
          delay(3500);
          sucess_auth_voice();
          lockState = HIGH; // サーボモータリセット
          //以下音声の回数リセット
          alert_cnt = 0;
          fail_cnt = 0;
          move_check = false;
          warningActive = false;
          reset_motionstate();
        }
        else
        {
          Serial.println("指紋失敗!");
          currentState = EXECUTING;
          clickCount = 0;
          //認証失敗音声
          fail_play();
          delay(1500);
          fail_cnt = fail_auth_voice(fail_cnt);
        }
        break;


  }



  // ボタンの状態を更新
  lastButtonState = currentButtonState;
}












// const int buttonPin = 2;   // ボタンを接続するピン
// const int ledPin = 13;     // LEDを接続するピン

// // 状態を管理する変数（true: 実行中, false: 待機中）
// boolean isExecuting = false;

// // ボタンが押されたことを検知するための変数
// int lastButtonState = LOW;

// void setup() {
//   pinMode(buttonPin, INPUT_PULLUP); // 内部プルアップ抵抗を使用
//   pinMode(ledPin, OUTPUT);
//   // シリアルモニタで状態を確認
//   Serial.begin(9600);
// }

// void loop() {
//   // 1. ボタンの状態を読み取る
//   int currentButtonState = digitalRead(buttonPin);

//   // 2. ボタンが押された瞬間を検知（立ち下がりエッジ）
//   if (lastButtonState == HIGH && currentButtonState == LOW) {
//     // 状態を反転させる
//     isExecuting = !isExecuting;

//     if (isExecuting) {
//       Serial.println("実行状態に切り替えました。");
//     } else {
//       Serial.println("待機状態に切り替えました。");
//     }
//     // ボタンのチャタリングを防ぐための遅延
//     delay(50);
//   }

//   // 3. 状態に応じて処理を分岐させる
//   if (isExecuting) {
//     // 実行状態の処理
//     digitalWrite(ledPin, HIGH); // LEDを点灯
//   } else {
//     // 待機状態の処理
//     digitalWrite(ledPin, LOW);  // LEDを消灯
//   }

//   // 4. 次のループのために現在の状態を保存
//   lastButtonState = currentButtonState;
// }