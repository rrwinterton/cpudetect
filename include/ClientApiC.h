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

#include "Base.h"
#include "Events.h"
#include "stddef.h"
#include "stdint.h"

// Export Symbols only when building ClientApi Dynamic Library
#if defined(ClientApi_EXPORTS)
#define APIEXPORT DLLEXPORT
#else
#define APIEXPORT
#endif

/* The C API for IPF ClientApi is for use only by C modules ending in ".c"
 * Including this header in C++ modules ending in ".cpp" requires use of
 * extern "C" {}  when #including this file, and is not officially supported
 * for use by C++ clients. Use the ClientApi.h Native C++ API instead.
 */

typedef int BOOL;
#define TRUE 1
#define FALSE 0

/* Used for Value-To-String case statements */
#define ESIF_CASE_ENUM(e)                                                                          \
	case e:                                                                                        \
		return (char*)#e

/*-----------------------------------------------------------------------------
  IPF errors enumeration
-----------------------------------------------------------------------------*/
enum IpfError {
	IPF_ERR_OK = 0,
	IPF_ERR_UNKNOWN,
	IPF_ERR_UNEXPECTED,
	IPF_ERR_MEMORY,
	IPF_ERR_INVALIDPARAM,
	IPF_ERR_CREATE,
	IPF_ERR_GETNODE,
	IPF_ERR_GETSCHEMA,
	IPF_ERR_QUERYNODE,
	IPF_ERR_EXECCMD,
	IPF_ERR_REGISTER,
	IPF_ERR_UNREGISTER,
	IPF_ERR_SETVALUE,
	IPF_ERR_GETVALUE,
	IPF_ERR_GETKEYVAL,
	IPF_ERR_GETKEYVALS,
	IPF_ERR_ENUMKEYS,
	IPF_ERR_ENUMKEYVALS,
	IPF_ERR_VALIDATE,
	IPF_ERR_UPDATEJSON,
	IPF_ERR_BEAUTIFYJSON,
	IPF_ERR_BUFFERTOOSMALL,
	IPF_ERR_JARRAYEXPECTED,
	IPF_ERR_NOTFOUND,
};
typedef enum IpfError ipf_err_t;

/* Convert Return Code To A String */
static __inline char* ipf_ef_error_str(ipf_err_t type)
{
	switch (type) {
		ESIF_CASE_ENUM(IPF_ERR_OK);
		ESIF_CASE_ENUM(IPF_ERR_UNKNOWN);
		ESIF_CASE_ENUM(IPF_ERR_UNEXPECTED);
		ESIF_CASE_ENUM(IPF_ERR_MEMORY);
		ESIF_CASE_ENUM(IPF_ERR_INVALIDPARAM);
		ESIF_CASE_ENUM(IPF_ERR_CREATE);
		ESIF_CASE_ENUM(IPF_ERR_GETNODE);
		ESIF_CASE_ENUM(IPF_ERR_GETSCHEMA);
		ESIF_CASE_ENUM(IPF_ERR_QUERYNODE);
		ESIF_CASE_ENUM(IPF_ERR_EXECCMD);
		ESIF_CASE_ENUM(IPF_ERR_REGISTER);
		ESIF_CASE_ENUM(IPF_ERR_UNREGISTER);
		ESIF_CASE_ENUM(IPF_ERR_SETVALUE);
		ESIF_CASE_ENUM(IPF_ERR_GETVALUE);
		ESIF_CASE_ENUM(IPF_ERR_GETKEYVAL);
		ESIF_CASE_ENUM(IPF_ERR_GETKEYVALS);
		ESIF_CASE_ENUM(IPF_ERR_ENUMKEYS);
		ESIF_CASE_ENUM(IPF_ERR_ENUMKEYVALS);
		ESIF_CASE_ENUM(IPF_ERR_VALIDATE);
		ESIF_CASE_ENUM(IPF_ERR_UPDATEJSON);
		ESIF_CASE_ENUM(IPF_ERR_BEAUTIFYJSON);
		ESIF_CASE_ENUM(IPF_ERR_BUFFERTOOSMALL);
		ESIF_CASE_ENUM(IPF_ERR_JARRAYEXPECTED);
		ESIF_CASE_ENUM(IPF_ERR_NOTFOUND);
	}
	return (char*)"Unknown Error\n";
}

