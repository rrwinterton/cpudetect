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

#include <map>
#include <stdint.h>
#include <string>

#include "Events.h"
#include "IFramework.h"
#include "SchemaType.h"
#include "nlohmann/json.hpp"

namespace Ipf
{
class IFramework;
} // namespace Ipf

using Json = nlohmann::json;

namespace Ipf
{

using SchemaMap = std::map<SchemaType, std::string>;

class ProviderDelegate
{
public:
	ProviderDelegate(
		IFramework* pfw, const SchemaMap& schema_map, const std::string& config = "{}");
	virtual ~ProviderDelegate();

	// Providers implement Start() to register callbacks or other operations
	// that can only be done safely after the the object is fully constructed
	virtual void Start();

	// Providers may call UpdateSchema to change a schema after initialization.
	// It may do this in response to some lower level resource availability
	void UpdateSchema(SchemaType schema_type, const std::string& schema);

	// ProviderBase will call UpdateNamespace in the ProviderDelegate when a
	// client requests data using GetNode() or GetValue().
	//
	// Derived providers should implement UpdateNamespace if they have properties in their namespace
	// that get updated during the lifetime of the provider.  If the derived provider does not
	// implement UpdateNamespace(), it can still update its namespace asynychonously, but it
	// won't respond synchronously to GetNode() or GetValue() calls.
	virtual void UpdateNamespace(const std::string& path = "");

	// Providers MUST implement SetValue if and only if they have any writeable parameters
	virtual void SetValue(const std::string& path, bool value);
	virtual void SetValue(const std::string& path, int64_t value);
	virtual void SetValue(const std::string& path, double value);
	virtual void SetValue(const std::string& path, const std::string& value);
	virtual void SetValue(const std::string& path, const char* value);

	// Derived providers MUST implement ExecuteCommand if and only if they handle commands
	virtual std::string
	ExecuteCommand(const std::string& path = "", const std::string& command = "{}");

	// Derived providers MAY override RegisterEvent and UnregisterEvent.
	// These methods will be called when a client registers for an event.
	virtual void RegisterEvent(const std::string& path);
	virtual void UnregisterEvent(const std::string& path);

	// Accessors used by ProviderBase
	IFramework* GetFramework();
	Json& GetNamespace();
	Json& GetConfig();
	Json& GetSchema(SchemaType schema_type);

protected:
	IFramework* framework_;
	Json namespace_;
	Json config_;

private:
	std::map<SchemaType, Json> schema_map_;
};

} // namespace Ipf