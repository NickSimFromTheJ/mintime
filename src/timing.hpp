#ifndef TIMING_H_
#define TIMING_H_

#include <cstdint>
#include <emmintrin.h>
#include <x86intrin.h>

/*
** tsc_start() -> uint64_t
**
** The _mm_lfence function makes the function forced to wait until all memoery
*and instructions are completed
** The _rdtsc function function returnes the processor timestamp.
**
*/
static inline uint64_t tsc_start() {
    _mm_lfence();
    return _rdtsc();
}

/*
** tsc_stop0 () -> uint64_t
**
** The _mm_lfence function makes the function forced to wait until all memoery
*and instructions are completed
** The _rdtsc function function returnes the processor timestamp.
**
*/
static inline uint64_t tsc_stop() {
    unsigned aux;
    uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
}

#endif // TIMING_H_
