#ifndef CPUINFO_H_
#define CPUINFO_H_

#include <string>
struct CPUInfo {
    std::string vendor;
    unsigned family = 0, model = 0;
};
struct MachineProfile {
    bool has_slow_subnormals;
    bool prefer_int_chain;
    bool lea_is_costly;
    int issue_width; // rough guidance
};

CPUInfo get_cpu_info();
MachineProfile profile_from_cpu(const CPUInfo &ci);
#endif // CPUINFO_H_
