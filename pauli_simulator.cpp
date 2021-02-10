#include <stdio.h>
#include <stdlib.h>
#include <vector>

// 設定する実験定数
const unsigned int N = 100;
const unsigned int d = 5;
const unsigned int num_rounds = 5;
const unsigned int width = 2 * d - 1;
const unsigned int max_size = (2 * d - 1) * (2 * d - 1);

// 各操作における誤り発生確率
const double EP_Preparation = 0.001;
const double EP_Identity = 0.001;
const double EP_Hadamard = 0.001;
const double EP_CNot = 0.001;
const double EP_Measurement = 0.01;

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
    int bit, phase;
}pauli_error_t;

int update_pauli_state(pauli_error_t state[max_size]) {
    int depth = (int)(sizeof(circuit) / sizeof(circuit[0]));
    int num_errors = 0;
    for (int step = 0; step < depth; step++) {
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < width; j++) {
                char c = circuit[step][i * width + j];
                int id = i * width + j;
                int tmp;
                switch(c) {
                // 状態を|0>に初期化する
                case '0':
                    state[i * width + j].bit = 0;
                    state[i * width + j].phase = 0;
                    break;
                // 何もしない
                case 'I':
                    break;
                // Xエラーを意図的に発生させる
                case 'X':
                    state[id].bit ^= 1;
                    break;
                // Yエラーを意図的に発生させる
                case 'Y':
                    state[id].bit ^= 1;
                    state[id].phase ^= 1;
                    break;
                // Zエラーを意図的に発生させる
                case 'Z':
                    state[id].phase ^= 1;
                    break;
                // Z基底（ビット）とX基底（位相）を入れ替える
                case 'H':
                    tmp = state[i * width + j].bit;
                    state[i * width + j].bit = state[i * width + j].phase;
                    state[i * width + j].phase = tmp;
                    break;
                // 上方向にCNOTゲートを作用させる
                case 'T':
                    if (i > 0 && circuit[step][id - width] == 'D') {
                        state[id].phase         ^= state[id - width].phase;
                        state[id - width].bit   ^= state[id].bit;
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
                    }
                    else {
                        printf("Invalid index number!!(%d, %d, %d)\n", step, i, j);
                        return -1;
                    }
                    break;
                // 最終的に残ったエラーの検出を行う
                case 'M':
                    if (state[id].bit == 1) {
                        printf("No.%d\tstep: %d/(%d, %d)\n", num_errors, step, i, j);
                        num_errors++;
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }

    return num_errors;
}

int main(int argc, char** argv) {
    // 符号状態を作成する
    pauli_error_t state[max_size];
    for (int i = 0; i < max_size; i++) {
      state[i].bit = 0;
      state[i].phase = 0;
    }
    //update_pauli_state(state);
    // N回繰り返す
    for (int count = 0; count < N; count++) {
        int num_errors = 0;
        // ノイズを入れながら回路をシミュレーションする
        for (int round = 0; round < num_rounds; round++) {
            update_pauli_state(state);
        }
    }
    
    // エラーの発生回数からデコードグラフの重みを計算する
}
