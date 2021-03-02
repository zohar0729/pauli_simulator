#ifndef DEF_CIRCUIT_H
#define DEF_CIRCUIT_H

#include "conditions.h"

// 測定回路に使用する文字の意味
// 0: |0>に初期化
// H: アダマール変換
// I: 恒等変換
// T,L,R,B: CNOT演算のコントロール部（上、左、右、下に接続）
// D: CNOT演算のターゲット部
// M: 測定
// X,Y,Z: 各種パウリ演算

// B -> L -> T -> R
const char X_circuit[][max_size + 1] = {
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
    "---MI----"
};
const char Z_circuit[][max_size + 1] = {
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
const char pseudo_X_circuit[][max_size + 1] {
    "----DP---"
    "---D-D---"
    "--DPDPDP-"
    "-D-D-D-D-"
    "DPDPDPDPD"
    "-D-D-D-D-"
    "-PDPDPD--"
    "---D-D---"
    "---PD----"
};
const char pseudo_Z_circuit[][max_size + 1] {
    "----D----"
    "--PDPD---"
    "--D-D-D--"
    "PDPDPDPD-"
    "D-D-D-D-D"
    "-DPDPDPDP"
    "--D-D-D--"
    "---DPDP--"
    "----D----"
};
const int X_depth = sizeof(X_circuit) / sizeof(X_circuit[0]);
const int Z_depth = sizeof(Z_circuit) / sizeof(Z_circuit[0]);

#else

const char X_circuit[][max_size + 1], Z_circuit[][max_size + 1];
const char pseudo_X_circuit[][max_size + 1], pseudo_Z_circuit[][max_size + 1];
const int X_depth, Z_depth;

#endif