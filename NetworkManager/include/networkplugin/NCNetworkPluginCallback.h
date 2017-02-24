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
 * @file NCNetworkPluginCallback.h
 * @brief Declaration file of class NCNetworkPluginCallback.
 *
 * This file includes the declaration of class NCNetworkPluginCallback, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKPLUGINCALLBACK_H
#define NCNETWORKPLUGINCALLBACK_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCRefBase.h>
#include <ncore/NCGlobalAPI.h>
#include "networkmanager/NCNetworkDef.h"

namespace nutshell
{
    /**
     * This is the callback class of network plug-in.
     *
     * This class is implemented by NCNetworkPluginCallbackImpl.
    */
    class NUTSHELL_API NCNetworkPluginCallback : public NCRefBase
    {
    public:

        NCNetworkPluginCallback() {}
        virtual ~NCNetworkPluginCallback() {}

        /**
         * This interface will be deleated later.
         */
        virtual VOID notifyRequestAccessResult(UINT32 result, const NCString& deviceType) {}

        /**
         * Notify the result of request network access.
         *
         * @param [IN] info : The result of request access.
         *
         * @return None
         *
         * @attention Asynchronous I/F.
         */
        virtual VOID notifyRequestAccessResult(const RequestPluginResult& info) {}

        /**
         * This interface will be deleated later
         */
        virtual VOID notifyReleaseAccessResult(UINT32 result, const NCString& deviceType) {}
        
        /**
         * Notify the result of disconnect pppd
         *
         * @param [IN] result : The result of disconnect pppd
         * @param [IN] deviceType: The device type
         * @param [IN] seqId : The sequence ID
         *
         * @return None
         *
         * @attention Asynchronous I/F.
         */
        virtual VOID notifyReleaseAccessResult(const UINT32 result, const NCString& deviceType, const UINT32 seqId) {}

        /**
         * Notify the result of update network access.
         *
         * @param [IN] result : The result of update network access
         * @param [IN] seqId : The sequence ID
         *
         * @return None
         *
         * @attention Asynchronous I/F.
         */
        virtual VOID notifyUpdateAccessResult(const UINT32 result, const UINT32 seqId) {}

    private:
        NC_DISABLE_COPY(NCNetworkPluginCallback);
    };

} /* namespace nutshell */

#endif /* NCNETWORKPLUGINCALLBACK_H */
/* EOF */
