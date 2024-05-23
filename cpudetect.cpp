// cpudetect.cpp
//

#include <windows.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <intrin.h>
#include <iomanip>
#include "HybridDetect.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <stdint.h>
#include <winreg.h>
#include <tchar.h>

#include "IpfClient.h"
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

//todo:rrw note frequency base and max issues
#define LEAF_FREQUENCY_INFORMATION				0x16        // Processor Frequency Information Leaf  function 0x16 only works on Skylake or newer.
#define MEM_DIV                                 1024        // Used for getting system memory information

//todo:rrw do we want global?
HybridDetect::PROCESSOR_INFO ProcessorInfo;
std::array<int,2> cores = {1,1}; //setup an array to get total Atom/Core numbers (needed for IPF)
std::unique_ptr<Ipf::ClientApiJson> ipf = nullptr;
typedef BOOL(WINAPI *PGetPhysicallyInstalledSystemMemory)(PULONGLONG TotalMemoryInKiloBytes);

//todo:remove in production
void ConsolePrint(std::string ipfCommand, Json result)
{
	if (result.is_null()) {
		std::cout << ipfCommand << "\n unexpectedly returned null." << std::endl;
	}
	else {
		std::cout << "\n" << ipfCommand << result.dump(4);
	}
}

void GPUDriverVersion()
{
    std::cout << "\n\n GPU default or primary driver version details follow: " << std::endl;

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
        cores[0] = coreTypeCount[0];
        OutputTotalCoresOfCoreType(HybridDetect::INTEL_CORE, coreTypeCount[0]);
        cores[1] = coreTypeCount[1];
        OutputTotalCoresOfCoreType(HybridDetect::INTEL_ATOM, coreTypeCount[1]);

        
    }
}

void UseLegacyCPUInformation(std::string CPUBrandString) {
    std::cout << CPUBrandString << "\n";
}

