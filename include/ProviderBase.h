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

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <stdint.h>
#include <string>

#include "Events.h"
#include "IFramework.h"
#include "IProvider.h"
#include "IProviderMarshaled.h"
#include "MarshalError.h"
#include "ProviderDelegate.h"
#include "SchemaType.h"
#include "nlohmann/json.hpp"

namespace Ipf
{
class ProviderDelegate;
} // namespace Ipf

using Json = nlohmann::json;

namespace Ipf
{

//
// Base class for Provider implementations, implementing IProvider
//
class ProviderBase : public IProviderMarshaled
{
public:
	explicit ProviderBase(std::unique_ptr<ProviderDelegate> delegate);
	virtual ~ProviderBase();

	virtual uint32_t AddRef();
	virtual uint32_t Release();

	virtual void Start();

	virtual std::string GetProperties(const std::string& properties_config = "") final;

	virtual std::string GetNode(const std::string& path = "") final;
	virtual std::string GetSchema(const std::string& path, SchemaType schema_type) final;
	virtual std::string QueryNode(const std::string& path = "") final;
	virtual std::string ExecuteCommand(const std::string& path, const std::string& command) final;

	virtual void SetValue(const std::string& path, bool value) final;
	virtual void SetValue(const std::string& path, int64_t value) final;
	virtual void SetValue(const std::string& path, double value) final;
	virtual void SetValue(const std::string& path, const std::string& value) final;
	virtual void SetValue(const std::string& path, const char* value) final;

	// Functions defined in IProviderMarshaled
	virtual ipf_marshal_err_t GetProperties(
		const std::string& properties_config,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen) final;

	virtual ipf_marshal_err_t GetNode(
		const std::string& path,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen) final;

	virtual ipf_marshal_err_t GetSchema(
		const std::string& path,
		const SchemaType schema_type,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen) final;

	virtual ipf_marshal_err_t QueryNode(
		const std::string& path,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen) final;

	virtual ipf_marshal_err_t ExecuteCommand(
		const std::string& path,
		const std::string& command,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen) final;

protected:
	virtual void RegisterEvent(const std::string& path, EventCallback cb, void* context = nullptr);
	virtual void UnregisterEvent(const std::string& path, EventCallback cb);

private:
	std::unique_ptr<ProviderDelegate> delegate_;
	std::atomic_uint refcount_;
	void ValidateWithSchema(SchemaType schema_type, const std::string& j);
	void ValidateWithSchema(SchemaType schema_type, const Json& j);
	bool IsValidEvent(const std::string& path);

	Json GetNodeForPath(const std::string& path, const Json& ns);

	ipf_marshal_err_t CallAndCopyResult(
		const std::function<std::string()>& f,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen,
		const std::string& path = "",
		const std::string& command = "",
		SchemaType schema_type = SchemaType::DATA);
};

} // namespace Ipf
