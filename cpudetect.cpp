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

// Version
#define CPUDETECT_MAJOR_VERSION 1
#define CPUDETECT_MINOR_VERSION 1

// processor names
#define TIGER_LAKE 1
#define RAPTOR_LAKE 2
#define ALDER_LAKE 3
#define ARROW_LAKE 4
#define METEOR_LAKE 5
#define LUNAR_LAKE 6
#define PANTHER_LAKE 7

static const int kCpuHasSSE2 = 0x100;
static const int kCpuHasSSSE3 = 0x200;
static const int kCpuHasSSE41 = 0x400;
static const int kCpuHasSSE42 = 0x800;
static const int kCpuHasAVX = 0x1000;
static const int kCpuHasAVX2 = 0x2000;
static const int kCpuHasERMS = 0x4000;
static const int kCpuHasFSMR = 0x8000;
static const int kCpuHasFMA3 = 0x10000;
static const int kCpuHasF16C = 0x20000;
static const int kCpuHasAVX512BW = 0x40000;
static const int kCpuHasAVX512VL = 0x80000;
static const int kCpuHasAVX512VNNI = 0x100000;
static const int kCpuHasAVX512VBMI = 0x200000;
static const int kCpuHasAVX512VBMI2 = 0x400000;
static const int kCpuHasAVX512VBITALG = 0x800000;
static const int kCpuHasAVX10 = 0x1000000;
static const int kCpuHasAVX10_2 = 0x2000000;
static const int kCpuHasAVXVNNI = 0x4000000;
static const int kCpuHasAVXVNNIINT8 = 0x8000000;
static const int kCpuHasAMXINT8 = 0x10000000;
static const int kCpuHasGFNI = 0x00000100;

// todo:rrw note frequency base and max issues
#define LEAF_FREQUENCY_INFORMATION 0x16 // Processor Frequency Information Leaf  function 0x16 only works on Skylake or newer.
#define MEM_DIV 1024					// Used for getting system memory information

// todo:rrw do we want global?
HybridDetect::PROCESSOR_INFO ProcessorInfo;
typedef BOOL(WINAPI *PGetPhysicallyInstalledSystemMemory)(PULONGLONG TotalMemoryInKiloBytes);

void GPUDriverVersion()
{
	std::cout << std::endl
			  << "GPU Information:" << std::endl;

	// Fetch registry data
	HKEY dxKeyHandle = nullptr;
	DWORD numOfAdapters = 0;
	unsigned int dxDriverVersion[4];

	LSTATUS returnCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\DirectX"), 0, KEY_READ, &dxKeyHandle);

	if (returnCode != ERROR_SUCCESS)
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
		nullptr);

	if (returnCode != ERROR_SUCCESS)
	{
		std::cout << "Unable to read registry value" << std::endl;
		return;
	}

	subKeyMaxLength += 1; // include the null character

	uint64_t driverVersionRaw = 0;

	bool foundSubkey = false;
	TCHAR *subKeyName = new TCHAR[subKeyMaxLength];

	// Code retrieves all the adaptors, typically the first adapter is the default one, use it to provide driver version
	for (DWORD i = 0; i < 1; ++i)
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
			nullptr);

		if (returnCode == ERROR_SUCCESS)
		{
			LUID adapterLUID = {};
			DWORD qwordSize = sizeof(uint64_t);

			returnCode = ::RegGetValue(
				dxKeyHandle,
				subKeyName,
				_T("AdapterLuid"),
				RRF_RT_QWORD,
				nullptr,
				&adapterLUID,
				&qwordSize);

			if (returnCode == ERROR_SUCCESS) // If we were able to retrieve the registry values

			{
				// We have our registry key! Let's get the driver version num now

				returnCode = ::RegGetValue(
					dxKeyHandle,
					subKeyName,
					_T("DriverVersion"),
					RRF_RT_QWORD,
					nullptr,
					&driverVersionRaw,
					&qwordSize);

				if (returnCode == ERROR_SUCCESS)
				{
					foundSubkey = true;
					break;
				}
			}
		}
	}

	returnCode = ::RegCloseKey(dxKeyHandle);
	assert(returnCode == ERROR_SUCCESS);
	delete[] subKeyName;

	if (!foundSubkey)
	{
		std::cout << "Error Missing Driver Info" << std::endl;
		return;
	}

	// Now that we have our driver version as a DWORD, let's process that into something readable
	dxDriverVersion[0] = (unsigned int)((driverVersionRaw & 0xFFFF000000000000) >> 16 * 3);
	dxDriverVersion[1] = (unsigned int)((driverVersionRaw & 0x0000FFFF00000000) >> 16 * 2);
	dxDriverVersion[2] = (unsigned int)((driverVersionRaw & 0x00000000FFFF0000) >> 16 * 1);
	dxDriverVersion[3] = (unsigned int)((driverVersionRaw & 0x000000000000FFFF));

	std::cout << "default/primary driver version - " << dxDriverVersion[0] << "." << dxDriverVersion[1] << "." << dxDriverVersion[2] << "." << dxDriverVersion[3] << std::endl
			  << std::endl;
}

