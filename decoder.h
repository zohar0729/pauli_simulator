#ifndef DEF_DECODER_H
#define DEF_DECODER_H

#include "conditions.h"

int detect_errors(int pair[2][3], int num_flips, int weight[2][2 * num_rounds - 1][max_size]);

#endif