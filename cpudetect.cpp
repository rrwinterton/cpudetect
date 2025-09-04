// cpudetect.cpp
//

#include <windows.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <intrin.h>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <thread>
#include <stdint.h>
#include <winreg.h>
#include <tchar.h>
#include "./include/HybridDetect.h"

//processor names
#define TIGER_LAKE 1
#define RAPTOR_LAKE 2
#define ALDER_LAKE 3
#define ARROW_LAKE 4
#define METEOR_LAKE 5
#define LUNAR_LAKE 6

//todo:rrw note frequency base and max issues
#define LEAF_FREQUENCY_INFORMATION				0x16        // Processor Frequency Information Leaf  function 0x16 only works on Skylake or newer.
#define MEM_DIV                                 1024        // Used for getting system memory information

//todo:rrw do we want global?
HybridDetect::PROCESSOR_INFO ProcessorInfo;
typedef BOOL(WINAPI *PGetPhysicallyInstalledSystemMemory)(PULONGLONG TotalMemoryInKiloBytes);

void GPUDriverVersion()
{
    std::cout << std::endl << std::endl << "GPU default or primary driver version details follow: " << std::endl;

	// Fetch registry data
	HKEY dxKeyHandle = nullptr;
	DWORD numOfAdapters = 0;
    unsigned int dxDriverVersion[4];

	LSTATUS returnCode = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\DirectX"), 0, KEY_READ, &dxKeyHandle );

	if( returnCode != ERROR_SUCCESS )
	{
		std::cout << "Unable to open registry key handle" << std::endl;
        return;
	}

	// Find all subkeys

	DWORD subKeyMaxLength = 0;

	returnCode = ::RegQueryInfoKey(
		dxKeyHandle,
		nullptr,
		nullptr,
		nullptr,
		&numOfAdapters,
		&subKeyMaxLength,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	);

	if( returnCode != ERROR_SUCCESS )
	{
		std::cout << "Unable to read registry value" << std::endl;
        return;
	}

	subKeyMaxLength += 1; // include the null character

	uint64_t driverVersionRaw = 0;

	bool foundSubkey = false;
	TCHAR* subKeyName = new TCHAR[subKeyMaxLength];

   //Code retrieves all the adaptors, typically the first adapter is the default one, use it to provide driver version
	for( DWORD i = 0; i < 1; ++i )
	{
		DWORD subKeyLength = subKeyMaxLength;

		returnCode = ::RegEnumKeyEx(
			dxKeyHandle,
			i,
			subKeyName,
			&subKeyLength,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		);

		if( returnCode == ERROR_SUCCESS )
		{
			LUID adapterLUID = {};
			DWORD qwordSize = sizeof( uint64_t );

			returnCode = ::RegGetValue(
				dxKeyHandle,
				subKeyName,
				_T("AdapterLuid"),
				RRF_RT_QWORD,
				nullptr,
				&adapterLUID,
				&qwordSize
			);

			if( returnCode == ERROR_SUCCESS) // If we were able to retrieve the registry values
				 
			{
				// We have our registry key! Let's get the driver version num now

				returnCode = ::RegGetValue(
					dxKeyHandle,
					subKeyName,
					_T("DriverVersion"),
					RRF_RT_QWORD,
					nullptr,
					&driverVersionRaw,
					&qwordSize
				);

				if( returnCode == ERROR_SUCCESS )
				{
					foundSubkey = true;
					break;
				}
			}
		}
	}

	returnCode = ::RegCloseKey( dxKeyHandle );
	assert( returnCode == ERROR_SUCCESS );
	delete[] subKeyName;

	if( !foundSubkey )
	{
		std::cout << "Error Missing Driver Info" << std::endl;
        return;
	}

	// Now that we have our driver version as a DWORD, let's process that into something readable
	dxDriverVersion[ 0 ] = (unsigned int) ( ( driverVersionRaw & 0xFFFF000000000000 ) >> 16 * 3 );
	dxDriverVersion[ 1 ] = (unsigned int) ( ( driverVersionRaw & 0x0000FFFF00000000 ) >> 16 * 2 );
	dxDriverVersion[ 2 ] = (unsigned int) ( ( driverVersionRaw & 0x00000000FFFF0000 ) >> 16 * 1 );
	dxDriverVersion[ 3 ] = (unsigned int) ( ( driverVersionRaw & 0x000000000000FFFF ) );

	std::cout << "\n "<< dxDriverVersion[0] << "." <<  dxDriverVersion[1] << "." <<  dxDriverVersion[2] << "." <<  dxDriverVersion[3]; 
    std::cout << "\n" ; 

}

