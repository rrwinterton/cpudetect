/*******************_************************************************************
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
*******************************************************************************/

#pragma once

// IPF Extensible Framework SDK Version
#define IPFSDK_VER_MAJOR 2
#define IPFSDK_VER_MINOR 1
#define IPFSDK_VER_REVISION 10105
#define IPFSDK_VER_BUILD 21

#define IPFEF_SDK_VERSION "2.1.10105.21"

//
// OS Base Type Definitions
//
#if defined(_WIN32)
#define IPF_OS_TYPE "Windows"
#define IPF_PLATFORM_TYPE "x64" // x86 not supported
#define IPF_INLINE __inline
#define DLLEXPORT __declspec(dllexport)
#define DLLCALLCONV __cdecl
#define __FUNC__ __FUNCTION__
#define UNREFERENCED_PARAMETER(x) (x)
#else
#define IPF_OS_TYPE "Linux"
#define IPF_PLATFORM_TYPE "x64" // x86 not supported
#define IPF_INLINE inline
#define DLLEXPORT __attribute__((visibility("default")))
#define DLLCALLCONV
#define __FUNC__ __PRETTY_FUNCTION__
#define UNREFERENCED_PARAMETER(x) (void)(x)
#endif

// NDEBUG is a C-standard cross-OS compiler setting used to disable C-assertions.
// It has nothing to do with _DEBUG, which is Windows-specific and automatically
// set when using the C/C++ Debug Runtime Library options /MDd or /MTd
#if !defined(NDEBUG)
#define IPF_BUILD_DEBUG
#endif

#ifdef IPF_BUILD_DEBUG
#define IPF_BUILD_TYPE "Debug"
#else
#define IPF_BUILD_TYPE "Release"
#endif

// Macro for statements that are executed only in Debug builds like std::cout streams.
// Enable globally with -DENABLE_DEBUGSTMT=true cmake flag or CMakeLists.txt option.
// Individual modules can #define ENABLE_DEBUGSTMT or DISABLE_DEBUGSTMT before any other
// #includes to override global behavior and allow or suppress debug statement output.
// Individual statements can be forced or suppressed with DO_DEBUGSTMT and NO_DEBUGSTMT.
// Example: DEBUGMSG(std::cout << "Some Message" << std::endl);
#if defined(IPF_BUILD_DEBUG) && defined(ENABLE_DEBUGSTMT) && !defined(DISABLE_DEBUGSTMT)
#define DEBUGSTMT(stmt) stmt
#else
#define DEBUGSTMT(stmt) (void)(0)
#endif
#define DO_DEBUGSTMT(stmt) stmt
#define NO_DEBUGSTMT(stmt) (void)(0)

// Opaque Base Types
#include <stdint.h>
typedef uint32_t revision_t; // Interface Revision Number Type
