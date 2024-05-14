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

#include <stdint.h>
#include <memory>
#include <string>

#include "Base.h"
#include "Events.h"
#include "Exceptions.h"
#include "SchemaType.h"

// Export Symbols only when building ClientApi Dynamic Library
#if defined(ClientApi_EXPORTS)
#define APIEXPORT DLLEXPORT
#else
#define APIEXPORT
#endif

namespace Ipf
{

class ClientApi
{
public:
	APIEXPORT ClientApi();
	APIEXPORT ClientApi(const std::string& config);
	APIEXPORT virtual ~ClientApi();

	APIEXPORT std::string GetProperties(const std::string& properties_config = "");

	APIEXPORT std::string GetValue(const std::string& path = "");
	APIEXPORT std::string GetNode(const std::string& path = "");
	APIEXPORT std::string GetSchema(const std::string& path, SchemaType schema_type);
	APIEXPORT std::string ExecuteCommand(const std::string& path, const std::string& command);
	APIEXPORT std::string QueryNode(const std::string& path);

	APIEXPORT void
	RegisterEvent(const std::string& path, EventCallback cb, void* context = nullptr);
	APIEXPORT void UnregisterEvent(const std::string& path, EventCallback cb);

	APIEXPORT void SetValue(const std::string& path, bool value);
	APIEXPORT void SetValue(const std::string& path, int value);
	APIEXPORT void SetValue(const std::string& path, int64_t value);
	APIEXPORT void SetValue(const std::string& path, double value);
	APIEXPORT void SetValue(const std::string& path, const std::string& value);
	APIEXPORT void SetValue(const std::string& path, const char* value);

private:
	class Impl;

	std::unique_ptr<Impl> pImpl;
};

} // namespace Ipf