/*-----------------------------------------------------------------------------
  IPF schema types enumeration
	Note: The real SchemaType enum is defined as enum class in the Ipf
	namespace. These could get out of sync if the enum class ever changes.
-----------------------------------------------------------------------------*/
enum IpfSchemaType {
	IPF_SCHEMA_TYPE_NONE = 0,
	IPF_SCHEMA_TYPE_DATA = 1,
	IPF_SCHEMA_TYPE_EVENT = 2,
	IPF_SCHEMA_TYPE_COMMAND = 3,
	IPF_SCHEMA_TYPE_RESULT = 4,
	IPF_SCHEMA_TYPE_INIT = 5,
};
typedef enum IpfSchemaType ipf_schema_t;

/******************************************************************************
  IPF Client APIs
******************************************************************************/

/*-----------------------------------------------------------------------------
  Create an instance of the IPF ClientApi and get a pointer to it
	config - IPF client configuration in json
	pIpf - pointer to IPF ClientApi instance
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t IpfCreate(const char* config, void** pIpf);

/*-----------------------------------------------------------------------------
  Destroy the IPF ClientApi instance created by IpfCreate()
	config - IPF client configuration in json
	pIpf - pointer to IPF ClientApi instance
-----------------------------------------------------------------------------*/
APIEXPORT void IpfDestroy(void* pIpf);

/*-----------------------------------------------------------------------------
  Get Properties
	pIpf - pointer to IPF ClientApi instance
	properties_config - properties config
	buf - pointer to buffer where the returned json data will be copied into
	len - pointer to the buffer size, and the json data size on return.
		  If buffer is not large enough, len will be set to the requred size.
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t
IpfGetProperties(void* pIpf, const char* properties_config, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Get a node
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired node
	buf - pointer to buffer where the returned json data will be copied into
	len - pointer to the buffer size, and the json data size on return.
		  If buffer is not large enough, len will be set to the requred size.
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t IpfGetNode(void* pIpf, const char* path, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Get a value
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired property
	buf - pointer to buffer where the returned json value will be copied into
	len - pointer to the buffer size, and the json value size on return.
		  If buffer is not large enough, len will be set to the requred size.
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t IpfGetValue(void* pIpf, const char* path, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Get schema
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired node
	schema_type - schema type
	buf - pointer to buffer where the returned json schema will be copied into
	len - pointer to the buffer size, and the json schema size on return.
		  If buffer is not large enough, len will be set to the requred size.
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t
IpfGetSchema(void* pIpf, const char* path, ipf_schema_t schema_type, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Query a node including its properties and child nodes
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired node
	buf - pointer to buffer where the returned json data will be copied into
	len - pointer to the buffer size, and the json data size on return.
		  If buffer is not large enough, len will be set to the requred size.
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t IpfQueryNode(void* pIpf, const char* path, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Execute a command on a node
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired node
	command - command in json
	buf - pointer to buffer where the returned json result will be copied into
	len - pointer to the buffer size, and the json result size on return.
		  If buffer is not large enough, len will be set to the requred size.
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t
IpfExecuteCommand(void* pIpf, const char* path, const char* command, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Register for data events from provider
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired node
	cb - event callback function
	context - context (user data) for the callback function
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t IpfRegisterEvent(void* pIpf, const char* path, EventCallback cb, void* context);

/*-----------------------------------------------------------------------------
  Unregister for data events from provider
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired node
	cb - event callback function
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t IpfUnregisterEvent(void* pIpf, const char* path, EventCallback cb);

/*-----------------------------------------------------------------------------
  Set a boolean value to a node property
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired node property
	value - a boolean value
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t IpfSetValue_bool(void* pIpf, const char* path, BOOL value);

/*-----------------------------------------------------------------------------
  Set an integer value to a node property
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired node property
	value - an integer value
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t IpfSetValue_int(void* pIpf, const char* path, int64_t value);

/*-----------------------------------------------------------------------------
  Set a double value to a node property
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired node property
	value - a float value
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t IpfSetValue_double(void* pIpf, const char* path, double value);

/*-----------------------------------------------------------------------------
  Set a string value to a node property
	pIpf - pointer to IPF ClientApi instance
	path - the path to the desired node property
	value - a string value
-----------------------------------------------------------------------------*/
APIEXPORT ipf_err_t IpfSetValue_string(void* pIpf, const char* path, const char* value);

/*-----------------------------------------------------------------------------
  Get IPF last error message (thread-safe)
-----------------------------------------------------------------------------*/
APIEXPORT const char* IpfGetLastErrorMessage();

/*-----------------------------------------------------------------------------
  Get IPF last exception type (thread-safe)
-----------------------------------------------------------------------------*/
APIEXPORT const char* IpfGetLastExceptionType();
