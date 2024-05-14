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
#include <string>

namespace Ipf
{

enum SchemaType { NONE = 0, DATA = 1, EVENT = 2, COMMAND = 3, RESULT = 4, INIT = 5 };

// Helper to make it easy to iterate over the enum and get string representation
const std::map<SchemaType, std::string> AllSchemaTypes{
	{SchemaType::DATA, "DATA"},
	{SchemaType::EVENT, "EVENT"},
	{SchemaType::COMMAND, "COMMAND"},
	{SchemaType::RESULT, "RESULT"},
	{SchemaType::INIT, "INIT"}};
} // namespace Ipf