//For code review only to be updated with LNL-M code drop
void getIPFSoCInformation()
{
        // Get SoC brand name
        // GetNode will show properties & values of the node, GetValue gets value of particular namespace variable
		auto node = ipf->GetValue("Platform.SOC.Info.BrandString");
		ConsolePrint("Platform.SOC.Info.BrandString: ", node);

        //Get SoC Family
        node = ipf->GetValue("Platform.SOC.Info.Family");
		ConsolePrint("Platform.SOC.Info.Family: ", node);

        //Get SoC temperature
        node = ipf->GetValue("Platform.SOC.Thermal.CurrentTemperature.IA");
		ConsolePrint("Platform.SOC.Thermal.CurrentTemperature.IA: ", node);

        //Get Highest Non Turbo Frequency
        node = ipf->GetValue("Platform.CPU.Performance.MaxNonTurboFrequency");
		ConsolePrint("Platform.CPU.Performance.MaxNonTurboFrequency: ", node);


		// Inspect PL1 node
		node = ipf->GetValue("Platform.SOC.Power.PL1");
		ConsolePrint("Platform.SOC.Power.PL1:", node); //use for load and setup

        //Get Platform TDP
        node = ipf->GetValue("Platform.SOC.Power.ThermalDesignPower");
		ConsolePrint("Platform.SOC.Power.ThermalDesignPower:", node);

        
        // Get CPU Workload classification
		node = ipf->GetValue("Platform.CPU.State.IsHWWorkloadClassificationSupported");
		ConsolePrint("Platform.CPU.State.IsHWWorkloadClassificationSupported: ", node);

         // Get Current CPU Workload classification - for the calling app/thread
		node = ipf->GetValue("Platform.CPU.State.HWWorkloadClassification");
		ConsolePrint("Platform.CPU.State.HWWorkloadClassification: ", node);

          // Get Current OS Workload classification (pre MTL) - for the calling app/thread 
		node = ipf->GetValue("Platform.CPU.State.SWWorkloadClassification");
		ConsolePrint("Platform.CPU.State.SWWorkloadClassification: ", node);

        //Get Thermal data across Atom cores  - recommend change to pkg temp
        node = ipf->GetValue("Platform.System.Thermal.CurrentTemperature.Atom[].Max");
		ConsolePrint("Platform.System.Thermal.CurrentTemperature.Atom[].Max: ", node);
        node = ipf->GetValue("Platform.System.Thermal.CurrentTemperature.Atom[].Min");
		ConsolePrint("Platform.System.Thermal.CurrentTemperature.Atom[].Min: ", node);

        //Get Thermal data across CPU cores 
        node = ipf->GetValue("Platform.System.Thermal.CurrentTemperature.Core[].Max");
		ConsolePrint("Platform.System.Thermal.CurrentTemperature.Core[].Max: ", node);
        node = ipf->GetValue("Platform.System.Thermal.CurrentTemperature.Core[].Min");
		ConsolePrint("Platform.System.Thermal.CurrentTemperature.Core[].Min: ", node);



        
        //Get CPU utilization information
        //INTEL_CORE
        for (int i = 0; i < cores[0]; i++)
        {
            auto s1 = std::to_string(i);
            node = ipf->GetValue("Platform.CPU.Info.Core["+ s1 + "].Utilization");
		    ConsolePrint("Platform.CPU.Info.Core[].Utilization: ", node);
            node = ipf->GetValue("Platform.CPU.Performance.Core["+ s1 + "].Frequency");
		    ConsolePrint("Platform.CPU.Info.Core[].Utilization: ", node);
            node = ipf->GetValue("Platform.CPU.Thermal.Core["+ s1 + "].Temperature");
		    ConsolePrint("Platform.CPU.Info.Core[].Utilization: ", node);
        }
        //INTEL_ATOM
        for (int i = 0; i < cores[1]; i++)
        {
            auto s2 = std::to_string(i);
            node = ipf->GetValue("Platform.CPU.Info.Core["+ s2 + "].Utilization");
		    ConsolePrint("Platform.CPU.Info.Core[].Utilization: ", node);
            node = ipf->GetValue("Platform.CPU.Performance.Core["+ s2 + "].Frequency");
		    ConsolePrint("Platform.CPU.Info.Core[].Utilization: ", node);
            node = ipf->GetValue("Platform.CPU.Thermal.Core["+ s2 + "].Temperature");
		    ConsolePrint("Platform.CPU.Info.Core[].Utilization: ", node);
        }

}
//LNL-M Pre-release IPF information 
void getMTLIPFInformation()
{
    //Get Platform TDP in mW 
    auto node = ipf->GetValue("Platform.SOC.Power.ThermalDesignPower");
	ConsolePrint("Platform.SOC.Power.ThermalDesignPower in mW: ", node);

    // Get Current CPU Workload classification - for the calling app/thread
	node = ipf->GetValue("Platform.CPU.State.HWWorkloadClassification");
	ConsolePrint("Platform.CPU.State.HWWorkloadClassification: ", node);

    //Get entire node values for SOC.Power Namespace (Note: requires admin privileges if uncommented)
    auto jsonStream = ipf->GetNode("Platform.SOC.Power");
	ConsolePrint("ipf->GetNode(\"Platform.SOC.Power\")", jsonStream.dump(4));

    //Get entire node values for System.State Namespace
    auto jsonStream2 = ipf->GetNode("Platform.System.State");
	ConsolePrint("ipf->GetNode(\"Platform.System.State\")", jsonStream2.dump(4));

    //Get entire node values for SOC.Thermal Namespace
    jsonStream2 = ipf->GetNode("Platform.SOC.Thermal");
	ConsolePrint("ipf->GetNode(\"Platform.SOC.Thermal\")", jsonStream2.dump(4));

     // Get Current iGfx energy till the last time the register was cleared in mJ (Note: requires admin privileges if uncommented)
	//node = ipf->GetValue("Platform.SOC.Power.IntegratedGfxEnergy");
	//ConsolePrint("Platform.SoC.Power.IntegratedGfxEnergy in mJ: ", node);

    // Get Current iGfx power in mW (Note: requires admin privileges if uncommented)
	//node = ipf->GetValue("Platform.SOC.Power.IntegratedGfx");
	//ConsolePrint("Platform.SoC.Power.IntegratedGfx in mW: ", node);

    // Get Current SoC package power in mW (Note: requires admin privileges if uncommented)
	//node = ipf->GetValue("Platform.SOC.Power.PSys");
	//ConsolePrint("Platform.SoC.Power.PSys in mW: ", node);

    // Get Current IA power in mW (Note: requires admin privileges if uncommented)
	//node = ipf->GetValue("Platform.SOC.Power.IA");
	//ConsolePrint("Platform.SoC.Power.IA in mW: ", node);

    // Get Current SoC Thermal Package Temp in Deci-K
	node = ipf->GetValue("Platform.SOC.Thermal.PackageTemp");
	ConsolePrint("Platform.SOC.Thermal.PackageTemp in Deci-K: ", node);


}
//For code review only to be updated with LNL-M code drop
void getIPFNPUInformation()
{

    //Get NPU Thermal temperature - to be updated with additional namespace items for LNL-M
    auto node = ipf->GetValue("Platform.SOC.Thermal.CurrentTemperature.NPU");
	ConsolePrint("Platform.SOC.Thermal.CurrentTemperature.NPU:  ", node);

}
//For code review only to be updated with LNL-M code drop
void getIPFIGPUInformation()
{
  

     // Get current Thermal details
	auto node = ipf->GetValue("Platform.SOC.Thermal.CurrentTemperature.GT.Max");
	ConsolePrint("Platform.SOC.Thermal.CurrentTemperature.GT.Max:  ", node);

    node = ipf->GetValue("Platform.SOC.Thermal.CurrentTemperature.GT.Min");
	ConsolePrint("Platform.SOC.Thermal.CurrentTemperature.GT.Min:  ", node);

    //GPU GT Engine busy count 
    node = ipf->GetValue("Platform.Graphics.IGfx.Performance.Residency.GTEngineBusyResidency");
	ConsolePrint("Platform.Graphics.IGfx.Performance.Residency.GTEngineBusyResidency: ", node);


    //GPU Actual Frequency
    node = ipf->GetValue("Platform.Graphics.IGfx.Performance.Frequency.ActualFrequency");
	ConsolePrint("Platform.Graphics.IGfx.Performance.Frequency.ActualFrequency: ", node);

    //GPU Min Frequency
    node = ipf->GetValue("Platform.Graphics.IGfx.Performance.Frequency.CurrentMinFrequency");
	ConsolePrint("Platform.Graphics.IGfx.Performance.Frequency.CurrentMinFrequency: ", node);

    //GPU Max Frequency
    node = ipf->GetValue("Platform.Graphics.IGfx.Performance.Frequency.CurrentMaxFrequency");
	ConsolePrint("Platform.Graphics.IGfx.Performance.Frequency.CurrentMaxFrequency: ", node);

    //GPU Engine Utlization
    node = ipf->GetValue("Platform.Graphics.IGfx.Info.Utilization.GfxEngineUtilization");
	ConsolePrint("Platform.Graphics.IGfx.Info.Utilization.GfxEngineUtilization: ", node);

     //Media Engine Utlization
    node = ipf->GetValue("Platform.Graphics.IGfx.Info.Utilization.MediaEngineUtilization");
	ConsolePrint("Platform.Graphics.IGfx.Info.Utilization.MediaEngineUtilization: ", node);

    //Render Engine Utlization
    node = ipf->GetValue("Platform.Graphics.IGfx.Info.Utilization.RenderEngineUtilization");
	ConsolePrint("Platform.Graphics.IGfx.Info.Utilization.RenderEngineUtilization: ", node);

    //IGFX engine power
    node = ipf->GetValue("Platform.SOC.Power.IntegratedGfx");
	ConsolePrint("Platform.SOC.Power.IntegratedGfx: ", node);



}

