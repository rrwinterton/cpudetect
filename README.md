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
This document covers the setup, build and runtime process for the cpu detect functionality. Documentation can be found on google drive [link](https://drive.google.com/drive/folders/1FpDT8CSGsZVlNrsqr2wFRHabYg-5eHLc)
Note: This is the Windows API based information for MTL release

## Step 1: Download the sample code
Sample code can be cloned from https://github.com/rrwinterton/cpudetect 


## Step 2: Import into VS 2022
Import the C++ files to VC 2022, this will create a new vcxproj file (Note current code also has existing vcxproj file which can be used for VC2019)
Pls update the project file to include the header files in the include/ folder for compile
Pls update the project file to include the library files in the lib/ folder for the linking during build


## Step 3: Building the code 
Cmake file is included with the code which can be imported into VS for generating build files. These can also be run from the command prompt

1. Launch a Developer Command Prompt for Visual Studio 2019. Run `cmake -S . -B build` from unzipped folder.
2. Run`cmake --build build --config <configuration>` where configuration is `Debug` or `Release`

For Visual Studio click on 'Build' option in Toolbar and select 'Release-x64' option to build the code 

## Pre-MTL code runtime flow
Code starts in main() function, enters ADL or legacy cpudetect code 
Updated to include memory information and GPU driver version

## MTL/ARL code runtime flow
Code starts in main() function, MTL/ARL native code detection is executed first
Updated to include memory information and GPU, NPU driver version


## Running CPU detect
 Execute cpudetect 
  - `cd build\<configuration>`
  - `cpudetect.exe` to run the C++ sample application
