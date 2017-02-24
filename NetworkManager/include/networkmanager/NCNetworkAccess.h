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
 * @file NCNetworkAccess.h
 * @brief Declaration file of class NCNetworkAccess.
 *
 * This file includes the declaration of class NCNetworkAccess, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKACCESS_H
#define NCNETWORKACCESS_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCGlobalAPI.h>
#include <ncservice/NCBinderServiceProxy.h>
#include <evip/NEServiceProxy.h>
#include "networkmanager/NCNetworkDef.h"

namespace nutshell
{
    /**
     * @brief The proxy class of NetworkManager.
     *
     * This class provides interfaces to request or release network access.
     */
    class NUTSHELL_API NCNetworkAccess : public NCBinderServiceProxy, public NEServiceProxy
    {
    public:

        /// The information of SLIP device
        struct SlipDeviceInfo
        {
            NCString deviceName;    ///< The name of SLIP device
            NCString deviceType;    ///< The type of SLIP device, which includes slip0, slip1, slip2
            NCString ip;            ///< The IP of SLIP device
            UINT32 mask;            ///< The prefix length of network mask
            UINT32 mtu;             ///< Maximum transmission unit
        };

        /// The information of request access result
        struct RequestAccessResult
        {
            NCString deviceName;    ///< The name of network device
            NCString deviceType;    ///< The type of network device
            NCString gateway;       ///< The gateway of default route
            NCString dns1;          ///< The primary DNS server
            NCString dns2;          ///< The secondary DNS server
        };

        /// The information of Gbook server dns
        struct GbookServerDnsInfo
        {
            NCString dcmDns1;       ///< The primary DNS for dcm
            NCString dcmDns2;       ///< The secondry DNS for dcm
            NCString wifiDns1;      ///< The primary DNS for wifi/btdun
            NCString wifiDns2;      ///< The secondry DNS for wifi/btdun
        };

        /**
         * Constructor
         */
        NCNetworkAccess();

        /**
         * Destructor
         */
        virtual ~NCNetworkAccess();

        /**
         * @brief Request network access.
         *
         * @param [in] resourceId : The resource identify.
         * @param [in] deviceType : The type of network device, which only can be dcm or btDun.
         * @param [in] mode : The access network mode that includes dial type and communicate priority
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 request(const NCString& resourceId, const NCString& deviceType,
                const AccessMode& mode = { DialType_Auto, CommunicatePriority_AutoHigh, "" });

        /**
         * @brief Request network access noly by dcm/dun plugin.
         *
         * @param [in] resourceId : The resource identify.
         * @param [in] deviceType : The type of network device, which only can be dcm or btDun.
         * @param [in] mode : The access network mode that includes dial type and communicate priority
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 remoteRequest(const NCString& resourceId, const NCString& deviceType,
                const AccessMode& mode = { DialType_Auto, CommunicatePriority_AutoHigh, "" });

        /**
         * @brief Request network access with sequence ID
         *
         * @param [in] deviceType : The type of network device, which only can be dcm or btDun.
         * @param [in] mode : The access network mode that includes dial type, communicate priority and resource ID
         * @param [out] seqId : The sequence ID for this request
         * 
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 requestWithSequenceId(const NCString& deviceType, const AccessMode& mode, UINT32& seqId);

        /**
         * @brief Release network access.
         *
         * @param [in] resourceId : The resource identify.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 release(const NCString& resourceId);

        /**
         * @brief Release network access with sequence ID.
         *
         * @param [in] resourceId : The resource identify.
         * @param [out] seqId : The sequence ID for this release.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 releaseWithSequenceId(const NCString& resourceId, UINT32& seqId);

        /**
         * This interface will be deleted later
         */
        INT32 disconnect();

        /**
         * @brief Disconenct the specified network.
         *
         * @param [in] deviceType : The type of network device, wifiSta, btDun or dcm.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 disconnect(const NCString& deviceType);

        /**
         * @brief Disconenct the specified network with sequence ID.
         *
         * @param [in] deviceType : The type of network device, wifiSta, btDun or dcm.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 disconnectWithSequenceId(const NCString& deviceType, UINT32& seqId);

        /**
         * @brief Disconenct the specified network. This interface is only used by DCM/DUN plugin.
         *
         * @param [in] deviceType : The type of network device, btDun or dcm.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 remoteDisconnect(const NCString& deviceType);

        /**
         * This interface will be deleted later
         */
        INT32 connect();

        /**
         * @brief Set IP, route and mtu for SLIP device.
         *
         * @param [in] slipInfo : The information of SLIP device
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 setSlipDevice(const struct SlipDeviceInfo& slipInfo);

        /**
         * @brief When SLIP device is disconnected, call this interface to notify NetworkManager.
         *
         * @param [in] deviceName : The name of SLIP device.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 notifySlipDisconnect(const NCString& deviceName);

        /**
         * This interface will be deleted later
         */
        INT32 notifyRequestAccessResult(const struct RequestAccessResult& accessInfo);

        /**
         * This interface will be deleted later
         */
        INT32 notifyDisconnectResult(const NCString& deviceType, const UINT32 result);

        /**
         * @brief When dcm/btDun is disconnected, call this interface to notify NetworkManager.
         *
         * @param [in] deviceType : The network device type
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 notifyDisconnectResult(const NCString& deviceType);

        /**
         * This interface will be deleted later
         */
        INT32 updateGbookDns(const NCString& dns1, const NCString& dns2);

        /**
         * This interface will be deleted later
         */
        INT32 updateGbookWifiDns(const NCString& dns1, const NCString& dns2);

        /**
         * @brief Update gbook dns for dcm connection
         *
         * @param [in] info : The Gbook server dns information
         * 
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 updateGbookServerDns(const GbookServerDnsInfo& info);

        /**
         * @brief Bind service and register notify ID.
         *
         * @param None.
         *
         * @return NC_TRUE indicates success, NC_FALSE indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_BOOL bindService();

        /**
         * @brief Unbind service and unregister notify ID.
         *
         * @param None.
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID unbindService();

    private:

        /**
         * Receive message from service.
         */
        virtual VOID onReceiveMessage(NEMessage& message);

        NC_DISABLE_COPY(NCNetworkAccess);
    };

} /* namespace nutshell */

#endif /* NCNETWORKACCESS_H */
/* EOF */
