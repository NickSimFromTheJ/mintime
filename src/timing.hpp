#ifndef TIMING_H_
#define TIMING_H_

#include <cstdint>
#include <emmintrin.h>
#include <x86intrin.h>
static inline uint64_t tsc_start() {
    _mm_lfence();
    return _rdtsc();
}

static inline uint64_t tsc_stop() {
    unsigned aux;
    uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
}

#endif // TIMING_H_
