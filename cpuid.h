#ifndef CPUID_H
#define CPUID_H

// see
// https://stackoverflow.com/a/4823889/3059809

#include <string>

#ifdef _WIN32
#include <limits.h>
#include <intrin.h>
typedef unsigned __int32  uint32_t;

#else
#include <stdint.h>
#endif

class CPUID {
    uint32_t regs[4];

public:
    explicit CPUID(unsigned i)
    {
#ifdef _WIN32
        __cpuid((int*)regs, (int)i);

#else
        asm volatile
            ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
                : "a" (i), "c" (0));
        // ECX is set to zero for CPUID function 4
#endif
    }

    const uint32_t& EAX() const { return regs[0]; }
    const uint32_t& EBX() const { return regs[1]; }
    const uint32_t& ECX() const { return regs[2]; }
    const uint32_t& EDX() const { return regs[3]; }
};

inline std::string cpu_vendor()
{
    CPUID cpuID(0); // Get CPU vendor
    std::string vendor;
    vendor.append((const char*)&cpuID.EBX(), 4);
    vendor.append((const char*)&cpuID.EDX(), 4);
    vendor.append((const char*)&cpuID.ECX(), 4);
    return vendor;
}

inline std::string cpu_brand()
{
    CPUID cpuID1(0x80000002);
    CPUID cpuID2(0x80000003);
    CPUID cpuID3(0x80000004);
    std::string vendor;
    vendor.append((const char*)&cpuID1.EAX(), 4 * 4);
    vendor.append((const char*)&cpuID2.EAX(), 4 * 4);
    vendor.append((const char*)&cpuID3.EAX(), 4 * 4);
    return vendor;
}

#endif // CPUID_H
