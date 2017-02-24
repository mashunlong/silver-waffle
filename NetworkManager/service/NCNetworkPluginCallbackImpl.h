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
 * @file NCNetworkPluginCallbackImpl.h
 * @brief Declaration file of class NCNetworkPluginCallbackImpl.
 *
 * This file includes the declaration of class NCNetworkPluginCallbackImpl, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKPLUGINCALLBACKIMPL_H
#define NCNETWORKPLUGINCALLBACKIMPL_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <ncore/NCString.h>
#include "networkplugin/NCNetworkPluginCallback.h"

namespace nutshell
{
    /**
     * This class implemented the NCNetworkPluginCallback.
     *
     * This class inherit the NCNetworkPluginCallback.
    */
    class NCNetworkPluginCallbackImpl : public NCNetworkPluginCallback
    {
    public:
        NCNetworkPluginCallbackImpl();
        virtual ~NCNetworkPluginCallbackImpl();

        virtual VOID notifyRequestAccessResult(const RequestPluginResult& info);
        virtual VOID notifyUpdateAccessResult(const UINT32 result, const UINT32 seqId);
        virtual VOID notifyReleaseAccessResult(const UINT32 result, const NCString& deviceType, const UINT32 seqId);

    private:
        NC_DISABLE_COPY(NCNetworkPluginCallbackImpl);
    };

} /* namespace nutshell */

#endif /* NCNETWORKPLUGINCALLBACKIMPL_H */
/* EOF */
