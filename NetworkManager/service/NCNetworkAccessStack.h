/**
 * Copyright @ 2013 - 2014 Suntec Software(Shanghai) Co., Ltd.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * Suntec Software(Shanghai) Co., Ltd.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
/**
 * @file NCNetworkAccessStack.h
 * @brief Declaration file of class NCNetworkAccessStack.
 *
 * This file includes the declaration of class NCNetworkAccessStack, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKACCESSSTACK_H
#define NCNETWORKACCESSSTACK_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <map>
#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCSyncObj.h>
#include "NCNetworkManagerDef.h"

namespace nutshell
{

    /**
     * @brief Class of NCNetworkAccessStack.
     *
     * This class is used to store the intermediate data during request or release network access.
     */
    class NCNetworkAccessStack
    {
    public:
        NCNetworkAccessStack();
        virtual ~NCNetworkAccessStack();

        VOID addRequestMap(const UINT32 mapType, const UINT32 seqId, const AccessParameter& param);
        NC_BOOL getRequestMap(const UINT32 mapType, const UINT32 seqId, AccessParameter& param);
        VOID rmRequestMap(const UINT32 mapType, const UINT32 seqId);
        NC_BOOL isRequestMapEmpty(const UINT32 mapType);

        NC_BOOL popRequestMap(const UINT32 mapType, AccessParameter& param);
        NC_BOOL popRequestMap(const UINT32 mapType, const UINT32 reqType, const NCString& deviceType, AccessParameter& param);
        
        NC_BOOL checkRequstType(const UINT32 mapType, const UINT32 reqType);
        NC_BOOL checkRequestSuccess(const UINT32 mapType, const UINT32 reqType);
        NC_BOOL checkRequestTimeout(const UINT32 mapType, AccessParameter& param);
        
        VOID updateCurrRouteInfo(const DeviceRouteDnsInfo& info);
        VOID getCurrRouteInfo(DeviceRouteDnsInfo& info);

        VOID addResourceId(const NCString& resourceId, const NCString& deviceType);
        VOID getResourceId(const NCString& resourceId, NCString& deviceType);
        NC_BOOL findResourceId(const NCString& resourceId);

    private:
        std::map<UINT32, AccessParameter> m_initMap;
        std::map<UINT32, AccessParameter> m_callbackMap;

        DeviceRouteDnsInfo m_currRouteInfo;
        NCSyncObj m_syncCurrRouteInfo;

        std::map<NCString, NCString> m_resourceIdMap;
        NCSyncObj m_syncResourceIdMap;

        NC_BOOL m_timerFlag;

        NC_DISABLE_COPY(NCNetworkAccessStack);
    };

} /* namespace nutshell */

#endif /* NCNETWORKACCESSSTACK_H */
/* EOF */
