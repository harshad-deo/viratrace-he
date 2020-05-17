#include <benchmark/benchmark.h>

#ifndef VTHE_BENCH_UTILS
#define VTHE_BENCH_UTILS

static const int MultiplyDeBruijnBitPosition2[32] = {0,  1,  28, 2,  29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4,  8,
                                                     31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6,  11, 5,  10, 9};

int log2Int(const int arg) { return MultiplyDeBruijnBitPosition2[(uint32_t)(arg * 0x077CB531U) >> 27]; }

#endif