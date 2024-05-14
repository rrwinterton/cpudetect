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

#include "ClientApiC.h"

/* The C API for IPF JSON ClientApi is for use only by C modules ending in ".c"
 * Including this header in C++ modules ending in ".cpp" requires use of
 * extern "C" {}  when #including this file, and is not officially supported
 * for use by C++ clients. Use the ClientApi.h Native C++ API instead.
 */

/*-----------------------------------------------------------------------------
	Verify if a string is in json format
	json - the string to verify
-----------------------------------------------------------------------------*/
BOOL JsonVerify(const char* json);

/*-----------------------------------------------------------------------------
	Get the elements count in a json string
	json - the json string
-----------------------------------------------------------------------------*/
size_t JsonGetElementsCount(const char* json);

/*-----------------------------------------------------------------------------
	Check if an json string contains a key
	json - the json string
	key - the key path in the form of key1.key2.key3...
-----------------------------------------------------------------------------*/
BOOL JsonContainsKey(const char* json, const char* key);

/*-----------------------------------------------------------------------------
	Get the raw value of an json element of json[key]
	json - the json string
	key - the key path of the element in the form of key1.key2.key3...
	buf - pointer to buffer where the returned raw json value will be copied into
	len - pointer to the buffer size, and the json value size on return.
			If buffer is not large enough, len will be set to the requred size.
-----------------------------------------------------------------------------*/
ipf_err_t JsonGetKeyValue(const char* json, const char* key, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
	Get a boolean value of an json element json[key]
	json - the json string
	key - the key path of the element in the form of key1.key2.key3...
	buf - pointer to the returned boolean value
-----------------------------------------------------------------------------*/
ipf_err_t JsonGetKeyValue_bool(const char* json, const char* key, BOOL* buf);

/*-----------------------------------------------------------------------------
	Get a boolean value of a raw json property
	json - the raw json property
	buf - pointer to the returned boolean value
-----------------------------------------------------------------------------*/
ipf_err_t JsonGetValue_bool(const char* json, BOOL* buf);

/*-----------------------------------------------------------------------------
	Get an integer value of an json element json[key]
	json - the json string
	key - the key path of the element in the form of key1.key2.key3...
	buf - pointer to the returned integer value
-----------------------------------------------------------------------------*/
ipf_err_t JsonGetKeyValue_int(const char* json, const char* key, int* buf);

/*-----------------------------------------------------------------------------
	Get an integer value of a raw json property
	json - the raw json property
	buf - pointer to the returned integer value
-----------------------------------------------------------------------------*/
ipf_err_t JsonGetValue_int(const char* json, int* buf);

/*-----------------------------------------------------------------------------
	Get a double value of an json element json[key]
	json - the json string
	key - the key path of the element in the form of key1.key2.key3...
	buf - pointer to the returned double value
-----------------------------------------------------------------------------*/
ipf_err_t JsonGetKeyValue_double(const char* json, const char* key, double* buf);

/*-----------------------------------------------------------------------------
	Get a double value of a raw json property
	json - the raw json property
	buf - pointer to the returned double value
-----------------------------------------------------------------------------*/
ipf_err_t JsonGetValue_double(const char* json, double* buf);

/*-----------------------------------------------------------------------------
	Get a string value of an json element json[key]
	json - the json string
	key - the key path of the element in the form of key1.key2.key3...
	buf - pointer to the returned string value
-----------------------------------------------------------------------------*/
ipf_err_t JsonGetKeyValue_string(const char* json, const char* key, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
	Get a string value of a raw json property
	json - the raw json property
	buf - pointer to the returned string value
-----------------------------------------------------------------------------*/
ipf_err_t JsonGetValue_string(const char* json, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
	Enumerate keys of the objects in an json string
	json - the json string
	keys - the returned array of key names
			keys[len][MAX_VALUE_BUFFER_CHARS]
	len - pointer to the array size, and the array elements count on return.
			If the 1st dimension of the array buffer is not large enough,
			len will be set to the requred size.
-----------------------------------------------------------------------------*/
ipf_err_t JsonEnumerateKeys(const char* json, char** keys, size_t* len);

/*-----------------------------------------------------------------------------
	Enumerate key/values of an json object
	json - the json object string
	keys - the returned array of key names
			keys[len][MAX_VALUE_BUFFER_CHARS]
	buf - the returned array of raw json values
			buf[len][MAX_VALUE_BUFFER_CHARS]
	len - pointer to the array size, and the array elements count on return.
			If the 1st dimension of the arrays is not large enough,
			len will be set to the requred size.
-----------------------------------------------------------------------------*/
ipf_err_t JsonEnumerateKeyValues(const char* json, char** keys, char** buf, size_t* len);

/*-----------------------------------------------------------------------------
	Get the raw json property values of an array of elements in json
	json - the json string
	count - the number of properties in element
	keys - the property names in element
	buf - the returned raw property values in the form of a 3-D string array:
			buf[array_size][props_count][MAX_VALUE_BUFFER_CHARS]
	len - pointer to the array size, and the array elements count on return.
			If the 1st dimension of the array buffer is not large enough,
			len will be set to the requred size.
-----------------------------------------------------------------------------*/
ipf_err_t
JsonGetArrayKeyValues(const char* json, size_t count, const char* keys[], char*** buf, size_t* len);

/*-----------------------------------------------------------------------------
	Update an element in the json string: json[key]=json2
	json - the json string
	key - the key path of the element in the form of key1.key2.key3...
	json2 - the new value for the element
	buf - pointer to buffer where the updated json string will be copied into
	len - pointer to the buffer size, and the new json string size on return.
			If buffer is not large enough, len will be set to the requred size.
-----------------------------------------------------------------------------*/
ipf_err_t JsonUpdate(const char* json, const char* key, const char* json2, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
	Format an json string nicely with identantation
	json - the json string
	indent - number of empty spaces for the identantation
	buf - pointer to buffer where the formatted json will be copied into
	len - pointer to the buffer size, and the formatted json size on return.
			If buffer is not large enough, len will be set to the requred size.
-----------------------------------------------------------------------------*/
ipf_err_t JsonBeautify(const char* json, int indent, char* buf, size_t* len);
