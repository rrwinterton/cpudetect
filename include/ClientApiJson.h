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

/*
 * This header is a self-contained JSON-object implementation of Ipf::ClientApi
 * that may be used by client applications so that they can directly work with
 * JSON objects instead of JSON strings returned by the Ipf::ClientApi interface
 *
 * The C++ implementation uses nlohmann::json, but other languages will use
 * other JSON libraries. This header can be customized by developers to use
 * a different JSON library if they choose, and link with the ClientApi
 * static or dynamic library.
 */

#include "ClientApi.h"
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

namespace Ipf
{

class ClientApiJson
{
public:
	ClientApiJson() : ipf()
	{}
	ClientApiJson(const Json& config) : ipf(config.dump())
	{}
	virtual ~ClientApiJson()
	{}

	Json GetProperties(const std::string& properties_config = "")
	{
		return Json::parse(ipf.GetProperties(properties_config));
	}

	Json GetValue(const std::string& path = "")
	{
		return Json::parse(ipf.GetValue(path));
	}
	Json GetNode(const std::string& path = "")
	{
		return Json::parse(ipf.GetNode(path));
	}
	Json GetSchema(const std::string& path, SchemaType schema_type)
	{
		return Json::parse(ipf.GetSchema(path, schema_type));
	}
	Json ExecuteCommand(const std::string& path, const Json& command)
	{
		return Json::parse(ipf.ExecuteCommand(path, command.dump()));
	}
	Json QueryNode(const std::string& path)
	{
		return Json::parse(ipf.QueryNode(path));
	}
	void RegisterEvent(const std::string& path, EventCallback cb, void* context = nullptr)
	{
		ipf.RegisterEvent(path, cb, context);
	}
	void UnregisterEvent(const std::string& path, EventCallback cb)
	{
		ipf.UnregisterEvent(path, cb);
	}

	void SetValue(const std::string& path, bool value)
	{
		ipf.SetValue(path, value);
	}
	void SetValue(const std::string& path, int value)
	{
		ipf.SetValue(path, static_cast<int64_t>(value));
	}
	void SetValue(const std::string& path, int64_t value)
	{
		ipf.SetValue(path, value);
	}
	void SetValue(const std::string& path, double value)
	{
		ipf.SetValue(path, value);
	}
	void SetValue(const std::string& path, const std::string& value)
	{
		ipf.SetValue(path, value);
	}
	void SetValue(const std::string& path, const char* value)
	{
		ipf.SetValue(path, value);
	}

private:
	ClientApi ipf;
};

} // namespace Ipf
