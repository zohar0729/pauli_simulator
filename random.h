#ifndef DEF_RANDOM_H
#define DEF_RANDOM_H

#include <stdint.h>

void initrand(uint32_t seed);
double urand();
void set_params(uint32_t cxorsft_x, uint32_t cxorsft_y, uint32_t cxorsft_z, uint32_t cxorsft_w);
void get_params();

#endif