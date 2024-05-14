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

#include "IProvider.h"
#include <string>

namespace Ipf
{

// IFramework derives from IProvider, so classes implementing
// IFramework must implment all the IProvider methods as well
class IFramework : public IProvider
{
public:
	// Events are handled by ProviderProxy if they are of PROVIDER type.
	// They are handled by the FrameworkProvider if they are of FRAMEWORK type.
	enum class EventType { FRAMEWORK, PROVIDER };

	virtual std::string GetProperties(const std::string& properties_config = "") = 0;

	virtual void
	RaiseEvent(const std::string& event_data, const EventType event_type = EventType::PROVIDER) = 0;

	virtual void CreateChildInstance(
		const std::string& provider_class,
		const std::string& instance_name,
		const std::string& config = "{}") = 0;
	virtual void DeleteChildInstance(const std::string& instance_name) = 0;

	// Utility methods
	virtual std::string GetProviderClass() = 0;
	virtual std::string GetParentPath() = 0;
	virtual std::string GetInstanceName() = 0;
	virtual std::string GetFullPath() = 0;
};

} // namespace Ipf
