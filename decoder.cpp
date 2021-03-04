/*
    decoder.cpp
    得られたシンドロームから差分シンドロームを生成し、差分シンドロームからエラーを特定する処理を与える。
*/

#include <stdio.h>

#include "conditions.h"

// シンドロームを差分シンドロームに変換する
int convert_to_difference_syndrome(
    int syndrome[num_rounds + 1][max_size],
    int difference_syndrome[num_rounds][max_size],
    int pair[2][3]
) {
    int num_flips = 0;
    for (int i = 0; i < num_rounds; i++) {
        for (int j = 0; j < max_size; j++){
            difference_syndrome[i][j] = syndrome[i][j] ^ syndrome[i + 1][j];
            if (difference_syndrome[i][j] == 1) {
                pair[num_flips % 2][0] = i;
                pair[num_flips % 2][1] = j / width;
                pair[num_flips % 2][2] = j % width;
                num_flips++;
            }
        }
    }
    return num_flips;
}

// 差分シンドロームを同サイズの配列に足す
void add_difference_syndrome(
    int difference_syndrome[num_rounds][max_size],
    int total_syndrome[num_rounds][max_size]
) {
    for (int i = 0; i < num_rounds; i++) {
        for (int j = 0; j < max_size; j++) {
            total_syndrome[i][j] += difference_syndrome[i][j];
        }
    }
    return;
}

// 隣接しているかどうかの判定も煩雑なので関数化
// 空間方向の隣接を判定
int is_nearby_pos(int id1, int id2) {
    if      (id1 - id2 == 2)            return 1;
    else if (id1 - id2 == -2)           return 1;
    else if (id1 - id2 == width * 2)    return 1;
    else if (id1 - id2 == width * (-2)) return 1;
    else return 0; 
}
// 時間方向の隣接を判定
int is_nearby_step(int step1, int step2) {
    if      (step1 - step2 ==  1) return 1;
    else if (step1 - step2 == -1) return 1;
    else return 0;
}

// 反転したシンドローム測定のペア、反転の総数からエラーの位置を特定する
int detect_errors(int pair[2][3], int num_flips, int count[2][2 * num_rounds - 1][max_size]) {
    // 反転したシンドローム測定が1個以上2個以下ならエッジへの対応づけを行う
    if (num_flips > 0 && num_flips < 3) {
        if (num_flips == 1) {
            int step = pair[0][0];
            int id = pair[0][1] * width + pair[0][2];
            int type = pair[0][2] % 2;
            // 左上半分の開境界に接したZエラーの場合
            if (pair[0][1] + pair[0][2] < 8 && type == 1) {
                count[type][2 * step][id - 1]++;
                return 1;
            }
            // 右下半分の開境界に接したZエラーの場合
            else if (pair[0][1] + pair[0][2] > 8 && type == 1) {
                count[type][2 * step][id + 1]++;
                return 1;
            }
            // 右上半分の開境界に接したXエラーの場合
            else if (pair[0][1] < pair[0][2] && type == 0) {
                count[type][2 * step][id - width]++;
                return 1;
            }
            // 左下半分の開境界に接したXエラーの場合
            else if (pair[0][1] > pair[0][2] && type == 0) {
                count[type][2 * step][id + width]++;
                return 1;
            }
            // それ以外の場合……え、それ以外って何？
            else {
                printf("Something cursed happend!!!");
                return 0;
            }
        }
        else {
            // 添字で間違えて修正はしたくないので変数化
            int step1 = pair[0][0];
            int step2 = pair[1][0];
            int id1 = pair[0][1] * width + pair[0][2];
            int id2 = pair[1][1] * width + pair[1][2];
            int type = pair[0][2] % 2;  // 偶数の列がZシンドローム、奇数の列がXシンドローム

            // 隣接した位置にあるシンドローム反転について具体的に考えられるパターンは3つ
            // 同一のステップで隣接した空間位置にある(データ量子ビットのエラー)
            if (step1 == step2 && is_nearby_pos(id1, id2)) {
                count[type][step1 * 2][(id1 + id2) / 2]++;
                return 1;
            }
            // 隣接したステップで同一の空間位置にある（測定エラー）
            else if (is_nearby_step(step1, step2) && id1 == id2) {
                count[type][step1 + step2][id1]++;
                return 1;
            }
            // 隣接したステップで隣接した空間位置にある（フックエラー）
            else if (is_nearby_step(step1, step2) && is_nearby_pos(id1, id2)) {
                if ((step1 - step2) * (id1 - id2) > 0) {
                    count[type][step1 + step2][(id1 + id2) / 2]++;
                    return 1;
                }
                else {
                    return 0;
                }
            }
            // それ以外の場合（別々の位置に発生した開境界エラーとか）
            else {
                return 0;
            }
        }
    }
    // 反転したシンドローム測定が0個または3個以上ならエラーの位置特定を行わない
    else {
        return 0;
    }
    // ここに来ることは無いはずなのでエラーを返す
    return -1;
}

void convert_count_to_prob(
    int count[2][2 * num_rounds - 1][max_size],
    double prob[2][2 * num_rounds - 1][max_size]
) {
    for (int type = 0; type < 2; type++) {
        for (int round = 0; round < 2 * num_rounds - 1; round++) {
            for (int id = 0; id < max_size; id++) {
                prob[type][round][id] = (double)(count[type][round][id]) / N * 1E3;
            }
        }
    }
}