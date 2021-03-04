#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "random.h"
#include "conditions.h"
#include "pauli_simulator.h"
#include "circuit.h"
#include "decoder.h"

void update_progress(int count) {
    for (int i = 0; i < 50; i++) {
        if (i < count * 50 / N) printf("|");
        else printf("_");
    }
    printf("%02d%%", count * 100 / N);
    printf("\r");
    fflush(stdout);
}

// 重みデータをgnuplotでプロットする関数
int output_gnuplot(int weight[2][2 * num_rounds - 1][max_size]) {
    FILE *data, *gp;
    char filename[64];

    // 重みデータをファイルに出力する
    for (int type = 0; type < 2; type++) {
        for (int round = 0; round < 2 * num_rounds - 1; round++) {
            // ラウンドごとにファイルを開く
            snprintf(filename, 64, "syndrome_%c_round%d%s.dat", 
                (type == 0) ? 'Z' : 'X', 
                round / 2,
                (round % 2 == 1) ? "half" : "");
            data = fopen(filename, "w");

            // データの本体を出力する
            for (int id = 0; id < max_size; id++) {
                fprintf(data, "%d\t%d\t%d\n", id / width, id % width, weight[type][round][id]);
            }
            fclose(data);
        }
    }
    // デフォルトのプロッタで画像を出力する
    gp = popen("gnuplot -persist 3dplot.plt", "w");
    pclose(gp);

    return 0;
}

int main(int argc, char** argv) {
    // 乱数を初期化する
    init_rand(1997);

    // 符号状態を作成する
    pauli_error_t state[max_size];
    
    // シンドローム・差分シンドローム
    int syndrome[num_rounds + 1][max_size];
    int difference_syndrome[num_rounds][max_size];

    // 反転したシンドロームのペア
    int pair[2][3];
    
    // エラーが発生した回数
    // 時間方向のペアも発生しうるのでそれを考慮した要素数になっている
    int weight[2][2 * num_rounds - 1][max_size];
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2 * num_rounds; j++) {
            for (int k = 0; k < max_size; k++) {
                weight[i][j][k] = 0;
            }
        }
    }

    // 反転したシンドローム測定の総数
    int num_flips = 0;

    // 特定されたエッジの総数
    int sum_edges = 0;

    // N回繰り返す
    for (int count = 0; count < N; count++) {
        update_progress(count);
        // エラーの総数をリセットする
        num_flips = 0;
        // 状態を初期化する
        for (int i = 0; i < max_size; i++) {
            state[i].bit = 0;
            state[i].phase = 0;
            state[i].reg = 0;
        }
        // 差分シンドローム・デコードグラフを初期化する
        for (int i = 0; i < num_rounds + 1; i++) {
            for (int j = 0; j < max_size; j++) {
                syndrome[i][j] = 0;
            }
        }
        // シンドロームのペアをリセットする
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                pair[i][j] = 0;
            }
        }
        // ノイズを入れながら回路をシミュレーションする
        for (int round = 0; round < num_rounds; round++) {
            update_pauli_state(state, X_circuit, X_depth);
            if (round < num_rounds - 1) {
                update_pauli_state(state, Z_circuit, Z_depth);
            }
            else {
                update_pauli_state(state, pseudo_Z_circuit, 1);
            }
            for (int i = 0; i < max_size; i++){
                syndrome[round + 1][i] = state[i].reg;
            }
        }
        // シンドロームを差分シンドロームに変換する
        for (int i = 0; i < num_rounds; i++) {
            for (int j = 0; j < max_size; j++){
                syndrome[i][j] = syndrome[i][j] ^ syndrome[i + 1][j];
                if (syndrome[i][j] == 1) {
                    pair[num_flips % 2][0] = i;
                    pair[num_flips % 2][1] = j / width;
                    pair[num_flips % 2][2] = j % width;
                    num_flips++;
                }
            }
        }
        // お手製デコーダ部分
        detect_errors(pair, num_flips, weight);
    }
    printf("\nEarned edge errors: %d/%d\n", sum_edges, N);
    for (int i = 0; i < 2 * num_rounds - 1; i++) {
        printf("step: %d%s\n", i / 2, ((i % 2) == 1) ? "+1/2" : "");
        for (int j = 0; j < max_size * 2; j++) {
            int id = j / (2 * width) * width + j % width;
            int type = (j / width) % 2;
            if (weight[type][i][id] != 0) {
                printf("%4d ", weight[type][i][id]);
            }
            else {
                printf("---- ");
            }
            printf("");
            if (j % width == 8) {
                printf("\t");
            }
            if (j % (2 * width) == 17) {
                printf("\n");
            }
        }
        printf("\n");
    }
    output_gnuplot(weight);

    return 0;
}