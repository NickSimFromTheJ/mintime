#include "scheduler.hpp"
#include "timing.hpp"
#include <cstdint>
#include <limits>
#include <vector>

/**
 * @brief Measures the incremental time taken by a candidate function when added to a prefix function.
 * 
 * This function calculates the time taken to execute a prefix function, and then the time taken to execute both the prefix and a candidate function.
 * The difference between these two times is returned as the incremental time.
 * 
 * @param prefix A function that represents the operations already scheduled.
 * @param candidate A function that represents the new operation to be scheduled.
 * @return The incremental time taken by the candidate function.
 */
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

/**
 * @brief Creates a schedule of operations using a greedy algorithm.
 * 
 * This function selects the operation that adds the minimum incremental time at each step.
 * 
 * @param palette A vector of TimedOperations to choose from.
 * @param a An unsigned integer used as input for the timed operations.
 * @param b An unsigned integer used as input for the timed operations.
 * @param steps The number of steps in the schedule.
 * @return A GreedyPlan struct containing the sequence of operations and the total cycles.
 */
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