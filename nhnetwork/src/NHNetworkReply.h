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
 * @file NHNetworkReply.h
 * @brief Declaration file of class NHNetworkReply.
 *
 * This file includes the declaration of class NHNetworkReply, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NHNETWORKREPLY_H
#define NHNETWORKREPLY_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <ncore/NCString.h>
#include "nhnetwork/NHNetworkDefine.h"

namespace nutshell
{
    /**
     * @brief This class is used to send event.
     *
     * This class is used to notify service the result of NHNetworkHandler by event.
     */
    class NHNetworkReply
    {
    public:
        NHNetworkReply();
        virtual ~NHNetworkReply();

        /**
         * @brief Notify the result of doing DHCP request.
         *
         * @param [in] device : The name of network device.
         * @param [in] info : The struct of DhclientInfo.
         * @param [in] result : The result of doing DHCP request.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyDoDhcpResult(const NCString& device, const DhclientInfo& info, UINT32 result);

        /**
         * @brief Notify the result of stopping IP request.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of stopping DHCP request.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyStopDhcpResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of releasing DHCP lease.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of releasing DHCP lease.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyReleaseDhcpLeaseResult(const NCString& device, UINT32 result);


        /**
         * @brief Notify the result of setting static IP.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of setting static IP.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifySetStaticIpResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of setting mask.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of setting mask.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifySetMaskResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of setting MTU(Maximum Transmission Unit).
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of setting MTU.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifySetMtuResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of setting static ip, mask and mtu.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of setting mask.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifySetStaticIpMaskMtuResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of adding default route.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of adding default route.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyAddDefaultRouteResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of removing default route.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of removing default route.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyRemoveDefaultRouteResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of adding route.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of adding route.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyAddRouteResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of removing route.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of removing route.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyRemoveRouteResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of setting default rules in filter table.
         *
         * @param [in] result : The result of setting default rules in filter table.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifySetFilterTableDefaultRuleResult(UINT32 result);

        /**
         * @brief Notify the result of adding rules in filter table.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of adding rules in filter table.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyAddFilterTableRuleResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of removing rules in filter table.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of removing rules in filter table.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyRemoveFilterTableRuleResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of setting rules in NAT table.
         *
         * @param [in] result : The result of setting rules in NAT table.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifySetNatTableRuleResult(UINT32 result);

        /**
         * @brief Notify the information of network device.
         *
         * @param [in] info : The struct that store device information.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyNetworkDeviceInfoResult(const NHNetworkDeviceInfo& info, UINT32 result);

        /**
         * @brief Notify the information of connected device.
         *
         * @param [in] info : The struct that store connected device information.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyConnectedClientInfoResult(const NHNetworkConnectedClientInfo& info, UINT32 result);

        /**
         * @brief Notify the result of starting DHCP server.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of starting DHCP server.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyStartDhcpServerResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of stopping DHCP server result.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of stopping DHCP server.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyStopDhcpServerResult(const NCString& device, UINT32 result);

        /**
         * @brief Notify the result of restart DHCP server result.
         *
         * @param [in] device : The name of network device.
         * @param [in] result : The result of restarting DHCP server.
         *
         * @return None.
         *
         * @attention Asynchronous I/F.
         */
        VOID notifyRestartDhcpServerResult(const NCString& device, UINT32 result);

    private:
        NC_DISABLE_COPY(NHNetworkReply);
    };

} /* namespace nutshell */

#endif /* NHNETWORKREPLY_H */
/* EOF */
