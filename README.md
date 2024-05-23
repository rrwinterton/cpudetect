```
*******************************************************************************
** INTEL CONFIDENTIAL
**
** Copyright (c) 2020-2024 Intel Corporation All Rights Reserved
**
** This software and the related documents are Intel copyrighted materials,
** and your use of them is governed by the express license under which they
** were provided to you ("License"). Unless the License provides otherwise,
** you may not use, modify, copy, publish, distribute, disclose or transmit
** this software or the related documents without Intel's prior written
** permission.
**
** The source code contained or described herein and all documents related to
** the source code ("Material") are owned by Intel Corporation or its suppliers
** or licensors.
**
** This software and the related documents are provided as is,
** with no express or implied warranties, other than those that are expressly
** stated in the License.
**
*******************************************************************************
```
# Setup
This document covers the setup, build and runtime process for the cpu detect functionality. Documentation related to IPF namespace can be found on google drive [link](https://drive.google.com/drive/folders/1FpDT8CSGsZVlNrsqr2wFRHabYg-5eHLc)
Note: This IPF code is a MTL pre-release version for evaluation only ahead of the LNL-M code drop
LNL-M Code Drops will include the following updates
1. Single package for IPF that combines multiple namespaces to simplify setup and install
2. Bug fixes for namespace conflicts 

Note: IPF also requires that Intel(R) Dynamic Tuning Technology Device Extension Component be installed on the system you can find the compatible software for MTL pre-release in the google drive [link](https://drive.google.com/drive/folders/1FpDT8CSGsZVlNrsqr2wFRHabYg-5eHLc)

## Step 1: download/install the IPF sdk

IPF SDK is available separately in addition to the code included in cpu detect, it can be found on the google drive- [link](https://drive.google.com/drive/folders/1FpDT8CSGsZVlNrsqr2wFRHabYg-5eHLc), name of the file is ipf-2.1.10105.21-windows-sdk. This has sample code and libraries for building/testing.

## Step2 : download IPF core package
IPF core packages need to be installed on the system for testing CPU detect it can be found on the google drive- [link](https://drive.google.com/drive/folders/1FpDT8CSGsZVlNrsqr2wFRHabYg-5eHLc), name of the file is ipf-2.1.10105.19-windows-setup. This install is only required if IPF specific code is being tested/developed on a fresh install or non-OEM system.

### Getting Started
Steps to install framework and execute of samples in the SDK.

### Install Framework
Using either of the methods below.

#### Manually from Windows Explorer
1. From the ipf-[version]-windows-setup package,
2. Right clicking & instal all .inf files. Listed below
 * ipf-core-[ver]/drivers/x64/ipf_acpi.inf
 * ipf-core-[ver]/drivers/x64/ipf_cpu.inf
 * ipf-ef-[ver]/drivers/x64/ipf_ef_ext.inf 
 * ipf-ef-[ver]/drivers/x64/ipf_ef_sw.inf

## Step3: download the IPF extension package
IPF extension packages need to be installed on the system for testing CPU detect it can be found on the google drive- [link](https://drive.google.com/drive/folders/1FpDT8CSGsZVlNrsqr2wFRHabYg-5eHLc), name of the file is Intel System Data IPF Extension Provider Package v02.05.1002
Note: Only Install the following 2 extension providers (optional - these are not required to run the sample code)
- Intel(R) System Config IPF extension provider
- Intel(R) CSME IPF Extension provider
If you have other extension providers installed from previous runs, pls uninstall those (PMT IPF Extension, SoC thermal Extension, Modern Standby IPF extension), these require admin command prompt run of the sample code and will cause errors if run with non-admin user privileges


Pleas follow the manual install steps noted above for all .inf files in the extension packages 

## Step 4: Download the sample code
Sample code can be cloned from https://github.com/rrwinterton/cpudetect 


## Step 5: Import into VS 2022
Import the C++ files to VC 2022, this will create a new vcxproj file (Note current code also has existing vcxproj file which can be used for VC2019)
Pls update the project file to include the header files in the include/ folder for compile
Pls update the project file to include the library files in the lib/ folder for the linking during build


## Step 6: Building the code 
Cmake file is included with the code which can be imported into VS for generating build files. These can also be run from the command prompt

1. Launch a Developer Command Prompt for Visual Studio 2019. Run `cmake -S . -B build` from unzipped folder.
2. Run`cmake --build build --config <configuration>` where configuration is `Debug` or `Release`

For Visual Studio click on 'Build' option in Toolbar and select 'Release-x64' option to build the code 

## Pre-MTL code runtime flow
Code starts in main() function, enters ADL or legacy cpudetect code 

## MTL/ARL code runtime flow
Code starts in main() function, MTL/ARL native code detection is executed first
Next part opens an ipf pointer to the IPF software module installed in Windows (requires ipf-core and ipf-ef versions)
To run the IPFSoC/NPU/IGFX detection functions pls install the IPF extension package as noted above

## Running CPU detect
 Execute cpudetect in admin command window (this is applicable for the MTL IPF pre-release version only)
  - `cd build\<configuration>`
  - `cpudetect.exe` to run the C++ sample application
