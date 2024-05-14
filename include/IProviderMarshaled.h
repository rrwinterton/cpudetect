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
#include "Events.h"
#include "IProvider.h"
#include "MarshalError.h"
#include "SchemaType.h"

//
// IProvider defines the interface that all Providers must expose.
//

namespace Ipf
{

class IProviderMarshaled : public IProvider
{
public:
	virtual ipf_marshal_err_t GetProperties(
		const std::string& properties_config,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen) = 0;

	virtual ipf_marshal_err_t GetNode(
		const std::string& path,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen) = 0;

	virtual ipf_marshal_err_t GetSchema(
		const std::string& path,
		const SchemaType schema_type,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen) = 0;

	virtual ipf_marshal_err_t QueryNode(
		const std::string& path,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen) = 0;

	virtual ipf_marshal_err_t ExecuteCommand(
		const std::string& path,
		const std::string& command,
		char* databuf,
		uint32_t& datalen,
		char* errbuf,
		uint32_t& errlen) = 0;
};

} // namespace Ipf
