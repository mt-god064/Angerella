// motion.h
#ifndef MOTION_H
#define MOTION_H

#include <Arduino.h>

// 外部公開する関数
void motion_setup();

// 変数をreset
void reset_motionstate();

// 監視状態が警告レベルにあるかどうかをチェックする関数
// EXECUTING状態から呼び出されます。
bool is_motion_warning();

#endif // MOTION_H