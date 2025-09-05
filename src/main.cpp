#include "cpuinfo.hpp"
#include "scheduler.hpp"
#include "stall_add.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

struct Measured {
    uint64_t add_slow, add_fast, mul_slow, mul_fast;
};

Measured probe(unsigned a, unsigned b) {
    auto median = [](auto &&f) {
        const int N = 9;
        uint64_t v[N];
        for (int i = 0; i < N; i++)
            v[i] = f();
        std::sort(v, v + N);
        return v[N / 2];
    };
    Measured m{};
    m.add_slow = median([&]() { return timed_add_subnormal(a, b); });
    m.add_fast = median([&]() { return timed_add_normal(a, b); });
    m.mul_slow = median([&]() { return timed_mul_subnormal(a, b); });
    m.mul_fast = median([&]() { return timed_mul_normal(a, b); });
    return m;
}

auto main(int argc, char *argv[]) -> int {

    unsigned a = 10, b = 20;
    if (argc >= 3) {
        a = std::strtoul(argv[1], nullptr, 10);
        b = std::strtoul(argv[2], nullptr, 10);
    }

    auto m = probe(50, 50);
    bool subnormals_help =
        (m.add_slow > m.add_fast * 2) || (m.mul_slow > m.mul_fast * 2);
    CPUInfo ci = get_cpu_info();
    auto prof = profile_from_cpu(ci);
    std::vector<TimedOperations> palette;

    if (subnormals_help || prof.has_slow_subnormals) {
        palette.push_back({"add_slow", [](unsigned x, unsigned y) {
                               return timed_add_subnormal(x, y);
                           }});
        palette.push_back({"mul_slow", [](unsigned x, unsigned y) {
                               return timed_mul_subnormal(x, y);
                           }});
        palette.push_back({"sub_slow", [](unsigned x, unsigned y) {
                               return timed_sub_subnormal(x, y);
                           }});
    }
    palette.push_back({"add_fast", [](unsigned x, unsigned y) {
                           return timed_add_normal(x, y);
                       }});
    palette.push_back({"mul_fast", [](unsigned x, unsigned y) {
                           return timed_mul_normal(x, y);
                       }});
    palette.push_back({"sub_fast", [](unsigned x, unsigned y) {
                           return timed_sub_normal(x, y);
                       }});

    auto plan = greedy_schedule(palette, a, b, /*steps=*/6);
    printf("Chosen seq (%zu ops): ", plan.sequence.size());
    for (auto &s : plan.sequence)
        printf("%s ", s.c_str());
    printf("\nTotal incremental cycles ~ %llu\n",
           (unsigned long long)plan.total_cycles);
    return 0;
}