// todo: update to return version in production
void NPUDriverVersion()
{
	std::cout << "NPU Details: " << std::endl;
	// Fetch registry data
	HKEY nKeyHandle = nullptr;
	DWORD numOfAdapters = 0;

	// Start with getting a Key Handle
	LSTATUS returnCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Class\\{f01a9d53-3ff6-48d2-9f97-c8a7004be10c}"), 0, KEY_READ, &nKeyHandle);

	if (returnCode != ERROR_SUCCESS)
	{
		std::cout << "Unable to open registry key handle" << std::endl;
		return;
	}

	// Get subkey from registry key, it is {0000} by default
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
		nullptr);

	if (returnCode != ERROR_SUCCESS)
	{
		std::cout << "Unable to enumerate registry subkey" << std::endl;
		return;
	}

	subKeyMaxLength += 1; // include the null character

	bool foundSubkey = false;
	TCHAR *subKeyName = new TCHAR[subKeyMaxLength];

	// Code will iterate through all adapters or subkeys, since we have a default value upfront setting this to single subkey for now
	for (DWORD i = 0; i < 1; ++i)
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
			nullptr);

		if (returnCode == ERROR_SUCCESS)
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
				&dataSize);

			LPBYTE lpData = (LPBYTE)malloc(dataSize);

			returnCode = ::RegGetValue(
				nKeyHandle,
				subKeyName,
				_T("DriverVersion"),
				RRF_RT_REG_SZ,
				nullptr,
				lpData,
				&dataSize);

			if (returnCode == ERROR_SUCCESS)
			{
				foundSubkey = true;
				std::cout << "NPU Driver Version: " << (char *)lpData << std::endl;
			}
			free(lpData);

			// Get NPU Driver Description
			returnCode = ::RegGetValue(
				nKeyHandle,
				subKeyName,
				_T("DriverDesc"),
				RRF_RT_REG_SZ,
				nullptr,
				nullptr,
				&dataSize2);

			LPBYTE lpData2 = (LPBYTE)malloc(dataSize2);

			returnCode = ::RegGetValue(
				nKeyHandle,
				subKeyName,
				_T("DriverDesc"),
				RRF_RT_REG_SZ,
				nullptr,
				lpData2,
				&dataSize2);

			if (returnCode == ERROR_SUCCESS)
			{
				foundSubkey = true;
				std::cout << "NPU Driver Description: " << (char *)lpData2 << std::endl;
			}
			free(lpData2);

			// Get NPU Driver Inf Details - Includes NPU version
			returnCode = ::RegGetValue(
				nKeyHandle,
				subKeyName,
				_T("InfSection"),
				RRF_RT_REG_SZ,
				nullptr,
				nullptr,
				&dataSize3);

			LPBYTE lpData3 = (LPBYTE)malloc(dataSize3);

			returnCode = ::RegGetValue(
				nKeyHandle,
				subKeyName,
				_T("InfSection"),
				RRF_RT_REG_SZ,
				nullptr,
				lpData3,
				&dataSize3);

			if (returnCode == ERROR_SUCCESS)
			{
				foundSubkey = true;
				std::cout << "NPU Driver Inf Description: " << (char *)lpData3 << std::endl;
			}
			free(lpData3);
		}
	}

	returnCode = ::RegCloseKey(nKeyHandle);
	assert(returnCode == ERROR_SUCCESS);
	delete[] subKeyName;

	if (!foundSubkey)
	{
		std::cout << "Error Missing Driver Info" << std::endl;
		return;
	}
}

// todo: rrw validate in production
void OutputCoreType(int coreType)
{
	if (coreType == HybridDetect::INTEL_CORE)
	{
		std::cout << std::endl
				  << "Core type:" << std::endl
				  << "Intel Core" << std::endl;
	}
	else
	{
		if (coreType == HybridDetect::INTEL_ATOM)
		{
			std::cout << "Core type:" << std::endl
					  << "Intel Atom" << std::endl;
		}
	}
}

