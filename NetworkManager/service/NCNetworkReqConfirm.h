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
 * @file NCNetworkReqConfirm.h
 * @brief Declaration file of class NCNetworkReqConfirm.
 *
 * This file includes the declaration of class NCNetworkReqConfirm, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKREQCONFIRM_H
#define NCNETWORKREQCONFIRM_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <ncore/NCString.h>
#include <ncore/NCSpThread.h>
#include "NCNetworkReq.h"

namespace nutshell
{

    /**
     * @brief Class of NCNetworkReqConfirm.
     *
     * This class is used confirm the request in work thread
     */
    class NCNetworkReqConfirm
    {
    public:

        NCNetworkReqConfirm();
        virtual ~NCNetworkReqConfirm();

        VOID confirm();

        NCString getDeviceType(const UINT32 reqType);
        NCString getDeviceName(const UINT32 reqType);
        UINT32 getLastRequest(const UINT32 reqType);

        NC_BOOL getRemoteFlag(const UINT32 reqType);

        UINT32 getReqState(const UINT32 reqType);
        VOID setReqState(const UINT32 reqType, const UINT32 state);

        VOID lockObject(const UINT32 reqType);
        VOID unlockObject(const UINT32 reqType);

    private:
        NCNetworkReqConfirm(const NCNetworkReqConfirm&);
        NCNetworkReqConfirm& operator=(const NCNetworkReqConfirm&);
    };

} /* namespace nutshell */

#endif /* NCNETWORKREQCONFIRM_H */
/* EOF */
