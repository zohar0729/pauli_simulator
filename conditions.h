/*
    conditions.h
    計算条件を管理するためのヘッダーファイル。
    全ソースコードで共有することになるのでここに変数は絶対に書き込まない
*/

#ifndef DEF_MAIN_H
#define DEF_MAIN_H

// 設定する実験定数
const int N = 1E6;
const int d = 5;
const int num_rounds = 5;
const int width = 2 * d - 1;
const int max_size = width * width;

// 各操作における誤り発生確率
const double coef = 5.0E-2;
const double EP_Preparation = coef * 0.0015;
const double EP_Identity    = coef * 0.0015;
const double EP_Hadamard    = coef * 0.0015;
const double EP_CNot        = coef * 0.0036;
const double EP_Measurement = coef * 0.0380;

#else

// 設定する実験定数
extern const int N;
extern const int d;
extern const int num_rounds;
extern const int width;
extern const int max_size;

extern const double coef;
extern const double EP_Preparation;
extern const double EP_Identity;
extern const double EP_Hadamard;
extern const double EP_CNot;
extern const double EP_Measurement;

#endif