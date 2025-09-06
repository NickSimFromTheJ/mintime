#include "cpuinfo.hpp"
#include "csv_logger.hpp"
#include "operations.hpp"
#include "scheduler.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>

struct Measured {
    uint64_t add_slow, add_fast, mul_slow, mul_fast;
};

/**
 * @brief Probes the system to measure the performance of different arithmetic
 * operations.
 *
 * This function calculates the median time taken for slow and fast addition and
 * multiplication.
 *
 * @param a An unsigned integer used as input for the timed operations.
 * @param b An unsigned integer used as input for the timed operations.
 * @return A Measured struct containing the median times for the four
 * operations.
 */
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

/**
 * @brief The main entry point of the program.
 *
 * This function parses command-line arguments, profiles the CPU, and runs a
 * series of timed operations. The results of the timed operations are logged to
 * a CSV file and printed to the console.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return An integer representing the exit status of the program.
 */
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
    palette.push_back({"branch_taken", [](unsigned, unsigned) {
                           return timed_branch_taken(1000000);
                       }});
    palette.push_back({"branch_random", [](unsigned, unsigned) {
                           return timed_branch_random(1000000);
                       }});
    palette.push_back({"branch_not_taken", [](unsigned, unsigned) {
                           return timed_branch_not_taken(1000000);
                       }});
    palette.push_back(
        {"mem_seq", [](unsigned, unsigned) { return timed_mem_seq(1000000); }});

    palette.push_back({"mem_random", [](unsigned, unsigned) {
                           return timed_mem_random(1000000);
                       }});

    //    auto plan = greedy_schedule(palette, a, b, /*steps=*/6);

    CSVLogger logger("results.csv");
    for (auto &operation : palette) {
        uint64_t cycles = operation.run(a, b);
        logger.log(operation.name, (a + b + 1) * 1000000, cycles);
        std::cout << operation.name << " cycles=" << cycles << "\n";
    }

    return 0;
}
