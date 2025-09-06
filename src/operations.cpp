
#include "timing.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <random>
#include <vector>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#include <xmmintrin.h>

/*
**
**
** fp_enable_subnormal_slowpath()
**
** disables
**
**
*/
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

static inline double make_normal() { return 1.0; };

uint64_t timed_add_subnormal(unsigned a, unsigned b) {
    fp_enable_subnormal_slowpath();
    const unsigned iters = (a + b + 1) * 1000000u;

    double slow = make_subnormal();
    volatile double acc = 1.1;

    uint64_t t0 = tsc_start();
    for (unsigned i = 0; i < iters; ++i) {
        acc = acc + slow;
    }

    uint64_t t1 = tsc_stop();

#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("" ::"x"(acc));
#endif

    return t1 - t0;
}

uint64_t timed_add_normal(unsigned a, unsigned b) {
    fp_enable_subnormal_slowpath();

    const unsigned iters = (a + b + 1) * 1000000u;

    volatile double fast = make_normal();
    volatile double acc = 1.1;

    uint64_t t0 = tsc_start();
    for (unsigned i = 0; i < iters; ++i) {
        acc = acc + fast;
        if ((i & 0xFFFFF) == 0) {
#ifdef _MSC_VER
            _ReadWriteBarrier();
#else
            asm volatile("" ::"x"(acc));
#endif
        }
    }

    uint64_t t1 = tsc_stop();

#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("" ::"x"(acc));
#endif

    return t1 - t0;
}

uint64_t timed_mul_subnormal(unsigned a, unsigned b) {
    fp_enable_subnormal_slowpath();
    const unsigned iters = (a + b + 1) * 1000000u;

    double slow = make_subnormal();
    volatile double acc = 1.1;

    uint64_t t0 = tsc_start();
    for (unsigned i = 0; i < iters; ++i) {
        acc = acc * slow;
        if ((i & 0x3FF) == 0) {
#ifdef _MSC_VER
            _ReadWriteBarrier();
#else
            asm volatile("" ::"x"(acc));
#endif
        }
    }

    uint64_t t1 = tsc_stop();

#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("" ::"x"(acc));
#endif

    return t1 - t0;
}

uint64_t timed_mul_normal(unsigned a, unsigned b) {
    fp_enable_subnormal_slowpath();

    const unsigned iters = (a + b + 1) * 1000000u;

    volatile double fast = 1.0;
    volatile double acc = 1.1;

    uint64_t t0 = tsc_start();
    for (unsigned i = 0; i < iters; ++i) {
        acc = acc * fast;
        if ((i & 0xFFFFF) == 0) {
            acc = 1.0;
        }
    }

    uint64_t t1 = tsc_stop();

#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("" ::"x"(acc));
#endif

    return t1 - t0;
}

uint64_t timed_sub_normal(unsigned a, unsigned b) {
    volatile float x = static_cast<float>(a);
    volatile float y = static_cast<float>(b);
    volatile float res;

    uint64_t t0 = tsc_start();
    for (int i = 0; i < 1000000; i++) {
        res = x - y;
#ifdef _MSC_VER
        _ReadWriteBarrier();
#else
        asm volatile("" ::"x"(res));
#endif
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
#ifdef _MSC_VER
        _ReadWriteBarrier();
#else
        asm volatile("" ::"x"(res));
#endif
    }
    uint64_t t1 = tsc_stop();

#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("" ::"r"(x));
#endif
    return t1 - t0;
}

uint64_t timed_branch_taken(int iters) {
    volatile int x = 0;
    uint64_t t0 = tsc_start();

    for (int i = 0; i < iters; i++) {
        if (i >= 0) {
            x++;
        }
    }
    uint64_t t1 = tsc_stop();
#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("" ::"r"(x));
#endif
    return t1 - t0;
}

uint64_t timed_branch_random(int iters) {
    volatile int x = 0;
    std::mt19937 rng(1234456);
    std::uniform_int_distribution<int> dist(0, 1);

    uint64_t t0 = tsc_start();
    for (int i = 0; i < iters; i++) {
        if (dist(rng)) {
            x++;
        }
    }
    uint64_t t1 = tsc_stop();
#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("" ::"r"(x));
#endif
    return t1 - t0;
}

uint64_t timed_branch_not_taken(int iters) {
    volatile int x = 0;
    std::mt19937 rng(1234456);
    std::uniform_int_distribution<int> dist(0, 1);

    uint64_t t0 = tsc_start();
    for (int i = 0; i < iters; i++) {
        if (i < 0) {
            x++;
        }
    }
    uint64_t t1 = tsc_stop();
#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("" ::"r"(x));
#endif
    return t1 - t0;
}

uint64_t timed_mem_seq(size_t N) {
    std::vector<int> data(N, 1);
    volatile int sum = 0;

    uint64_t t0 = tsc_start();
    for (size_t i = 0; i < N; i++) {
        sum += data[i];
    }

    uint64_t t1 = tsc_stop();
#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("" ::"r"(sum));
#endif
    return t1 - t0;
}

uint64_t timed_mem_random(size_t N) {

    std::vector<int> data(N, 1);
    std::vector<size_t> idx(N);
    for (size_t i = 0; i < N; i++)
        idx[i] = i;

    std::shuffle(idx.begin(), idx.end(), std::mt19937(1234));

    volatile int sum = 0;
    uint64_t t0 = tsc_start();
    for (size_t i = 0; i < N; i++) {
        sum += data[idx[i]];
    }

    uint64_t t1 = tsc_stop();
#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("" ::"r"(sum));
#endif
    return t1 - t0;
}
