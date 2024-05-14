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

#include <string>

#include "Base.h"
#include "IFramework.h"
#include "IProvider.h"
#include "MarshalError.h"
#include "Revision.h"

class IProviderApi
{
public:
	virtual ~IProviderApi() = default;

	static const revision_t revision = PROVIDERAPI_CURRENT_REVISION;

	virtual ipf_marshal_err_t GetManifest(char* databuf, uint32_t& datalen) = 0;

	virtual Ipf::IProvider* CreateInstance(
		Ipf::IFramework* pfw,
		const std::string& provider_class = "",
		const std::string& config = "{}") = 0;
};
