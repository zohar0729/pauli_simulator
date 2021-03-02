#include <stdio.h>

#include "pauli_simulator.h"
#include "conditions.h"
#include "random.h"

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
// 読み出し時のビット反転ノイズ
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
void update_pauli_state(
    pauli_error_t state[max_size], 
    const char circuit[][max_size + 1], 
    int depth
) {
    for (int step = 0; step < depth; step++) {
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < width; j++) {
                int id = i * width + j;
                int tmp;
                char c = circuit[step][id];
                int pseudo_syndrome;
                switch(c) {
                // 状態を|0>に初期化する
                case '0':
                    state[id].bit = 0;
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
                        return;
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
                        return;
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
                        return;
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
                        return;
                    }
                    break;
                // 最終的に残ったエラーの検出を行う
                case 'P':
                    pseudo_syndrome = 0;
                    // 上側のデータ量子ビットのエラーを参照する
                    if (i > 0 && circuit[step][id - width] == 'D') {
                        pseudo_syndrome += state[id - width].bit;
                    }
                    // 左側のデータ量子ビットのエラーを参照する
                    if (j > 0 && circuit[step][id - 1] == 'D') {
                        pseudo_syndrome += state[id - 1].bit;
                    }
                    // 右側のデータ量子ビットのエラーを参照する
                    if (j < width - 1 && circuit[step][id + 1] == 'D') {
                        pseudo_syndrome += state[id + 1].bit;
                    }
                    // 下側のデータ量子ビットのエラーを参照する
                    if (i < width - 1 && circuit[step][id + width] == 'D') {
                        pseudo_syndrome += state[id + width].bit;
                    }
                    state[id].reg = pseudo_syndrome % 2;
                    break;
                case 'M':
                    readout_noise(id, state, EP_Measurement);
                    state[id].reg = state[id].bit;
                    break;
                default:
                    break;
                }
            }
        }
    }
}
