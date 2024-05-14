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

//
// Significant revisions that are checked to enable / disable features
//

// ProviderApi Rev3 v2.0.10100 release - initial revision
// ClientApi Rev3 v2.0.10100 release - initial revision
#define PROVIDERAPI_REVISION_REL_2_0_101_00 3
#define CLIENTAPI_REVISION_REL_2_0_101_00 3

// ProviderApi Rev4 v2.1.10000 release - added Start() method to IProvider
#define PROVIDERAPI_REVISION_REL_2_1_100_00 4
#define CLIENTAPI_REVISION_REL_2_1_100_00 3

// ProviderApi Rev4 v2.1.10100 release - added Start() method to IProvider
#define PROVIDERAPI_REVISION_REL_2_1_101_00 4
#define CLIENTAPI_REVISION_REL_2_1_101_00 3


#define PROVIDERAPI_CURRENT_REVISION PROVIDERAPI_REVISION_REL_2_1_101_00
#define CLIENTAPI_CURRENT_REVISION CLIENTAPI_REVISION_REL_2_1_101_00
