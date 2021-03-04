#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "random.h"
#include "conditions.h"
#include "pauli_simulator.h"
#include "circuit.h"
#include "decoder.h"

// 進捗をAAで表示する
void update_progress(int count) {
    for (int i = 0; i < 50; i++) {
        if (i < count * 50 / N) printf("|");
        else printf("_");
    }
    printf("%02d%%", count * 100 / N);
    printf("\r");
    fflush(stdout);
}

// エラー発生回数をAAでプロットする関数
void output_ascii(int error_count[2][2 * num_rounds - 1][max_size]) {
    for (int i = 0; i < 2 * num_rounds - 1; i++) {
        printf("round: %d%s\n", i / 2, ((i % 2) == 1) ? "+1/2" : "");
        for (int j = 0; j < max_size * 2; j++) {
            int id = j / (2 * width) * width + j % width;
            int type = (j / width) % 2;
            if (error_count[type][i][id] != 0) {
                printf("%4d ", error_count[type][i][id]);
            }
            else {
                printf("---- ");
            }
            if (j % width == 8) {
                printf("\t");
            }
            if (j % (2 * width) == 17) {
                printf("\n");
            }
        }
        printf("\n");
    }
}

// 重みデータをgnuplotでプロットする関数
int output_gnuplot(double weight[2][2 * num_rounds - 1][max_size]) {
    FILE *data;
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
                fprintf(data, "%d\t%d\t%lf\n", id / width, id % width, weight[type][round][id]);
            }
            fclose(data);
        }
    }
    // デフォルトのプロッタで画像を出力する
    // FILE *gp;
    // gp = popen("gnuplot -persist 3dplot.plt", "w");
    // pclose(gp);

    return 0;
}

// 2次元配列を初期化することが多かったので関数化
void init_2darray(int *array, int height, int width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            array[i * width + j] = 0;
        }
    }
}
void init_2darray(double *array, int height, int width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            array[i * width + j] = 0.0;
        }
    }
}

int main(int argc, char** argv) {
    // 乱数を初期化する
    init_rand(1997);

    // 符号状態を作成する
    pauli_error_t state[max_size];
    
    // シンドローム・差分シンドローム・シンドローム反転の総数
    int syndrome[num_rounds + 1][max_size];
    int difference_syndrome[num_rounds][max_size];
    int total_syndrome[num_rounds][max_size];

    // 反転したシンドロームのペア
    int pair[2][3];
    
    // エラーが発生した回数
    // 時間方向のペアも発生しうるのでそれを考慮した要素数になっている
    int error_count[2][2 * num_rounds - 1][max_size];
    for (int i = 0; i < 2; i++) {
        init_2darray(&error_count[0][0][0], 2 * num_rounds, max_size);
        init_2darray(&error_count[1][0][0], 2 * num_rounds, max_size);
    }

    // 各エッジにおけるエラーの発生確率
    double error_prob[2][2 * num_rounds - 1][max_size];
    for (int i = 0; i < 2; i++) {
        init_2darray(&error_prob[0][0][0], 2 * num_rounds, max_size);
        init_2darray(&error_prob[1][0][0], 2 * num_rounds, max_size);
    }

    // シンドローム反転の総数を初期化する
    init_2darray(&total_syndrome[0][0], num_rounds, max_size);

    // 特定されたエッジの総数
    int sum_edges = 0;

    // N回繰り返す
    for (int count = 0; count < N; count++) {
        update_progress(count);
        // 状態を初期化する
        for (int i = 0; i < max_size; i++) {
            state[i].bit = 0;
            state[i].phase = 0;
            state[i].reg = 0;
        }
        // シンドロームを初期化する
        init_2darray(&syndrome[0][0], num_rounds + 1, max_size);
        // シンドロームのペアをリセットする
        init_2darray(&pair[0][0], 2, 3);
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
        // シンドロームを差分シンドロームに変換し、反転の総数を得る
        int num_flips = convert_to_difference_syndrome(syndrome, difference_syndrome, pair);
        add_difference_syndrome(difference_syndrome, total_syndrome);
        // お手製デコーダ部分
        sum_edges += detect_errors(pair, num_flips, error_count);
    }
    // 確率に変換
    convert_count_to_prob(error_count, error_prob);

    // シミュレーション結果の描画
    printf("\nEarned edge errors: %d/%d\n", sum_edges, N);
    output_ascii(error_count);
    output_gnuplot(error_prob);
    printf("Finish!\n");

    return 0;
}