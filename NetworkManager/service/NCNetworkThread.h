/**
 * Copyright @ 2013 - 2015 Suntec Software(Shanghai) Co., Ltd.
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
 * @file NCNetworkThread.h
 * @brief Declaration file of class NCNetworkThread.
 *
 * This file includes the declaration of class NCNetworkThread, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */
#ifndef NCNETWORKTHREAD_H
#define NCNETWORKTHREAD_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <ncore/NCSpThread.h>
#include "NCNetworkReq.h"

namespace nutshell
{
    /**
     * The main thread for NetworkManager
     *
     * All the request from service will be posted to this thread.
     *
     */
    class NCNetworkThread: public NCSpThread<NCNetworkReqBase>
    {
    public:
        NCNetworkThread(const NCString& threadName);
        virtual ~NCNetworkThread();

        VOID insertRequest(NCNetworkReqBase* req);
        VOID removeSimilarDhcpRequestInList(const UINT32& requestType, const NCString& deviceType, const NCString& deviceName);

        NC_BOOL checkReqList(const AccessParameter& param);
        NC_BOOL checkCurrentReq(const AccessParameter& param);

        NC_BOOL lockReqState(const UINT32 reqType);
        NC_BOOL unlockReqState(const UINT32 reqType);

    private:
        NCNetworkThread(const NCNetworkThread&);
        NCNetworkThread& operator=(const NCNetworkThread&);
    };

} /* namespace nutshell */

#endif /* NCNETWORKTHREAD_H */
/* EOF */