void MemoryInformation()
{
    std::cout << " \n\n Memory Information follows....." << std::endl;
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

//main - code needs to be run as Admin for some IPF namespace access, this is for evaluation only in MTL, to be updated in LNL-M 

int main()
{
    int ret = 0;
    bool bMTLorARL = false;
    int Max = 0; 
    int Base = 0;
    int sleepTime = 2;
    

    std::cout << "cpu detect!\n";
   

    GetProcessorInfo(ProcessorInfo);
    if (ProcessorInfo.IsIntel() == true) {

        bMTLorARL = IsIntelMTLorARL();
        if (bMTLorARL) {
              std::cout << "cpu is MTL or ARL!\n";
              UseHybridCPUInformation();
              MemoryInformation(); //Get physical installed memory and memory availble
              GPUDriverVersion(); // Get GPU driver version
              NPUDriverVersion(); //Get NPU driver version

             try{
              
              
               // Load IPF Extensible Framework using the Json Api
		      ipf = std::make_unique<Ipf::ClientApiJson>();
              auto value = ipf->GetValue("Platform.SOC.Power.ThermalDesignPower");
		      //ConsolePrint("Platform.SOC.Power.ThermalDesignPower: ", value); -- remove, for test only
              auto maxTries = 5;
              int sleepInt = 2;
              bool ipfConnection = false;

              for(size_t i = 0; i < maxTries; i++)
              {
                if(value.is_null()) {
                    //todo:: May not be needed in production, used for developer testing only
                    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
				     value = ipf->GetValue("Platform.SOC.Power.ThermalDesignPower");
		             //ConsolePrint("Platform.SOC.Power.ThermalDesignPower: ", value); -- remove for test only

                }else {
                    ipfConnection = true;
                    break;
                }
              }
              if(ipfConnection != true) {
                std::cerr << "Unable to establish a connection to IPF core after " << maxTries
					  << " attempts. Exiting" << std::endl;
			    return -1;
              }
              
              //to be updated in follow on code drop for LNL-M with single Core Provider Package
       
              //getIPFSoCInformation();
              //getIPFIGPUInformation();
              //getIPFNPUInformation();
              std::cout << "\n IPF Namespace output \n";
              
              //Currently functional IPF namespace for MTL pre-release 
              getMTLIPFInformation();

        
            }
            catch (const Ipf::IpfException& e){
                std::cerr << "\n Got IpfException when accessing the namespace: " << e.what();
		        ret = -1;
            }
            catch (...) {
		        std::cerr << "\n Unexpected exception while processing IPF system detect" << std::endl;
		        ret = -1;
	        }
            return ret;



        }
        else {
            std::cout << "cpu is not MTL or ARL!\n";
            if (ProcessorInfo.hybrid == true) {
                std::cout << "cpu is ADL!\n";
                UseHybridCPUInformation();
                MemoryInformation(); //Get physical installed memory and memory availble
                GPUDriverVersion(); //Get GPU driver version

        
            }
            else {
                UseLegacyCPUInformation(ProcessorInfo.brandString);
                MemoryInformation(); //Get physical installed memory and memory availble
                GPUDriverVersion(); //Get GPU driver version
            }
        }
    }

}