// todo:rrw comment out in production
void OutputCoreInfo(HybridDetect::LOGICAL_PROCESSOR_INFO logicalCore)
{
	std::cout << "max frequency " << logicalCore.maximumFrequency << std::endl;
	std::cout << "base frequency " << logicalCore.baseFrequency << std::endl;
}

// todo:rrw comment out in production
void OutputTotalCoresOfCoreType(int coreType, int cnt)
{
	if (coreType == HybridDetect::INTEL_CORE)
	{
		std::cout << "core type total: Intel Core " << cnt << std::endl;
	}
	else
	{
		if (coreType == HybridDetect::INTEL_ATOM)
		{
			std::cout << "core type total: Intel Atom " << cnt << std::endl;
		}
	}
}

// IntelProcessor
bool IntelProcessor(int &ProcessorName)
{
	const int kVendorID_Intel[3] = {0x756e6547, 0x6c65746e, 0x49656e69}; // "GenuntelineI"
	bool bDetected = false;
	int cpu_info[4];

	cpu_info[0] = 0;
	cpu_info[1] = 0;
	cpu_info[2] = 0;
	cpu_info[3] = 0;
	__cpuidex(cpu_info, 0, 0);

	bool bIsIntel =
		(kVendorID_Intel[0] == cpu_info[1]) &&
		(kVendorID_Intel[1] == cpu_info[2]) &&
		(kVendorID_Intel[2] == cpu_info[3]);
	if (bIsIntel)
	{
		__cpuidex(cpu_info, 1, 0);
		const int basicCPUID = cpu_info[0];
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
			case 0x97: // ADL-S
			case 0x9D: // ADL-P
				ProcessorName = ALDER_LAKE;
				break;
			case 0xAA: // MTL-P/M, ARL-Z0
			case 0xAC: // MTL-S
			case 0xAE: // MTL-U/Y
				ProcessorName = METEOR_LAKE;
				break;
			case 0xB7: // RPL
			case 0xBA: // RPL-S
			case 0xBE: // RPL-P
			case 0xBF: // RPL-HX
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
			case 0xCC: // PTL
				ProcessorName = PANTHER_LAKE;
				break;
			default:
				bDetected = false;
			}
		}
		//		std::cout << "extModel: " << std::hex << extModel << std::endl;
		//		std::cout << "CPUID: 0x" << std::hex << std::setw(8) << std::setfill('0') << basicCPUID << ", Detected = " << bDetected << std::endl
		//				  << std::dec;
	}
	return bDetected;
}

