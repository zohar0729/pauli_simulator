#include <stdio.h>
#include <vector>

// 設定する実験定数
const unsigned int N = 10000;
const unsigned int d = 5;
const unsigned int num_rounds = 5;

// 各操作における誤り発生確率
const double EP_Preparation = 0.001;
const double EP_Identity = 0.001;
const double EP_Hadamard = 0.001;
const double EP_CNOT = 0.001;
const double EP_Measurement = 0.01;

// シンドローム測定に使う回路図
const char measurement_circuit[][(2 * d - 1) * (2 * d - 1) + 1] = {
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
const char X_measurement_circuit[][(2 * d - 1) * (2 * d - 1) + 1] = {
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
};

const char Z_measurement_circuit[][(2 * d - 1) * (2 * d - 1) + 1] = {
    //
    "----D----"
    "--ZDZD---"
    "--D-D-D--"
    "ZDZDZDZD-"
    "D-D-D-D-D"
    "-DZDZDZDZ"
    "--D-D-D--"
    "---DZDZ--"
    "----D----",
    // 測定量子ビットの初期化
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
    "----D----"
    "--ZDZD---"
    "--D-D-D--"
    "ZDZDZDZD-"
    "D-D-D-D-D"
    "-DZDZDZDZ"
    "--D-D-D--"
    "---DZDZ--"
    "----D----",
};

// パウリ演算子の固有状態としての量子状態を記述するクラス
class State {
    public:
    State(unsigned int d);
    // 状態を|0>に初期化する
    int initialize();

    private:
    // 擬似的な量子ビットの構造体
    struct qubit {
        int bit, phase;
    };
    // 保持している情報の実体
    std::vector<qubit> qubits;
};

// ノイズのある量子ゲートを記述するクラス
class NoisyGate {
    public :
    NoisyGate(unsigned int index);
    virtual void update_quantum_state(State target) = 0;
};

// ノイズのある量子回路を記述するクラス
class NoisyCircuit {
    public:
    NoisyCircuit(unsigned int d);
    void update_quantum_state(State target);
};

// 測定を行うクラス
class Measurement : public NoisyGate {

};

int main(int argc, char** argv) {
    // 回路を準備する

    // N回繰り返す
        // ノイズを入れながら回路をシミュレーションする
        // 発生したエラーの総数を数える
            // エラーの総数が1個なら、発生した位置を記録する
            // それ以外の場合は何も記録を行わない
    // エラーの発生回数からデコードグラフの重みを計算する
}
