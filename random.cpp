#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t xorsft_x = 123456789;
uint32_t xorsft_y = 362436069;
uint32_t xorsft_z = 521288629;
uint32_t xorsft_w = 88675123;

#define XORSHIFT
#ifdef XORSHIFT

void init_rand(uint32_t seed){
    do {
        seed = seed * 1812433253 + 1; seed ^= seed << 13; seed ^= seed >> 17;
        xorsft_x = 123464980 ^ seed;
        seed = seed * 1812433253 + 1; seed ^= seed << 13; seed ^= seed >> 17;
        xorsft_y = 3447902351 ^ seed;
        seed = seed * 1812433253 + 1; seed ^= seed << 13; seed ^= seed >> 17;
        xorsft_z = 2859490775 ^ seed;
        seed = seed * 1812433253 + 1; seed ^= seed << 13; seed ^= seed >> 17;
        xorsft_w = 47621719 ^ seed;
    } while(xorsft_x == 0 && xorsft_y == 0 && xorsft_z == 0 && xorsft_w == 0);
}

double urand(){
    uint32_t t;
    t = xorsft_x ^ (xorsft_x << 11);
    xorsft_x = xorsft_y;
    xorsft_y = xorsft_z;
    xorsft_z = xorsft_w;
    xorsft_w ^= t ^ (t >> 8) ^ (xorsft_w >> 19);
    return ((xorsft_x + 0.5) / 4294967296.0 + xorsft_w) / 4294967296.0;
}

void set_params(uint32_t cxorsft_x, uint32_t cxorsft_y, uint32_t cxorsft_z, uint32_t cxorsft_w) {
    xorsft_x = cxorsft_x;
    xorsft_y = cxorsft_y;
    xorsft_z = cxorsft_z;
    xorsft_w = cxorsft_w;
}

void get_params() {
    printf("%u, %u, %u, %u\t\n", xorsft_x, xorsft_y, xorsft_z, xorsft_w);
    fflush(stdout);
}

#else

void initrand(uint32_t seed){
    srand(seed);
}

double urand(){
    return ((double)rand() + 1.0) / ((double)RAND_MAX + 2.0);
}

#endif