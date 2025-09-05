#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <cstdint>
#include <functional>
#include <string>

#include <cstdint>
struct TimedOperations {
    std::string name;
    std::function<uint64_t(unsigned, unsigned)> run;
};

struct GreedyPlan {
    std::vector<std::string> sequence;
    uint64_t total_cycles = 0;
};

GreedyPlan greedy_schedule(const std::vector<TimedOperations> &palette,
                           unsigned a, unsigned b, int steps);

#endif // SCHEDULER_H_
