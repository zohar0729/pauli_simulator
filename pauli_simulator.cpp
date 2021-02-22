#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "random.h"

#define REAL_EP

// 設定する実験定数
const unsigned int N = 1E6;
const unsigned int d = 5;
const unsigned int num_rounds = 5;
const unsigned int width = 2 * d - 1;
const unsigned int max_size = width * width;

// 各操作における誤り発生確率
double coef = 5.0E-2;
const double EP_Preparation = coef * 0.0015;
const double EP_Identity    = coef * 0.0015;
const double EP_Hadamard    = coef * 0.0015;
const double EP_CNot        = coef * 0.0036;
const double EP_Measurement = coef * 0.038;

// シンドローム測定に使う回路図
const char measurement_circuit[][max_size + 1] = {
    "----DX---"
    "--ZDZD---"
    "--DXDXDX-"
    "ZDZDZDZD-"
    "DXDXDXDXD"
    "-DZDZDZDZ"
    "-XDXDXD--"
    "---DZDZ--"
    "---XD----"
};

// 測定回路に使用する文字の意味
// 0: |0>に初期化
// H: アダマール変換
// I: 恒等変換
// T,L,R,B: CNOT演算のコントロール部（上、左、右、下に接続）
// D: CNOT演算のターゲット部
// M: 測定
// X,Y,Z: 各種パウリ演算

// B -> L -> T -> R
const char circuit[][max_size + 1] = {
    // 測定量子ビットの初期化
    "----I0---"
    "---I-I---"
    "--I0I0I0-"
    "-I-I-I-I-"
    "I0I0I0I0I"
    "-I-I-I-I-"
    "-0I0I0I--"
    "---I-I---"
    "---0I----",
    // アダマール変換
    "----IH---"
    "---I-I---"
    "--IHIHIH-"
    "-I-I-I-I-"
    "IHIHIHIHI"
    "-I-I-I-I-"
    "-HIHIHI--"
    "---I-I---"
    "---HI----",
    // CNOT1段目
    "----IB---"
    "---I-D---"
    "--IBIBIB-"
    "-I-D-D-D-"
    "IBIBIBIBI"
    "-D-D-D-D-"
    "-IIBIBI--"
    "---D-D---"
    "---II----",
    // CNOT2段目
    "----DL---"
    "---I-I---"
    "--DLDLDL-"
    "-I-I-I-I-"
    "DLDLDLDLI"
    "-I-I-I-I-"
    "-IDLDLI--"
    "---I-I---"
    "---II----",
    // CNOT3段目
    "----II---"
    "---D-D---"
    "--ITITII-"
    "-D-D-D-D-"
    "ITITITITI"
    "-D-D-D-I-"
    "-TITITI--"
    "---D-I---"
    "---TI----",
    // CNOT4段目
    "----DI---"
    "---I-I---"
    "--IRDRDI-"
    "-I-I-I-I-"
    "IRDRDRDRD"
    "-I-I-I-I-"
    "-RDRDRD--"
    "---I-I---"
    "---RD----",
    // アダマール変換
    "----IH---"
    "---I-I---"
    "--IHIHIH-"
    "-I-I-I-I-"
    "IHIHIHIHI"
    "-I-I-I-I-"
    "-HIHIHI--"
    "---I-I---"
    "---HI----",
    // 測定
    "----IM---"
    "---I-I---"
    "--IMIMIM-"
    "-I-I-I-I-"
    "IMIMIMIMI"
    "-I-I-I-I-"
    "-MIMIMI--"
    "---I-I---"
    "---MI----",
    //初期化
    "----I----"
    "--0I0I---"
    "--I-I-I--"
    "0I0I0I0I-"
    "I-I-I-I-I"
    "-I0I0I0I0"
    "--I-I-I--"
    "---I0I0--"
    "----I----",
    // CNOT1段目
    "----I----"
    "--IRDI---"
    "--I-I-I--"
    "IRDRDRDI-"
    "I-I-I-I-I"
    "-RDRDRDRD"
    "--I-I-I--"
    "---RDRD--"
    "----I----",
    // CNOT2段目
    "----B----"
    "--IIDI---"
    "--B-B-B--"
    "IIDIDIDI-"
    "I-B-B-B-B"
    "-IDIDIDID"
    "--I-B-B--"
    "---IDID--"
    "----I----",
    // CNOT3段目
    "----I----"
    "--DLDL---"
    "--I-I-I--"
    "DLDLDLDL-"
    "I-I-I-I-I"
    "-IDLDLDLI"
    "--I-I-I--"
    "---IDLI--"
    "----I----",
    // CNOT4段目
    "----I----"
    "--DIDI---"
    "--T-T-I--"
    "DIDIDIDI-"
    "T-T-T-T-I"
    "-IDIDIDII"
    "--T-T-T--"
    "---IDII--"
    "----T----",
    // 測定
    "----I----"
    "--MIMI---"
    "--I-I-I--"
    "MIMIMIMI-"
    "I-I-I-I-I"
    "-IMIMIMIM"
    "--I-I-I--"
    "---IMIM--"
    "----I----"
};

