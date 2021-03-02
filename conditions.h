/*
    conditions.h
    計算条件を管理するためのヘッダーファイル。
    全ソースコードで共有することになるのでここに変数は絶対に書き込まない
*/

#ifndef DEF_MAIN_H
#define DEF_MAIN_H

// 設定する実験定数
const unsigned int N = 1E6;
const unsigned int d = 5;
const unsigned int num_rounds = 5;
const unsigned int width = 2 * d - 1;
const unsigned int max_size = width * width;

// 各操作における誤り発生確率
const double coef = 5.0E-2;
const double EP_Preparation = coef * 0.0015;
const double EP_Identity    = coef * 0.0015;
const double EP_Hadamard    = coef * 0.0015;
const double EP_CNot        = coef * 0.0036;
const double EP_Measurement = coef * 0.0380;

#else

// 設定する実験定数
extern const unsigned int N;
extern const unsigned int d;
extern const unsigned int num_rounds;
extern const unsigned int width;
extern const unsigned int max_size;

extern const double coef;
extern const double EP_Preparation;
extern const double EP_Identity;
extern const double EP_Hadamard;
extern const double EP_CNot;
extern const double EP_Measurement;

#endif