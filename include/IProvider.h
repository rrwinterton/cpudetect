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

#include "Events.h"
#include "SchemaType.h"

//
// IProvider defines the interface that all Providers must expose.
//

namespace Ipf
{

class IProvider
{
public:
	// Classes derived from IProvider are destroyed through calling Release().
	//
	// AddRef() and Release() maintain a reference count and will
	// delete the object when the reference count reaches zero.
	//
	// This pattern ensures that the provider is freed from the DLL that created it.
	// There is no virtual destructor in this class for these reasons.

	virtual uint32_t AddRef() = 0;
	virtual uint32_t Release() = 0;

	virtual std::string GetProperties(const std::string& properties_config = "") = 0;

	virtual std::string GetNode(const std::string& path) = 0;

	virtual std::string GetSchema(const std::string& path, SchemaType schema_type) = 0;

	virtual std::string QueryNode(const std::string& path) = 0;

	virtual std::string ExecuteCommand(const std::string& path, const std::string& command) = 0;

	virtual void SetValue(const std::string& path, bool value) = 0;
	virtual void SetValue(const std::string& path, int64_t value) = 0;
	virtual void SetValue(const std::string& path, double value) = 0;
	virtual void SetValue(const std::string& path, const std::string& value) = 0;
	virtual void SetValue(const std::string& path, const char* value) = 0;

	virtual void
	RegisterEvent(const std::string& path, EventCallback cb, void* context = nullptr) = 0;
	virtual void UnregisterEvent(const std::string& path, EventCallback cb) = 0;

	// Added in Revision 4
	virtual void Start() = 0;
};

} // namespace Ipf