typedef struct pauli_error {
    int bit, phase, old_bit;
    int reg;
}pauli_error_t;

// 各種パウリ演算
// エラーとして発生するので関数にまとめておく
void Xgate(int id, pauli_error_t state[max_size]) {
    state[id].bit ^= 1;
    return;
}
void Ygate(int id, pauli_error_t state[max_size]) {
    state[id].bit ^= 1;
    state[id].phase ^= 1;
    return;
}
void Zgate(int id, pauli_error_t state[max_size]) {
    state[id].phase ^= 1;
    return;
}
void Igate(int id, pauli_error_t state[max_size]) {
    return;
}

// 1量子ビットエラーを記述する関数
void one_qubit_noise(
    int id, 
    pauli_error_t state[max_size], 
    void (*ops[])(int, pauli_error_t*), 
    const double probs[],
    int num_ops
) {
    double r = urand();
    double s = 0.0;
    int i;

    for (i = 0; i < num_ops; i++) {
        s += probs[i];
        if(r < s) break;
    }
    (*ops[i])(id, state);
    return;
}

// 2量子ビットエラーを記述する関数
// 作ってはみたけど結局使わないかな
void two_qubit_noise(
    int id1, int id2, 
    pauli_error_t state[max_size],
    void (*ops[])(int, int, pauli_error_t*), 
    const double probs[],
    int num_ops
) {
    double r = urand();
    double s = 0.0;
    int i, j;

    for (i = 0; i < num_ops; i++) {
        s += probs[i];
        if(r < s) break;
    }
    (*ops[i])(id1, id2, state);

    return;
}
// 1量子ビットのdepolarizing channel
void depolarizing_noise(int id, pauli_error_t state[max_size], double error_prob) {
    void (*ops[])(int, pauli_error_t*) = {
        &Igate, 
        &Xgate, 
        &Ygate, 
        &Zgate
    };
    double probs[] = {
        1.0 - (3.0 * error_prob), 
        error_prob, 
        error_prob, 
        error_prob
    };

    one_qubit_noise(id, state, ops, probs, 4);
    return;
}
void readout_noise(int id, pauli_error_t state[max_size], double error_prob) {
    void (*ops[])(int, pauli_error_t*) = {
        &Igate, 
        &Xgate
    };
    double probs[] = {
        1.0 - error_prob,
        error_prob
    };

    one_qubit_noise(id, state, ops, probs, 2);
    return;
}
// 2量子ビットのdepolarizing channel
// これ1量子ビットのものを別々に作用させるのと何が違うんだろう……？
void two_qubit_depolarizing_noise(int id1, int id2, pauli_error_t state[max_size], double error_prob) {
    void (*ops[])(int, pauli_error_t*) = {
        &Igate, 
        &Xgate, 
        &Ygate, 
        &Zgate
    };
    double probs[] = {
        1.0 - (3.0 * error_prob), 
        error_prob, 
        error_prob, 
        error_prob
    };

    one_qubit_noise(id1, state, ops, probs, 4);
    one_qubit_noise(id2, state, ops, probs, 4);
    return;
}

