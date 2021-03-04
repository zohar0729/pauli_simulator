#ifndef DEF_DECODER_H
#define DEF_DECODER_H

#include "conditions.h"

int convert_to_difference_syndrome(
    int syndrome[num_rounds + 1][max_size],
    int difference_syndrome[num_rounds][max_size],
    int pair[2][3]
);
int add_difference_syndrome(
    int difference_syndrome[num_rounds][max_size],
    int total_syndrome[num_rounds][max_size]
);
int detect_errors(int pair[2][3], int num_flips, int count[2][2 * num_rounds - 1][max_size]);
void convert_count_to_prob(
    int count[2][2 * num_rounds - 1][max_size],
    double prob[2][2 * num_rounds - 1][max_size]
);

#endif