//todo: update to return version in production 
void NPUDriverVersion()
{
    std::cout << "\n\n NPU driver version details follow: " << std::endl;
    // Fetch registry data
	HKEY nKeyHandle = nullptr;
	DWORD numOfAdapters = 0;

    //Start with getting a Key Handle 
    LSTATUS returnCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Class\\{f01a9d53-3ff6-48d2-9f97-c8a7004be10c}"), 0, KEY_READ, &nKeyHandle );

    if( returnCode != ERROR_SUCCESS )
	{
		std::cout << "Unable to open registry key handle" << std::endl;
        return;
	}
    
    //Get subkey from registry key, it is {0000} by default
    DWORD subKeyMaxLength = 0;

	returnCode = ::RegQueryInfoKey(
		nKeyHandle,
		nullptr,
		nullptr,
		nullptr,
		&numOfAdapters,
		&subKeyMaxLength,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	);
    
    if( returnCode != ERROR_SUCCESS )
	{
		std::cout << "Unable to enumerate registry subkey" << std::endl;
        return;
	}

    subKeyMaxLength += 1; // include the null character

	bool foundSubkey = false;
	TCHAR* subKeyName = new TCHAR[subKeyMaxLength];

    //Code will iterate through all adapters or subkeys, since we have a default value upfront setting this to single subkey for now
	for( DWORD i = 0; i < 1; ++i )
	{
		DWORD subKeyLength = subKeyMaxLength;

		returnCode = ::RegEnumKeyEx(
			nKeyHandle,
			i,
			subKeyName,
			&subKeyLength,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		);

		if( returnCode == ERROR_SUCCESS )
		{
            DWORD dataSize = 0;
            DWORD dataSize2 = 0;
            DWORD dataSize3 = 0;
            
            
    
				// We have our registry key! Let's get the driver version num now
                // Retrieve data size first so we can create a variable with the size needed 

				returnCode = ::RegGetValue(
					nKeyHandle,
					subKeyName,
					_T("DriverVersion"),
					RRF_RT_REG_SZ,
					nullptr,
					nullptr,
					&dataSize
				);
                
                
               
                LPBYTE lpData = (LPBYTE)malloc(dataSize);

                returnCode = ::RegGetValue(
					nKeyHandle,
					subKeyName,
					_T("DriverVersion"),
					RRF_RT_REG_SZ,
					nullptr,
                    lpData,
					&dataSize
				); 

				if( returnCode == ERROR_SUCCESS )
				{
					foundSubkey = true;
                    
                    std::cout << "\n\n"; 
                    printf("NPU Driver Version: %s\n", (char*)lpData);
                 
				}
                free(lpData);

                //Get NPU Driver Description 
                returnCode = ::RegGetValue(
					nKeyHandle,
					subKeyName,
					_T("DriverDesc"),
					RRF_RT_REG_SZ,
					nullptr,
					nullptr,
					&dataSize2
				);
                
                
               
                LPBYTE lpData2 = (LPBYTE)malloc(dataSize2);

                returnCode = ::RegGetValue(
					nKeyHandle,
					subKeyName,
					_T("DriverDesc"),
					RRF_RT_REG_SZ,
					nullptr,
                    lpData2,
					&dataSize2
				); 

				if( returnCode == ERROR_SUCCESS )
				{
					foundSubkey = true;
                    
                    std::cout << "\n\n"; 
                    printf("NPU Driver Description: %s\n", (char*)lpData2);
                 
				}
                free(lpData2);

                 //Get NPU Driver Inf Details - Includes NPU version
                returnCode = ::RegGetValue(
					nKeyHandle,
					subKeyName,
					_T("InfSection"),
					RRF_RT_REG_SZ,
					nullptr,
					nullptr,
					&dataSize3
				);
                
                
               
                LPBYTE lpData3 = (LPBYTE)malloc(dataSize3);

                returnCode = ::RegGetValue(
					nKeyHandle,
					subKeyName,
					_T("InfSection"),
					RRF_RT_REG_SZ,
					nullptr,
                    lpData3,
					&dataSize3
				); 

				if( returnCode == ERROR_SUCCESS )
				{
					foundSubkey = true;
                    
                    std::cout << "\n\n"; 
                    printf("NPU Driver Inf Description: %s\n", (char*)lpData3);
                 
				}
                free(lpData3);
			
		}
	}

	returnCode = ::RegCloseKey( nKeyHandle );
	assert( returnCode == ERROR_SUCCESS );
	delete[] subKeyName;
  

	if( !foundSubkey )
	{
		std::cout << "Error Missing Driver Info" << std::endl;
        return;
	}


}

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


