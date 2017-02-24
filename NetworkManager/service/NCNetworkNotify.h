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
 * @file NCNetworkNotify.h
 * @brief Declaration file of class NCNetworkNotify.
 *
 * This file includes the declaration of class NCNetworkNotify, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKNOTIFY_H
#define NCNETWORKNOTIFY_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <string>
#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include "NCNetworkManagerDef.h"
#include "networkmanager/NCNetworkDataReplier.h"

namespace nutshell
{
    class NCNetworkStatus;
    class NCNetworkDeviceType;
    /**
     * @brief Class of NCNetworkNotify.
     *
     * This class is used to notify others the result of request.
     */
    class NCNetworkNotify
    {
    public:
        NCNetworkNotify();
        virtual ~NCNetworkNotify();

        /// Return the result of request IP address
        VOID notifyRequestIpResult(const NCString& deviceType, UINT32 result);
        /// Return the result of release IP address
        VOID notifyReleaseIpResult(const NCString& deviceType, UINT32 result);
        /// Return the result of start DHCP server
        VOID notifyStartDhcpServerResult(const NCString& deviceType, UINT32 result);
        /// Return the result of stop DCHP server
        VOID notifyStopDhcpServerResult(const NCString& deviceType, UINT32 result);
        /// Return the result of request network access
        VOID notifyRequestAccessResult(const NCString& resourceId, const UINT32 result, const NC_BOOL wifiApFlag, const UINT32 seqId);
        /// Return the result of release network access
        VOID notifyReleaseAccessResult(const NCString& resourceId, const UINT32 result, const UINT32 seqId);

        /// Return the result disconnect
        VOID notifyDisconnectResult(const NCString& deviceType, const UINT32 result, const UINT32 seqId);

        /// Return the request device information
        VOID notifyDeviceInfo(const NCString& deviceType, const NCNetworkDataReplier::NetworkDeviceInfo& info);
        /// Return the connected client information
        VOID notifyConnectedClientInfo(const NCString& mac, const NCNetworkDataReplier::ConnectedClientInfo& info);
        /// Notify dial state of dcm/btDun
        VOID notifyDialState(const NCString& deviceType, const UINT32 state, const UINT32 type);
        
        /// Notify dcm/dun link state
        VOID notifyLinkState(const NCString& deviceType, const UINT32 dialState, const UINT32 dialType);

        VOID notifyStartFirewallFailed();

        VOID broadcastNetworkStatus(const NCString& identity, const NCString& deviceType);

        VOID notifyNcmIpFinished();
        
    private:
        UINT32 parseResult(const UINT32 result);
        VOID notifyResult(std::string param, UINT32 type, UINT32 methodId);

        NC_DISABLE_COPY(NCNetworkNotify);
    };

} /* namespace nutshell */

#endif /* NCNETWORKNOTIFY_H */
/* EOF */
