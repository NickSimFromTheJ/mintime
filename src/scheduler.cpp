

#include "scheduler.hpp"
#include "timing.hpp"
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

static inline uint64_t
measure_increment(const std::function<void()> &prefix,
                  const std::function<uint64_t()> &candidate) {
    (void)candidate();

    uint64_t t0 = tsc_start();
    prefix();
    uint64_t t1 = tsc_stop();
    uint64_t base = t1 - t0;

    t0 = tsc_start();
    prefix();
    uint64_t _ = candidate();
    t1 = tsc_stop();

    uint64_t combined = (t1 - t0);

    return combined > base ? combined - base : 0;
}

GreedyPlan greedy_schedule(const std::vector<TimedOperations> &palette,
                           unsigned a, unsigned b, int steps) {
    GreedyPlan plan;

    std::vector<std::function<void()>> chosen;

    for (int k = 0; k < steps; ++k) {
        uint64_t best_inc = std::numeric_limits<uint64_t>::max();
        int best_idx = -1;

        auto prefix = [&chosen]() {
            for (auto &f : chosen)
                f();
        };

        for (int i = 0; i < (int)palette.size(); ++i) {
            auto cand = [&]() -> uint64_t { return palette[i].run(a, b); };

            uint64_t inc = measure_increment(prefix, cand);
            if (inc < best_inc) {
                best_inc = inc;
                best_idx = i;
            }

            plan.sequence.push_back(palette[best_idx].name);
            plan.total_cycles += best_inc;
            chosen.push_back(
                [&, idx = best_idx]() { (void)palette[idx].run(a, b); });
        }
    }
    return plan;
}