//IntelProcessor
bool IntelProcessor(int &ProcessorName)
{
    const int kVendorID_Intel[3] = { 0x756e6547, 0x6c65746e, 0x49656e69 }; // "GenuntelineI"
    int regs[4];
    bool bDetected = false;
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

        bDetected = (family == 6);
		ProcessorName = 0;
		if (bDetected)
        {
            switch (extModel)
            {
			case 0x8E: // TGL-U
				ProcessorName = TIGER_LAKE;
				break;
			case 0x9A: // ADL
				ProcessorName = ALDER_LAKE;
				break;
            case 0xAA: // MTL-P/M, ARL-Z0
            case 0xAC: // MTL-S
				ProcessorName = METEOR_LAKE;
                break;
			case 0xB7:
				ProcessorName = RAPTOR_LAKE;
				break;

            case 0xB5: // ARL-U
            case 0xC5: // ARL-P
            case 0xC6: // ARL-S/HX
                ProcessorName = ARROW_LAKE;
                break;
            case 0xBD: // LNL
                ProcessorName = LUNAR_LAKE;
                break;
            default:
                bDetected = false;
            }
        }
		std::cout << "extModel: " << std::hex << extModel << std::endl;
        std::cout << "CPUID: 0x" << std::hex << std::setw(8) << std::setfill('0') << basicCPUID << ", Detected = " << bDetected << std::endl << std::dec;
    }
    return bDetected;
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
		std::cout << std::endl <<  "hybrid core counts:" << std::endl;
        OutputTotalCoresOfCoreType(HybridDetect::INTEL_CORE, coreTypeCount[0]);
        OutputTotalCoresOfCoreType(HybridDetect::INTEL_ATOM, coreTypeCount[1]);

        
    }
}

void UseLegacyCPUInformation(std::string CPUBrandString) {
    std::cout << CPUBrandString << "\n";
}

void MemoryInformation()
{
	std::cout << std::endl << std::endl << "Memory Information follows....." << std::endl;
    std::cout << "\n\n";

    //Get information about memory installed on system
    HINSTANCE hGetProcDLL = LoadLibrary("kernel32.dll");
    if(!hGetProcDLL)
    {
        std::cout << "Unable to load dynamic library" << std::endl;
        return;
    }
    ULONGLONG installedMem;
	PGetPhysicallyInstalledSystemMemory getInstalledMem_out = (PGetPhysicallyInstalledSystemMemory)GetProcAddress(hGetProcDLL, "GetPhysicallyInstalledSystemMemory");
	if (!getInstalledMem_out) {
		std::cout << "Unable to locate the get installed memory function" << std::endl;
		return;
	}
	
	if (!getInstalledMem_out(&installedMem)) {
		std::cout << "Error calling get installed memory function." << std::endl;

		DWORD error = GetLastError();
		std::cout << "Error code is: " << std::hex << error << std::endl;
		return;
	}
	
	std::cout << "total installed memory in GB: " << installedMem / MEM_DIV / MEM_DIV << std::endl;

    //Get information about memory available on system
    MEMORYSTATUSEX statusex;

    statusex.dwLength = sizeof (statusex);

    GlobalMemoryStatusEx (&statusex) ; 

    std::cout << "Amount of memory in use in GB: " << (statusex.dwMemoryLoad * installedMem) / 100 / MEM_DIV / MEM_DIV << std::endl ; 

    std::cout << "Amount of free physical memory available in KB: " << (statusex.ullAvailPhys) / MEM_DIV << std::endl; 

    std::cout << "Amount of free physical paging file available in KB: " << (statusex.ullAvailPageFile) / MEM_DIV << std::endl;

    std::cout << "Amount of free virtual memory available in KB: " << (statusex.ullAvailVirtual) / MEM_DIV << std::endl;



}

//main - uses Windows API only

int main()
{
    int ret = 0;
    bool bDetected = false;
    int Max = 0; 
    int Base = 0;
    int sleepTime = 1;
	int ProcessorName = 0;
    
    std::cout << "cpu detect!\n";
   

    GetProcessorInfo(ProcessorInfo);
    if (ProcessorInfo.IsIntel() == true) {

        bDetected = IntelProcessor(ProcessorName);
		switch (ProcessorName) {
		case TIGER_LAKE:
			std::cout << "Tiger Lake" << std::endl;
			break;
		case RAPTOR_LAKE:
			std::cout << "Raptor Lake" << std::endl;
			break;
		case ALDER_LAKE:
			std::cout << "Alder Lake" << std::endl;
			break;
		case ARROW_LAKE:
			std::cout << "Arrow Lake" << std::endl;
			break;
		case METEOR_LAKE:
			std::cout << "Meteor Lake" << std::endl;
			break;
		case LUNAR_LAKE: 
			std::cout << "Lunar Lake" << std::endl;
			break;
		default:
			std::cout << "Unknown" << std::endl;
			break;
		}
        if (bDetected) {
              UseHybridCPUInformation();
              MemoryInformation(); //Get physical installed memory and memory availble
              GPUDriverVersion(); // Get GPU driver version
              if (ProcessorName > ALDER_LAKE) {
				  std::cout << "CPU has NPU!\n";
				  NPUDriverVersion(); //Get NPU driver version
              }
        }
        else {
            std::cout << "cpu is older!\n";
                UseLegacyCPUInformation(ProcessorInfo.brandString);
                MemoryInformation(); //Get physical installed memory and memory availble
                GPUDriverVersion(); //Get GPU driver version
            }
    }

}
