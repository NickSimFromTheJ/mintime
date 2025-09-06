#include "cpuinfo.hpp"
#include <memory.h>

#ifdef _MSC_VER
#include <intrin.h>
/**
 * @brief A wrapper for the __cpuid intrinsic function.
 *
 * This function retrieves information about the CPU using the CPUID
 * instruction.
 *
 * @param cpuInfo An array of four integers to store the CPU information.
 * @param function_id The ID of the function to be executed by the CPUID
 * instruction.
 */
void cpuid(int cpuInfo[4], int function_id) { __cpuid(cpuInfo, function_id); }
/**
 * @brief A wrapper for the __cpuidex intrinsic function.
 *
 * This function retrieves information about the CPU using the CPUID instruction
 * with a specified subleaf.
 *
 * @param cpuInfo An array of four integers to store the CPU information.
 * @param function_id The ID of the function to be executed by the CPUID
 * instruction.
 * @param subleaf The subleaf to be executed by the CPUID instruction.
 */
void cpuidex(int cpuInfo[4], int function_id, int subleaf) {
    __cpuidex(cpuInfo, function_id, subleaf);
}
#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
/**
 * @brief A wrapper for the __cpuid intrinsic function.
 *
 * This function retrieves information about the CPU using the CPUID
 * instruction.
 *
 * @param cpuInfo An array of four integers to store the CPU information.
 * @param function_id The ID of the function to be executed by the CPUID
 * instruction.
 */
void cpuid(int cpuInfo[4], int function_id) {
    __cpuid(function_id, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
}
/**
 * @brief A wrapper for the __cpuid_count intrinsic function.
 *
 * This function retrieves information about the CPU using the CPUID instruction
 * with a specified subleaf.
 *
 * @param cpuInfo An array of four integers to store the CPU information.
 * @param function_id The ID of the function to be executed by the CPUID
 * instruction.
 * @param subleaf The subleaf to be executed by the CPUID instruction.
 */
void cpuidex(int cpuInfo[4], int function_id, int subleaf) {
    __cpuid_count(function_id, subleaf, cpuInfo[0], cpuInfo[1], cpuInfo[2],
                  cpuInfo[3]);
}
#else
#error "Unsupported compiler"
#endif

/**
 * @brief Retrieves information about the CPU.
 *
 * This function parses the CPUID registers to get the model, vendor, family,
 * and brand of the CPU. It uses either <cpuid.h> (on Linux) or <intrin.h> (on
 * Windows) to get the CPU info.
 *
 * @return A CPUInfo struct containing the CPU's vendor, family, model, and
 * brand.
 */
CPUInfo get_cpu_info() {
    CPUInfo info;
    int regs[4] = {0};

    // Vendor
    cpuid(regs, 0);
    char vendor[13];
    memcpy(vendor + 0, &regs[1], 4); // EBX
    memcpy(vendor + 4, &regs[3], 4); // EDX
    memcpy(vendor + 8, &regs[2], 4); // ECX
    vendor[12] = '\0';
    info.vendor = vendor;

    cpuid(regs, 1);
    unsigned eax = regs[0];
    unsigned base_family = (eax >> 8) & 0xF;
    unsigned base_model = (eax >> 4) & 0xF;
    unsigned ext_family = (eax >> 20) & 0xFF;
    unsigned ext_model = (eax >> 16) & 0xF;

    info.family = (base_family == 0xF) ? base_family + ext_family : base_family;
    info.model = (ext_model << 4) | base_model;

    // Brand string
    char brand[0x40] = {};
    cpuid(regs, 0x80000000);
    if ((unsigned)regs[0] >= 0x80000004) {
        int *brand_int = (int *)brand;
        cpuid(&brand_int[0], 0x80000002);
        cpuid(&brand_int[4], 0x80000003);
        cpuid(&brand_int[8], 0x80000004);
        info.brand = std::string(brand);
    }

    return info;
}

/**
 * @brief Creates a machine profile based on the CPU information.
 *
 * This function analyzes the CPU information to determine if it has slow or
 * fast subnormals and other performance characteristics. Note: Most of the
 * extra parameters are not currently being used.
 *
 * @param ci A const reference to a CPUInfo struct.
 * @return A MachineProfile struct containing the machine's performance
 * characteristics.
 */
MachineProfile profile_from_cpu(const CPUInfo &ci) {
    MachineProfile p{};
    if (ci.vendor == "GenuineIntel") {
        p.has_slow_subnormals = true;
        p.prefer_int_chain = true;
        p.lea_is_costly = true;
        p.issue_width = 4;
        p.branch_mispredict_expensive = true;
        p.prefer_independent_ops = true;
    } else if (ci.vendor == "AuthenticAMD") {
        p.has_slow_subnormals = false;
        p.prefer_int_chain = true;
        p.lea_is_costly = false;
        p.issue_width = 4;
        p.branch_mispredict_expensive = true;
        p.prefer_independent_ops = false;
    } else {
        p.has_slow_subnormals = false;
        p.prefer_int_chain = true;
        p.lea_is_costly = false;
        p.issue_width = 3;
        p.branch_mispredict_expensive = true;
        p.prefer_independent_ops = true;
    }
    return p;
}
