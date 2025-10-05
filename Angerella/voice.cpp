#include <Arduino.h>
#include <DFPlayer_Mini_Mp3.h>
#include "voice.h"

const int BUSY_PIN = 12;//DFPlayerのBUSYピンの接続先
int track = 0;


//setup関数
void initMP3() {
  Serial1.begin(9600);          // UNO R4 WiFi の場合
  mp3_set_serial(Serial1);      // DFPlayer のシリアルを設定
  mp3_set_volume(23);           // 音量設定
  randomSeed(analogRead(A0));   // 乱数の種を初期化
  pinMode(BUSY_PIN, INPUT);
  //int alert_cnt = 0;
}

//ランダムにトラック選択して再生する関数
//引数は指定範囲の数字二つ 例:(1,11)この場合は1~10
static void playRamdomTrack(int start,int end){
    track = random(start, end); // 指定範囲からランダムに選択
    Serial.print(track);//再生するトラックを表示
    Serial.println(": 再生開始");
    mp3_play(track);// トラックの再生
}

//システム起動時に再生
unsigned long start_voice(){
    Serial.println("Start_System_voice");
    playRamdomTrack(1,11);
}
//認証をスタートするとき
unsigned long start_auth_voice(){
    Serial.println("Strat_Auth_voice");
    playRamdomTrack(11,16);
}

//警告の音声　引数にアラートした合計回数(合計回数は関数内で++)
//レベルごとにトラックの範囲を変更
//認証の完了時などに回数をリセットする処理を追加する必要がある
unsigned long alert_voice(int alert_cnt){
    //再生中なら終了
    if(digitalRead(BUSY_PIN) == HIGH){
        Serial.print("Alert_voice:");
        if(alert_cnt < 3){
            //レベル1
            Serial.println("level1");
            playRamdomTrack(21,26);
            alert_cnt++;
            return alert_cnt;
        }else if(alert_cnt < 6){
            //レベル2
            Serial.println("level2");
            playRamdomTrack(41,50);
            alert_cnt++;
            return alert_cnt;
        }else{
            //レベル3
            Serial.println("level3");
            playRamdomTrack(62,71);
            return alert_cnt;
        }
    }else{
        return alert_cnt;
    }
}

//認証失敗時の音声　失敗した合計回数(合計回数は関数内で足し算している)
//レベルごとにトラックを変更
//認証の完了時などに回数をリセットする処理を追加する必要がある
unsigned long fail_auth_voice(int fail_auth_cnt){
    if(digitalRead(BUSY_PIN) == LOW){
        return fail_auth_cnt;//ボイスが再生中なら終了
    }else{
        Serial.print("Fail_Auth_voice:");
        if(fail_auth_cnt == 0){//初めての認証失敗
            Serial.println("1");
            playRamdomTrack(31,38);
            return ++fail_auth_cnt;//失敗回数を+1
        }else if(fail_auth_cnt == 1){//二回目の認証失敗
            Serial.println("2");
            playRamdomTrack(51,55);
            return ++fail_auth_cnt;//失敗回数を+1
        }else{
            Serial.println("3");//失敗回数が3回以上
            playRamdomTrack(71,73);
            return fail_auth_cnt;//失敗回数
        }

    }

}

//認証が成功したとき
unsigned long sucess_auth_voice(){
    Serial.println("Sucess_Auth_voice");
    playRamdomTrack(81,91);
}


//リズム感知で鳴らすやつ
unsigned long rhythm_play(){
    mp3_play(100);
}
unsigned long fail_play(){
    mp3_play(101);
}
unsigned long suc_play(){
    mp3_play(102);
}
unsigned long retry_play() {
    mp3_play(103);
}