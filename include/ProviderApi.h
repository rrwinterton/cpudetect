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

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "IProviderApi.h"
#include "MarshalError.h"
#include "ProviderBase.h"
#include "ProviderDelegate.h"

namespace Ipf
{

using ProviderMap =
	std::map<std::string, std::function<Ipf::IProvider*(Ipf::IFramework*, const std::string&)>>;

class ProviderApi : public IProviderApi
{
public:
	ProviderApi(const std::string& manifest = "", const ProviderMap& factory = ProviderMap());
	virtual ~ProviderApi();

	ipf_marshal_err_t GetManifest(char* databuf, uint32_t& datalen) override;

	Ipf::IProvider* CreateInstance(
		Ipf::IFramework* pfw,
		const std::string& provider_class = "",
		const std::string& config = "{}") override;

	using Creator = ProviderApi* (*)(const std::string& config);
	using Destroyer = void (*)(ProviderApi*);
	using Revisioner = revision_t (*)();

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

// Template used to define ProviderMaps that are passed into ProviderApi constructor
template <typename T>
Ipf::IProvider* ProviderMapEntry(Ipf::IFramework* pfw, const std::string& config)
{
	T* provider = new T(pfw, config);
	if (provider) {
		// If this provider is derived from IProvider, we can return it directly
		// This will be true for FrameworkProvider and MarshaledProviderAdapter.
		// Our core or base providers may also derive directly from IProvider.
		if (dynamic_cast<IProvider*>(provider)) {
			return dynamic_cast<IProvider*>(provider);
		}

		// If the provider derives from ProviderDelegate, wrap it in a unique ptr and
		// create an instance of ProviderBase to manage it.
		// Providers developed with the SDK are defined this way.
		if (dynamic_cast<ProviderDelegate*>(provider)) {
			std::unique_ptr<ProviderDelegate> delegate(dynamic_cast<ProviderDelegate*>(provider));
			return dynamic_cast<IProvider*>(new ProviderBase(std::move(delegate)));
		}

		// Provider is not null, but doesn't derive from one of the known parent classes!
		delete provider;
	}
	return nullptr;
}

} // namespace Ipf

// Macro to define standard DLL exports for a ProviderApi object
#define EXPORT_PROVIDER(CLASSNAME)                                                                 \
	extern "C"                                                                                     \
	{                                                                                              \
		DLLEXPORT Ipf::ProviderApi* ProviderApiCreate()                                            \
		{                                                                                          \
			return new CLASSNAME;                                                                  \
		}                                                                                          \
	}
