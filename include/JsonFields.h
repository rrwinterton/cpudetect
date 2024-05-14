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

namespace Ipf
{

namespace JsonFields
{

const char SCHEMA_TYPE[] = "schema_type";
const char SCHEMA_REVISION[] = "schema_revision";
const char PLATFORM_CONFIG[] = "platform_config";
const char PROVIDER_MANIFEST[] = "provider_manifest";
const char PROVIDER_CONFIG[] = "provider_config";
const char PROVIDER_CATALOG[] = "provider_catalog";
const char MODULE_PROPERTIES[] = "module_properties";
const char GET_PROPERTIES[] = "get_properties";

const char EXAMPLES[] = "examples";
const char DESCRIPTION[] = "description";
const char PROVIDER_PACKAGES[] = "provider_packages";
const char MODULES[] = "modules";
const char LOCAL_PROVIDERS[] = "local_providers";
const char INF_PROVIDERS[] = "inf_providers";
const char FILE_PROVIDERS[] = "file_providers";
const char BODY[] = "body";
const char NAMESPACE[] = "namespace";
const char INSTANCE_NAME[] = "name";
const char CLASS[] = "class";
const char UNIQUE_ID[] = "unique_id";
const char ALLOWED_UNIQUE_IDs[] = "allowed_unique_ids";
const char CHILDREN[] = "children";
const char PROPERTIES[] = "properties";
const char INIT[] = "init";
const char ATTACH[] = "attach";
const char LOCATION[] = "location";
const char OPTIONS[] = "options";
const char ENABLE_PROVIDER_DISCOVERY[] = "enable_provider_discovery";
const char OBJECT_TYPE[] = "object";
const char COMMANDS[] = "commands";
const char EVENTS[] = "events";
const char SCHEMA[] = "schema";
const char NAME[] = "name";
const char DATA_SCHEMA[] = "data_schema";
const char EVENT_SCHEMA[] = "event_schema";
const char INIT_SCHEMA[] = "init_schema";
const char COMMAND_SCHEMA[] = "command_schema";
const char RESULT_SCHEMA[] = "result_schema";
const char PROPERTIES_TYPE[] = "properties_type";
const char CATALOG[] = "catalog";
const char VERSION[] = "version";
const char AUTHOR[] = "author";
const char REVISION[] = "revision";
const char ATTACH_INSTRUCTIONS[] = "attach_instructions";
const char PROVIDERS[] = "providers";
const char NAMESPACE_CHANGED[] = "NamespaceChanged";
const char PROVIDER_INSTANCE_CREATED[] = "ProviderInstanceCreated";
const char PROVIDER_INSTANCE_DELETED[] = "ProviderInstanceDeleted";
const char PROVIDER_SCHEMA_CHANGED[] = "ProviderSchemaChanged";
const char INSTANCE[] = "instance";
const char PARTKEY[] = "PartKey";

} // namespace JsonFields
} // namespace Ipf
