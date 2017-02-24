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
 * @file NCNetworkDhcp.h
 * @brief Declaration file of class NCNetworkDhcp.
 *
 * This file includes the declaration of class NCNetworkDhcp, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKDHCP_H
#define NCNETWORKDHCP_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCGlobalAPI.h>
#include <ncservice/NCBinderServiceProxy.h>
#include <evip/NEServiceProxy.h>

namespace nutshell
{
    /**
     * @brief The proxy class of NCNetworkManager.
     *
     * This class provided interfaces which are related to DHCP.
     */
    class NUTSHELL_API NCNetworkDhcp : public NCBinderServiceProxy, public NEServiceProxy
    {
    public:

        /**
         * Constructor
         */
        NCNetworkDhcp();

        /**
         * Destructor
         */
        virtual ~NCNetworkDhcp();

        /**
         * @brief Request IP via DHCP protocol. The specified device not need verify resource ID.
         *
         * @param [in] deviceType : The device type, except wifiSta.
         * @param [in] deviceName : The device name.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 requestIp(const NCString& deviceType, const NCString& deviceName);

        /**
         * @brief Request IP via DHCP protocol.The specified device need verify resource ID.
         *
         * @param [in] resourceId : The reource identify
         * @param [in] deviceType : The device type, only wifiSta can be specified.
         * @param [in] deviceName : The device name.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 requestIp(const NCString& resourceId, const NCString& deviceType, const NCString& deviceName);

        /**
         * @brief Cancel request IP.
         *
         * @param [in] deviceType : The device type, pan or wifiSta.
         * @param [in] deviceName : The device name.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 stopRequestIp(const NCString& deviceType, const NCString& deviceName);

        /**
         * @brief Release IP via DHCP protocol.
         *
         * @param [in] deviceType : The device type, pan or wifiSta.
         * @param [in] deviceName : The device name.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 releaseIp(const NCString& deviceType, const NCString& deviceName);


        /**
         * @brief After open AP model, start DHCP server.
         *
         * @param [in] deviceType : The device type, which is wifiAp.
         * @param [in] deviceName : The device name.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 startDhcpServer(const NCString& deviceType, const NCString& deviceName);

        /**
         * @brief After close AP model, Stop DHCP server, which is wifiAp.
         *
         * @param [in] deviceType : The device type.
         * @param [in] deviceName : The device name.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 stopDhcpServer(const NCString& deviceType, const NCString& deviceName);

        /**
         * This interface will be deleted later
         */
        NC_BOOL checkDhcpClientStatus(const NCString& deviceName);

        /**
         * @brief Check the dhcp server is running or not.
         *
         * @param None.
         *
         * @return NC_TRUE indicates there is a dhcp server running
         *         NC_FALSE indicates there is no dhcp server running.
         *
         * @attention Synchronous I/F.
         */
        NC_BOOL checkDhcpServerStatus();

        /**
         * @brief Start monitor data communication on specified network node.
         *
         * @param [in] deviceType : The device type.
         * @param [in] deviceName : The device name.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 startMonitorDataComm(const NCString& deviceType, const NCString& deviceName);

        /**
         * @brief Stop monitor data communication on specified network node.
         *
         * @param [in] deviceType : The device type.
         * @param [in] deviceName : The device name.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 stopMonitorDataComm(const NCString& deviceType, const NCString& deviceName);

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
        virtual VOID onReceiveMessage(NEMessage &message);

        NC_DISABLE_COPY(NCNetworkDhcp);
    };

} /* namespace nutshell */

#endif /* NCNETWORKDHCP_H */
/* EOF */
