
#include "timing.hpp"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <x86intrin.h>
#include <xmmintrin.h>

static inline void fp_enable_subnormal_slowpath() {
    unsigned csr = _mm_getcsr();

    csr &= ~(1u << 15);
    csr &= ~(1u << 6);
    _mm_setcsr(csr);
}

static inline double make_subnormal() {
    volatile double x = std::numeric_limits<double>::denorm_min();
    if (std::fpclassify(x) != FP_SUBNORMAL) {
        x = std::ldexp(1.0, -1074);
    }
    return x;
}

static inline double make_normal() { return 1.0; }

uint64_t timed_add_subnormal(unsigned a, unsigned b) {
    fp_enable_subnormal_slowpath();
    const unsigned iters = (a + b + 1) * 1000000u;

    double slow = make_subnormal();
    volatile double acc = 0;

    uint64_t t0 = tsc_start();
    for (unsigned i = 0; i < iters; ++i) {
        acc = acc + slow;
    }

    uint64_t t1 = tsc_stop();

    asm volatile("" ::"x"(acc));

    return t1 - t0;
}

uint64_t timed_add_normal(unsigned a, unsigned b) {
    fp_enable_subnormal_slowpath();

    const unsigned iters = (a + b + 1) * 1000000u;

    double fast = make_normal();
    volatile double acc = 0.0;

    uint64_t t0 = tsc_start();
    for (unsigned i = 0; i < iters; ++i) {
        acc = acc + fast;
    }

    uint64_t t1 = tsc_stop();

    asm volatile("" ::"x"(acc));

    return t1 - t0;
}

uint64_t timed_mul_subnormal(unsigned a, unsigned b) {
    fp_enable_subnormal_slowpath();
    const unsigned iters = (a + b + 1) * 1000000u;

    double slow = make_subnormal();
    volatile double acc = 0;

    uint64_t t0 = tsc_start();
    for (unsigned i = 0; i < iters; ++i) {
        acc = acc * slow;
    }

    uint64_t t1 = tsc_stop();

    asm volatile("" ::"x"(acc));

    return t1 - t0;
}

uint64_t timed_mul_normal(unsigned a, unsigned b) {
    fp_enable_subnormal_slowpath();

    const unsigned iters = (a + b + 1) * 1000000u;

    double fast = make_normal();
    volatile double acc = 0.0;

    uint64_t t0 = tsc_start();
    for (unsigned i = 0; i < iters; ++i) {
        acc = acc * fast;
    }

    uint64_t t1 = tsc_stop();

    asm volatile("" ::"x"(acc));

    return t1 - t0;
}

uint64_t timed_sub_normal(unsigned a, unsigned b) {
    volatile float x = static_cast<float>(a);
    volatile float y = static_cast<float>(b);
    volatile float res;

    uint64_t t0 = tsc_start();
    for (int i = 0; i < 1000000; i++) {
        res = x - y;
        asm volatile("" ::"x"(res));
    }
    uint64_t t1 = tsc_stop();

    return t1 - t0;
}

uint64_t timed_sub_subnormal(unsigned a, unsigned b) {
    volatile float x = static_cast<float>(a) / 1e38f;
    volatile float y = static_cast<float>(b) / 1e38f;
    volatile float res;

    uint64_t t0 = tsc_start();
    for (int i = 0; i < 1000000; i++) {
        res = x - y;
        asm volatile("" ::"x"(res));
    }
    uint64_t t1 = tsc_stop();

    return t1 - t0;
}