// パウリエラーの時間発展を調べる
int update_pauli_state(pauli_error_t state[max_size], int m_error) {
    int depth = (int)(sizeof(circuit) / sizeof(circuit[0]));
    int num_errors = 0;

    for (int step = 0; step < depth; step++) {
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < width; j++) {
                int id = i * width + j;
                int tmp;
                char c = circuit[step][id];
                switch(c) {
                // 状態を|0>に初期化する
                case '0':
                    depolarizing_noise(id, state, EP_Preparation);
                    break;
                // 何もしない
                case 'I':
                    depolarizing_noise(id, state, EP_Identity);
                    break;
                // Xエラーを意図的に発生させる
                case 'X':
                    Xgate(id, state);
                    break;
                // Yエラーを意図的に発生させる
                case 'Y':
                    Ygate(id, state);
                    break;
                // Zエラーを意図的に発生させる
                case 'Z':
                    Zgate(id, state);
                    break;
                // Z基底（ビット）とX基底（位相）を入れ替える
                case 'H':
                    tmp = state[id].bit;
                    state[id].bit = state[id].phase;
                    state[id].phase = tmp;
                    depolarizing_noise(id, state, EP_Hadamard);
                    break;
                // 上方向にCNOTゲートを作用させる
                case 'T':
                    if (i > 0 && circuit[step][id - width] == 'D') {
                        state[id].phase         ^= state[id - width].phase;
                        state[id - width].bit   ^= state[id].bit;
                        two_qubit_depolarizing_noise(id, id - width, state, EP_CNot);
                    }
                    else {
                        printf("Invalid index number!!(%d, %d, %d)\n", step, i, j);
                        return -1;
                    }
                    break;
                // 左方向にCNOTゲートを作用させる
                case 'L':
                    if (j > 0 && circuit[step][id - 1] == 'D') {
                        state[id].phase     ^= state[id - 1].phase;
                        state[id - 1].bit   ^= state[id].bit;
                        two_qubit_depolarizing_noise(id, id - 1, state, EP_CNot);
                    }
                    else {
                        printf("Invalid index number!!(%d, %d, %d)\n", step, i, j);
                        return -1;
                    }
                    break;
                // 右方向にCNOTゲートを作用させる
                case 'R':
                    if (j < width - 1 && circuit[step][id + 1] == 'D') {
                        state[id].phase     ^= state[id + 1].phase;
                        state[id + 1].bit   ^= state[id].bit;
                        two_qubit_depolarizing_noise(id, id + 1, state, EP_CNot);
                    }
                    else {
                        printf("Invalid index number!!(%d, %d, %d)\n", step, i, j);
                        return -1;
                    }
                    break;
                // 下方向にCNOTゲートを作用させる
                case 'B':
                    if (i < width - 1 && circuit[step][id + width] == 'D') {
                        state[id].phase         ^= state[id + width].phase;
                        state[id + width].bit   ^= state[id].bit;
                        two_qubit_depolarizing_noise(id, id + width, state, EP_CNot);
                    }
                    else {
                        printf("Invalid index number!!(%d, %d, %d)\n", step, i, j);
                        return -1;
                    }
                    break;
                // 最終的に残ったエラーの検出を行う
                case 'M':
                    //if(m_error == 1) {
                        readout_noise(id, state, EP_Measurement);
                    //}
                    state[id].reg = state[id].bit ^ state[id].old_bit;
                    state[id].old_bit = state[id].bit;
                    break;
                default:
                    break;
                }
            }
        }
    }

    return num_errors;
}

// 隣接しているかどうかの判定も煩雑なので関数化
int is_nearby_pos(int id1, int id2) {
    if      (id1 - id2 == 2)            return 1;
    else if (id1 - id2 == -2)           return 1;
    else if (id1 - id2 == width * 2)    return 1;
    else if (id1 - id2 == width * (-2)) return 1;
    else return 0; 
}
int is_nearby_step(int step1, int step2) {
    if      (step1 - step2 ==  1) return 1;
    else if (step1 - step2 == -1) return 1;
    else return 0;
}

