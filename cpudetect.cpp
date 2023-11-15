// cpudetect.cpp
//

#include <windows.h>
#include <string>
#include <iostream>
#include <intrin.h>
#include <iomanip>
#include "HybridDetect.h"

//todo:rrw note frequency base and max issues
#define LEAF_FREQUENCY_INFORMATION				0x16        // Processor Frequency Information Leaf  function 0x16 only works on Skylake or newer.

//todo:rrw do we want global?
HybridDetect::PROCESSOR_INFO ProcessorInfo;

//todo:rrw comment out in production
void OutputCoreType(int coreType) {
    if (coreType == HybridDetect::INTEL_CORE) {
        std::cout << "\ncore type: Intel Core\n";
    }
    else {
        if (coreType == HybridDetect::INTEL_ATOM) {
            std::cout << "\ncore type: Intel Atom\n";
        }
    }
}

//todo:rrw comment out in production
void OutputCoreInfo(HybridDetect::LOGICAL_PROCESSOR_INFO logicalCore) {
    std::cout << "max frequency " << logicalCore.maximumFrequency << "\n";
    std::cout << "base frequency " << logicalCore.baseFrequency << "\n";
    std::cout << "avx isa " << logicalCore.AVX << "\n";
    std::cout << "avx 2 isa " << logicalCore.AVX2 << "\n";
    std::cout << "avx 512 isa " << logicalCore.AVX512 << "\n";
}


//todo:rrw comment out in production
void OutputTotalCoresOfCoreType(int coreType, int cnt) {
    if (coreType == HybridDetect::INTEL_CORE) {
        std::cout << "core type total: Intel Core " << cnt << "\n";
    }
    else {
        if (coreType == HybridDetect::INTEL_ATOM) {
            std::cout << "core type total: Intel Atom " << cnt << "\n";
        }
    }
}


//IsIntelMTLorARL
bool IsIntelMTLorARL()
{
    const int kVendorID_Intel[3] = { 0x756e6547, 0x6c65746e, 0x49656e69 }; // "GenuntelineI"
    int regs[4];
    bool bIsMTLorARL = false;
    __cpuid(regs, 0);
    bool bIsIntel =
        (kVendorID_Intel[0] == regs[1]) &&
        (kVendorID_Intel[1] == regs[2]) &&
        (kVendorID_Intel[2] == regs[3]);
    if (bIsIntel)
    {
        __cpuid(regs, 1);
        const int basicCPUID = regs[0];
        int family = (basicCPUID >> 8) & 0xf;
        int extModel = ((basicCPUID & 0xf0000) >> 12) | ((basicCPUID & 0xf0) >> 4);

        bIsMTLorARL = (family == 6);
        if (bIsMTLorARL)
        {
            switch (extModel)
            {
            case 0xAA: // MTL-P/M, ARL-Z0
            case 0xAC: // MTL-S
            case 0xB5: // ARL-U
            case 0xC5: // ARL-P
            case 0xC6: // ARL-S/HX
                break;
            default:
                bIsMTLorARL = false;
            }
        }
        std::cout << "CPUID: 0x" << std::hex << std::setw(8) << std::setfill('0') << basicCPUID << ", IsMTLorARL = " << bIsMTLorARL << std::endl << std::dec;
    }
    return bIsMTLorARL;
}

void UseHybridCPUInformation() {
    int coreIndex;
    int coreType;
    int coreTypeIndex;
    int coreTypeCount[4];
    HybridDetect::LOGICAL_PROCESSOR_INFO logicalCore;

    //todo:rrw comment out in production
    std::cout << "number of physical cores " << ProcessorInfo.numPhysicalCores << "\n";
    coreIndex = 0;
    coreTypeIndex = 0;
    coreTypeCount[0] = 0;
    coreTypeCount[1] = 0;
    coreTypeCount[2] = 0;
    coreTypeCount[3] = 0;
    //todo:rrw comment out in production
    std::cout << "Brand String : " << ProcessorInfo.brandString << "\n";
    if (ProcessorInfo.numLogicalCores > 0) {
        logicalCore = ProcessorInfo.cores.at(coreIndex);
        while (coreIndex < ProcessorInfo.numLogicalCores) {
            logicalCore = ProcessorInfo.cores.at(coreIndex);
            coreType = logicalCore.coreType;
            switch (coreType) {
                case HybridDetect::INTEL_CORE:
                    coreTypeIndex = 0;
                    break;
                case HybridDetect::INTEL_ATOM:
                    coreTypeIndex = 1;
                    break;
                default:
                    coreTypeIndex = 2;
            }
            coreTypeCount[coreTypeIndex]++;
            if (coreTypeCount[coreTypeIndex] == 1) {
                OutputCoreType(coreType);
                OutputCoreInfo(logicalCore);
            }
            if (++coreIndex == ProcessorInfo.numLogicalCores) {
                break;
            }
        }
        std::cout << "\n\n Core Counts:\n";
        OutputTotalCoresOfCoreType(HybridDetect::INTEL_CORE, coreTypeCount[0]);
        OutputTotalCoresOfCoreType(HybridDetect::INTEL_ATOM, coreTypeCount[1]);
    }
}

void UseLegacyCPUInformation(std::string CPUBrandString) {
    std::cout << CPUBrandString << "\n";
}

//main
int main()
{
    int ret;
    bool bMTLorARL = false;
    int Max = 0; 
    int Base = 0;

    std::cout << "cpu detect!\n";

    GetProcessorInfo(ProcessorInfo);
    if (ProcessorInfo.IsIntel() == true) {

        bMTLorARL = IsIntelMTLorARL();
        if (bMTLorARL) {
            std::cout << "cpu is MTL or ARL!\n";
            UseHybridCPUInformation();
        }
        else {
            std::cout << "cpu is not MTL or ARL!\n";
            if (ProcessorInfo.hybrid == true) {
                std::cout << "cpu is ADL!\n";
                UseHybridCPUInformation();
            }
            else {
                UseLegacyCPUInformation(ProcessorInfo.brandString);
                UseHybridCPUInformation();
            }
        }
    }

}
