#include "cpuinfo.hpp"
#include <cpuid.h>

CPUInfo get_cpu_info() {
    CPUInfo info;
    unsigned eax, ebx, ecx, edx;

    __get_cpuid(0, &eax, &ebx, &ecx, &edx);
    char v[13];
    *(unsigned *)&v[0] = ebx;
    *(unsigned *)&v[4] = edx;
    *(unsigned *)&v[8] = ecx;
    v[12] = 0;
    info.vendor = v;

    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    unsigned base_family = (eax >> 8) & 0xF;
    unsigned base_model = (eax >> 4) & 0xF;
    unsigned ext_family = (eax >> 20) & 0xFF;
    unsigned ext_model = (eax >> 16) & 0xF;
    info.family = (base_family == 0xF) ? base_family + ext_family : base_family;
    info.model = (ext_model << 4) | base_model;
    return info;
}

MachineProfile profile_from_cpu(const CPUInfo &ci) {
    MachineProfile p{};
    if (ci.vendor == "GenuineIntel") {
        p.has_slow_subnormals = true;
        p.prefer_int_chain = true;
        p.lea_is_costly = true;
        p.issue_width = 4;
    } else if (ci.vendor == "AuthenticAMD") {
        p.has_slow_subnormals = false;
        p.prefer_int_chain = true;
        p.lea_is_costly = false;
        p.issue_width = 4;
    } else {
        p.has_slow_subnormals = false;
        p.prefer_int_chain = true;
        p.lea_is_costly = false;
        p.issue_width = 3;
    }
    return p;
}
