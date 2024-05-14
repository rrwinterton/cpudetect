/*******************************************************************************
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

// Event callback declaration
//
// The context pointer is stored when an event is registered and is
// passed to the EventCallback when an event is raised.  It may be
// used to store an object's "this" pointer or other context.
//
// Each callback will be called on a separate thread.
// The implementer of the EventCallback must make sure that it is
// thread-safe.
typedef void (*EventCallback)(const char* event_name, const char* event_data, void* context);