void UseHybridCPUInformation()
{
	unsigned int coreIndex;
	unsigned int coreType;
	unsigned int coreTypeIndex;
	unsigned int coreTypeCount[4];

	HybridDetect::LOGICAL_PROCESSOR_INFO logicalCore;

	// todo:rrw comment out in production
	std::cout << "number of physical cores " << ProcessorInfo.numPhysicalCores << "\n";
	coreIndex = 0;
	coreTypeIndex = 0;
	coreTypeCount[0] = 0;
	coreTypeCount[1] = 0;
	coreTypeCount[2] = 0;
	coreTypeCount[3] = 0;
	// todo:rrw comment out in production
	std::cout << "Brand String : " << ProcessorInfo.brandString << "\n";
	if (ProcessorInfo.numLogicalCores > 0)
	{
		logicalCore = ProcessorInfo.cores.at(coreIndex);
		while (coreIndex < ProcessorInfo.numLogicalCores)
		{
			logicalCore = ProcessorInfo.cores.at(coreIndex);
			coreType = logicalCore.coreType;
			switch (coreType)
			{
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
			if (coreTypeCount[coreTypeIndex] == 1)
			{
				OutputCoreType(coreType);
				OutputCoreInfo(logicalCore);
				std::cout << std::endl;
			}
			if (++coreIndex == ProcessorInfo.numLogicalCores)
			{
				break;
			}
		}
		std::cout << std::endl
				  << "Hybrid core counts:" << std::endl;
		OutputTotalCoresOfCoreType(HybridDetect::INTEL_CORE, coreTypeCount[0]);
		OutputTotalCoresOfCoreType(HybridDetect::INTEL_ATOM, coreTypeCount[1]);
	}
}

void UseLegacyCPUInformation(std::string CPUBrandString)
{
	std::cout << CPUBrandString << "\n";
}

void MemoryInformation()
{
	std::cout << std::endl
			  << "Memory Information:" << std::endl;

	// Get information about memory installed on system
	HINSTANCE hGetProcDLL = LoadLibrary("kernel32.dll");
	if (!hGetProcDLL)
	{
		std::cout << "Unable to load dynamic library" << std::endl;
		return;
	}
	ULONGLONG installedMem;
	PGetPhysicallyInstalledSystemMemory getInstalledMem_out = (PGetPhysicallyInstalledSystemMemory)GetProcAddress(hGetProcDLL, "GetPhysicallyInstalledSystemMemory");
	if (!getInstalledMem_out)
	{
		std::cout << "Unable to locate the get installed memory function" << std::endl;
		return;
	}

	if (!getInstalledMem_out(&installedMem))
	{
		std::cout << "Error calling get installed memory function." << std::endl;

		DWORD error = GetLastError();
		std::cout << "Error code is: " << std::hex << error << std::endl;
		return;
	}

	std::cout << "total installed memory in GB: " << installedMem / MEM_DIV / MEM_DIV << std::endl;

	// Get information about memory available on system
	MEMORYSTATUSEX statusex;

	statusex.dwLength = sizeof(statusex);

	GlobalMemoryStatusEx(&statusex);

	std::cout << "Amount of memory in use in GB: " << (statusex.dwMemoryLoad * installedMem) / 100 / MEM_DIV / MEM_DIV << std::endl;
	std::cout << "Amount of free physical memory available in KB: " << (statusex.ullAvailPhys) / MEM_DIV << std::endl;
	std::cout << "Amount of free physical paging file available in KB: " << (statusex.ullAvailPageFile) / MEM_DIV << std::endl;
	std::cout << "Amount of free virtual memory available in KB: " << (statusex.ullAvailVirtual) / MEM_DIV << std::endl;
}

int QuickCPUInfo()
{
	int cpu_info[4];

	cpu_info[0] = 1;
	cpu_info[1] = 0;
	cpu_info[2] = 0;
	cpu_info[3] = 0;
	__cpuidex(cpu_info, 1, 0);

	// CPU Family and Model
	// 3:0 - Stepping
	// 7:4 - Model
	// 11:8 - Family
	// 13:12 - Processor Type
	// 19:16 - Extended Model
	// 27:20 - Extended Family
	int family = ((cpu_info[0] >> 8) & 0x0f) | ((cpu_info[0] >> 16) & 0xff0);
	int model = ((cpu_info[0] >> 4) & 0x0f) | ((cpu_info[0] >> 12) & 0xf0);
	printf("CPU Family %d (0x%x), Model %d (0x%x)\n", family, family, model,
		   model);
	return 0;
}

static int GetXCR0()
{
	int xcr0 = 0;
	xcr0 = (int)_xgetbv(0); // VS2010 SP1 required.  NOLINT
	return xcr0;
}

void GetISA()
{
	int cpu_info1[4] = {0, 0, 0, 0};
	int cpu_info7[4] = {0, 0, 0, 0};
	int cpu_einfo7[4] = {0, 0, 0, 0};

	__cpuidex(cpu_info1, 1, 0);
	__cpuidex(cpu_info7, 7, 0);
	__cpuidex(cpu_einfo7, 7, 1);

	std::cout << "ISA Support:" << std::endl;
	if ((cpu_info1[3] & 0x04000000) ? kCpuHasSSE2 : 0) // EDX bit 26
		std::cout << "SSE2: supported" << std::endl;
	else
		std::cout << "SSE2: not supported" << std::endl;

	if ((cpu_info1[2] & 0x00000200) ? kCpuHasSSSE3 : 0) // ECX bit 9
		std::cout << "SSSE3: supported" << std::endl;
	else
		std::cout << "SSSE3: not supported" << std::endl;

	if ((cpu_info1[2] & 0x00080000) ? kCpuHasSSE41 : 0) // ECX bit 19
		std::cout << "SSE4.1: supported" << std::endl;
	else
		std::cout << "SSE4.1: not supported" << std::endl;

	if ((cpu_info1[2] & 0x00100000) ? kCpuHasSSE42 : 0) // ECX bit 20
		std::cout << "SSE4.2: supported" << std::endl;
	else
		std::cout << "SSE4.2: not supported" << std::endl;

	if ((cpu_info7[1] & 0x00000200) ? kCpuHasERMS : 0) // EBX bit 9
		std::cout << "ERMS: supported" << std::endl;
	else
		std::cout << "ERMS: not supported" << std::endl;

	if ((cpu_info7[3] & 0x00000010) ? kCpuHasFSMR : 0) // EDX bit 4
		std::cout << "Fast Short REP MOV: supported" << std::endl;
	else
		std::cout << "Fast Short REP MOV: not supported:" << std::endl;

	if ((cpu_info1[2] & 0x1c000000) == 0x1c000000) // AVX
		std::cout << "AVX: supported" << std::endl;
	else
		std::cout << "AVX: not supported" << std::endl;

	if ((cpu_info7[1] & 0x00000020) ? kCpuHasAVX2 : 0) // EBX bit 5
		std::cout << "AVX2: supported" << std::endl;
	else
		std::cout << "AVX2: not supported" << std::endl;

	if ((GetXCR0() & 6) == 6) // XMM and YMM state enabled by OS
		std::cout << "OS XMM YMM save state: supported" << std::endl;
	else
		std::cout << "OS XMM YMM save state: not supported" << std::endl;

	if ((cpu_info1[2] & 0x00001000) ? kCpuHasFMA3 : 0) // ECX bit 12
		std::cout << "FMA3: supported" << std::endl;
	else
		std::cout << "FMA3: not supported" << std::endl;

	if ((cpu_info1[2] & 0x20000000) ? kCpuHasF16C : 0) // ECX bit 29
		std::cout << "F16C: supported" << std::endl;
	else
		std::cout << "F16C: not supported" << std::endl;

	if ((cpu_info7[1] & 0x40000000) ? kCpuHasAVX512BW : 0) // EBX bit 30
		std::cout << "AVX-512BW: supported" << std::endl;
	else
		std::cout << "AVX-512BW: not supported" << std::endl;

	if ((cpu_info7[1] & 0x80000000) ? kCpuHasAVX512VL : 0) // EBX bit 31
		std::cout << "AVX-512VL: supported" << std::endl;
	else
		std::cout << "AVX-512VL: not supported" << std::endl;

	if ((cpu_info7[2] & 0x00000002) ? kCpuHasAVX512VBMI : 0) // ECX bit 1
		std::cout << "AVX-512VBMI: supported" << std::endl;
	else
		std::cout << "AVX-512VBMI: not supported" << std::endl;

	if ((cpu_einfo7[0] & 0x00000010) ? kCpuHasAVXVNNI : 0) // EAX bit 4
		std::cout << "AVX-VNNI: supported" << std::endl;
	else
		std::cout << "AVX-VNNI: not supported" << std::endl;

	if ((cpu_info7[1] & 0x00000100) ? kCpuHasGFNI : 0) // EBX bit 16
		std::cout << "GFNI: supported" << std::endl;
	else
		std::cout << "GFNI: not supported" << std::endl;
}

// main - uses Windows API only
int main()
{
	int ret = 0;
	bool bDetected = false;
	int Max = 0;
	int Base = 0;
	int sleepTime = 1;
	int ProcessorName = 0;

	std::cout << "cpudetect version: " << CPUDETECT_MAJOR_VERSION << "." << CPUDETECT_MINOR_VERSION << std::endl;
	QuickCPUInfo();
	GetProcessorInfo(ProcessorInfo);
	if (ProcessorInfo.IsIntel() == true)
	{
		std::cout << std::endl
				  << "Processor Name: " << std::endl;
		bDetected = IntelProcessor(ProcessorName);
		switch (ProcessorName)
		{
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
		case PANTHER_LAKE:
			std::cout << "Panther Lake" << std::endl;
			break;
		default:
			std::cout << "Unknown" << std::endl;
			break;
		}

		std::cout << std::endl;
		GetISA(); // Get ISA support information
		std::cout << std::endl;

		if (bDetected)
		{
			UseHybridCPUInformation();
			MemoryInformation(); // Get physical installed memory and memory availble
			GPUDriverVersion();	 // Get GPU driver version
			if (ProcessorName > ALDER_LAKE)
			{
				NPUDriverVersion(); // Get NPU driver version
			}
		}
		else
		{
			std::cout << "CPU is older!\n";
			UseLegacyCPUInformation(ProcessorInfo.brandString);
			MemoryInformation(); // Get physical installed memory and memory availble
			GPUDriverVersion();	 // Get GPU driver version
		}
	}
}
