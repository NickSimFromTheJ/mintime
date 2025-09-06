#ifndef STALL_ADD_H_
#define STALL_ADD_H_

#include <cstdint>
#include <stddef.h>

uint64_t timed_add_subnormal(unsigned a, unsigned b);
uint64_t timed_add_normal(unsigned a, unsigned b);
uint64_t timed_mul_subnormal(unsigned a, unsigned b);
uint64_t timed_mul_normal(unsigned a, unsigned b);
uint64_t timed_sub_normal(unsigned a, unsigned b);
uint64_t timed_sub_subnormal(unsigned a, unsigned b);
uint64_t timed_branch_taken(int iters);
uint64_t timed_branch_random(int iters);
uint64_t timed_branch_not_taken(int iters);
uint64_t timed_mem_seq(size_t N);
uint64_t timed_mem_random(size_t N);

#endif // STALL_ADD_H_
