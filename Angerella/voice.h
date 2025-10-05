#ifndef VOICE_H
#define VOICE_H

// 定数 (const) の宣言
// BUSY_PIN は他のファイルから参照する可能性があるため宣言に含めます。
//const int BUSY_PIN = 12;



// 関数のプロトタイプ宣言

//setup関数
void initMP3();

// システムの起動時
unsigned long start_voice();

//認証開始時
unsigned long start_auth_voice();

//警告音声
//引数はこの音声を再生した回数
unsigned long alert_voice(int alert_cnt);

//認証失敗
//引数は失敗した回数
unsigned long fail_auth_voice(int fail_auth_cnt);

//認証成功時
unsigned long sucess_auth_voice();

//リズム再生
unsigned long rhythm_play();

//失敗再生
unsigned long fail_play();

//成功再生
unsigned long suc_play();

//リズム認証リセット再生
unsigned long retry_play();



#endif // VOICE_H