int main(int argc, char** argv) {
    // 乱数を初期化する
    initrand(1997);

    // 符号状態を作成する
    pauli_error_t state[max_size];
    
    // 差分シンドローム
    int syndrome[num_rounds + 1][max_size];

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
    int num_flips[N];
    for (int i = 0; i < N; i++) {
        num_flips[i] = 0;
    }

    // 特定されたエッジの総数
    int sum_edges = 0;

    // N回繰り返す
    for (int count = 0; count < N; count++) {
        for (int i = 0; i < 50; i++) {
            if (i < count * 50 / N) printf("|", count);
            else printf("_");
        }
        printf("%02d%%", count * 100 / N);
        printf("\r");
        fflush(stdout);
        // 状態を初期化する
        for (int i = 0; i < max_size; i++) {
            state[i].bit = 0;
            state[i].phase = 0;
            state[i].old_bit = 0;
            state[i].reg = 0;
        }
        // 差分シンドローム・デコードグラフを初期化する
        for (int i = 0; i < num_rounds + 1; i++) {
            for (int j = 0; j < max_size; j++) {
                syndrome[i][j] = 0;
            }
        }
        // ノイズを入れながら回路をシミュレーションする
        for (int round = 0; round < num_rounds; round++) {
            update_pauli_state(state, (round == num_rounds - 1) ? 0 : 1);
            for (int i = 0; i < max_size; i++){
                syndrome[round + 1][i] = state[i].reg;
            }
        }
        // シンドロームを差分シンドロームに変換する
        for (int i = 0; i < num_rounds; i++) {
            for (int j = 0; j < max_size; j++){
                syndrome[i][j] = syndrome[i][j] ^ syndrome[i + 1][j];
                if (syndrome[i][j] == 1) {
                    //printf("(%d, %d, %d) --- %c syndrome\n", i, j / width, j % width, (j % width % 2 == 0) ? 'Z' : 'X');
                    pair[num_flips[count] % 2][0] = i;
                    pair[num_flips[count] % 2][1] = j / width;
                    pair[num_flips[count] % 2][2] = j % width;
                    num_flips[count]++;
                }
            }
        }
        // 反転したシンドローム測定が1個以上2個以下ならエッジへの対応づけを行う
        if (num_flips[count] > 0 && num_flips[count] < 3) {
            if (num_flips[count] == 1) {
                int step = pair[0][0];
                int id = pair[0][1] * width + pair[0][2];
                // 左上半分の開境界に接したZエラーの場合
                if (pair[0][1] + pair[0][2] < 8 && pair[0][1] % 2 == 0) {
                    weight[0][2 * step][id - 1]++;
                    sum_edges++;
                }
                // 右下半分の開境界に接したZエラーの場合
                else if (pair[0][1] + pair[0][2] > 8 && pair[0][1] % 2 == 0) {
                    weight[0][2 * step][id + 1]++;
                    sum_edges++;
                }
                // 右上半分の開境界に接したXエラーの場合
                else if (pair[0][1] < pair[0][2] && pair[0][1] % 2 == 1) {
                    weight[1][2 * step][id - width]++;
                    sum_edges++;
                }
                // 左下半分の開境界に接したXエラーの場合
                else if (pair[0][1] > pair[0][2] && pair[0][1] % 2 == 1) {
                    weight[1][2 * step][id + width]++;
                    if (id + width == 8 * width + 6) {
                        printf("error!! at sample %d\n", count);
                        printf("num_flips: %d\n", num_flips[count]);
                        printf("(%d, %d, %d), ", pair[0][0], pair[0][1], pair[0][2]);
                        printf("(%d, %d, %d)", pair[1][0], pair[1][1], pair[1][2]);
                        return -1;
                    }
                    sum_edges++;
                }
                // それ以外の場合……え、それ以外って何？
                else {
                    printf("Something cursed happend!!!");
                    return -1;
                }
            }
            else {
                // 添字で間違えて修正はしたくないので変数化
                int step1 = pair[0][0];
                int step2 = pair[1][0];
                int id1 = pair[0][1] * width + pair[0][2];
                int id2 = pair[1][1] * width + pair[1][2];
                int type = pair[0][1] % 2;

                // 隣接した位置にあるシンドローム反転について具体的に考えられるパターンは3つ
                // 同一のステップで隣接した空間位置にある(データ量子ビットのエラー)
                if (step1 == step2 && is_nearby_pos(id1, id2)) {
                    weight[type][step1 * 2][(id1 + id2) / 2]++;
                    sum_edges++;
                }
                // 隣接したステップで同一の空間位置にある（測定エラー）
                else if (is_nearby_step(step1, step2) && id1 == id2) {
                    weight[type][step1 + step2][id1]++;
                    sum_edges++;
                }
                // 隣接したステップで隣接した空間位置にある（フックエラー）
                else if (is_nearby_step(step1, step2) && is_nearby_pos(id1, id2)) {
                    if ((step1 - step2) * (id1 - id2) > 0) {
                        weight[type][step1 + step2][(id1 + id2) / 2]++;
                        sum_edges++;
                    }
                }
                // それ以外の場合（別々の位置に発生した開境界エラーとか）
                else {
                    continue;
                }
            }
        }
        else {
            continue;
        }
    }
    printf("\nEarned edge errors: %d/%d\n", sum_edges, N);
    for (int i = 0; i < 2 * num_rounds - 1; i++) {
        printf("step: %d%s\n", i / 2, ((i % 2) == 1) ? "+1/2" : "");
        for (int j = 0; j < max_size; j++) {
            if (weight[1][i][j] != 0) {
                printf("%d", weight[1][i][j]);
            }
            else {
                printf(" ");
            }
            printf("\t");
            if (j % 9 == 8) {
                printf("\n\n\n");
            }
        }
        printf("\n");
    }
}
