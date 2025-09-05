#ifndef STALL_ADD_H_
#define STALL_ADD_H_

#include <cstdint>

uint64_t timed_add_subnormal(unsigned a, unsigned b);
uint64_t timed_add_normal(unsigned a, unsigned b);
uint64_t timed_mul_subnormal(unsigned a, unsigned b);
uint64_t timed_mul_normal(unsigned a, unsigned b);
uint64_t timed_sub_normal(unsigned a, unsigned b);
uint64_t timed_sub_subnormal(unsigned a, unsigned b);

#endif // STALL_ADD_H_
