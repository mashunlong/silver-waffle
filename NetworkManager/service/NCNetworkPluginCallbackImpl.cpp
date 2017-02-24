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

#include <ncore/NCLog.h>
#include "networkmanager/NCNetworkDef.h"
#include "NCNetworkManager.h"
#include "NCNetworkEventReq.h"
#include "NCNetworkPluginCallbackImpl.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{
    NCNetworkPluginCallbackImpl::NCNetworkPluginCallbackImpl()
    : NCNetworkPluginCallback()
    {

    }

    NCNetworkPluginCallbackImpl::~NCNetworkPluginCallbackImpl()
    {
    }

    VOID
    NCNetworkPluginCallbackImpl::notifyRequestAccessResult(const RequestPluginResult& info)
    {
        NCLOGD("NCNetworkPluginCallbackImpl::notifyRequestAccessResult, Start");
        NCLOGD("ReqId = [%d], result = [%d], deviceType = [%s]", info.reqId, info.result, info.deviceType.getString());
        NCNetworkManager::Instance()->getEventThreadPointer()->looper().postRunnable(new NCNetworkRequestCallbackReq(info));
    }

    VOID
    NCNetworkPluginCallbackImpl::notifyUpdateAccessResult(const UINT32 result, const UINT32 seqId)
    {
        NCLOGD("NCNetworkPluginCallbackImpl::notifyUpdateAccessResult, Start");
        NCLOGD("ReqId = [%d], result = [%d]", seqId, result);
        NCNetworkManager::Instance()->getEventThreadPointer()->looper().postRunnable(new NCNetworkReleaseCallbackReq(result, seqId));
    }
    
    VOID
    NCNetworkPluginCallbackImpl::notifyReleaseAccessResult(const UINT32 result, const NCString& deviceType, const UINT32 seqId)
    {
        NCLOGD("NCNetworkPluginCallbackImpl::notifyReleaseAccessResult, Start");
        NCLOGD("ReqId = [%d], result = [%d], deviceType = [%s]", seqId, result, deviceType.getString());
        NCNetworkManager::Instance()->getEventThreadPointer()->looper().postRunnable(new NCNetworkDisconnectCallbackReq(result, deviceType, seqId));
    }

} /* namespace nutshell */
/* EOF */
