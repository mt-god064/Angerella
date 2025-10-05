#ifndef AUTH_SYSTEM_RHYTHM_H
#define AUTH_SYSTEM_RHYTHM_H

#include <Arduino.h>

// リズム認証モジュールの初期化
void init_rhythm_auth();

// MPU6050からデータを読む
void read_mpudata();

// 縦振りのリズム入力を検出
// 戻り値: 1 = Success, 0 = Failed, -1 = Timeout
int check_rhythm();

// タイムアウト時の初期化用
void variable_init();

#endif