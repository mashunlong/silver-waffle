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
 * @file NHNetworkHandler.h
 * @brief Declaration file of class NHNetworkHandler.
 *
 * This file includes the declaration of class NHNetworkHandler, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NHNETWORKHANDLER_H
#define NHNETWORKHANDLER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCQueuingThread.h>
#include "NHNetworkReq.h"
#include "NINetworkHandlerStub.h"

namespace nutshell
{
    class NHDevice;
    class NHNetworkDhcpInfo;
    class NHNetworkTimerThread;

    /**
     * @brief Implementation class of NINetworkHandler.
     *
     * This class implements the interface in NINetworkHandler. The main functions include <br>
     * requesting or releasing IP address, adding or removing route, starting or stopping DHCP <br>
     * server, getting the information of network device, etc.
     */
    class NHNetworkHandler : public NINetworkHandlerStub
    {
    public:
        /// Constructor.
        NHNetworkHandler(NHDevice* device);

        /// Destructor
        virtual ~NHNetworkHandler();

        /**
         * @brief Request IP address.
         *
         * @param [in] device : The name of network device.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR doDhcpRequest(const NCString& device);

        /**
         * @brief Cancel IP address request.
         *
         * @param [in] device : The name of network device.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR stopDhcpRequest(const NCString& device);

        /**
         * @brief Release IP address.
         *
         * @param [in] device : The name of network device.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR releaseDhcpLease(const NCString& device);

        /**
         * @brief Set static IP address.
         *
         * @param [in] device : The name of network device.
         * @param [in] staticIp : The specified static IP address.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR setStaticIp(const NCString& device, const NCString& staticIp);

        /**
         * @brief Set network mask.
         *
         * @param [in] device : The name of network device.
         * @param [in] prefixLength : The prefix length of network mask.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR setMask(const NCString& device, const UINT32& prefixLength);

        /**
         * @brief Set MTU (Maximum Transmission Unit) for network device
         *
         * @param [in] device : The name of network device.
         * @param [in] mtu : The maximum transmission unit
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR setMtu(const NCString& device, const UINT32 mtu);

        /**
         * @brief Set IP mask and mtu for netowork interface
         *
         * @param [in] device : The name of network device.
         * @param [in] staticIp : The IP address.
         * @param [in] prefixLength : The prefix length of network mask.
         * @param [in] mtu : The maximum transmission unit
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
         virtual NC_ERROR setStaticIpMaskMtu(const NCString& device, const NCString& staticIp, const UINT32& prefixLength, const UINT32& mtu);

        /**
         * @brief Set gateway.
         *
         * @param [in] device : The name of network device.
         * @param [in] gateway : The IP address of gateway.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR setGateway(const NCString& device, const NCString& gateway);

        /**
         * @brief Add default route.
         *
         * @param [in] device : The name of network device.
         * @param [in] gateway : The gateway of default route.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR addDefaultRoute(const NCString& device, const NCString& gateway);

        /**
         * @brief Remove default route.
         *
         * @param [in] device : The name of network device.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR removeDefaultRoute(const NCString& device);

        /**
         * @brief Add route.
         *
         * @param [in] device : The name of network device.
         * @param [in] route : The information of specified route. It includes destination IP address,
         *                     network mask and gateway.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR addRoute(const NCString& device, const struct RouteInfo& route);

        /**
         * @brief Remove route.
         *
         * @param [in] device : The name of network device.
         * @param [in] route : The information of specified route. It includes destination IP address,
         *                     network mask and gateway.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR removeRoute(const NCString& device, const struct RouteInfo& route);

        /**
         * @brief Set default rules for firewall.
         *
         * @param None.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR setFirewallDefaultRule();

        /**
         * @brief Open firewall port.
         *
         * @param [in] protocol : The transport layer protocol that includes tcp and udp
         * @param [in] port : The port number
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR openFirewallPort(const NCString& protocol, const UINT32 port);

        /**
         * @brief Close firewall port.
         *
         * @param [in] protocol : The transport layer protocol that includes tcp and udp
         * @param [in] port : The port number
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR closeFirewallPort(const NCString& protocol, const UINT32 port);

        /**
         * @brief Set NAT table.
         *
         * @param [in] setMode : The current mode to access network.
         * @param [in] deviceName : The name of network device, which are used to forward packets.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR setNatTable(enum NatSetMode setMode, const NCString& deviceName);

        /**
         * @brief Get the detail information of specified network device.
         *
         * @param [in] device : The name of network device.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR networkDeviceInfo(const NCString& device);

        /**
         * @brief Get the detail information of connected client.
         *
         * @param [in] mac : The mac of connected client.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR connectedClientInfo(const NCString& mac);

        /**
         * @brief Start DHCP server.
         *
         * @param [in] device : The name of network device.
         * @param [in] ip : The static ip address.
         * @param [in] prefixLength : The prefix length of mask
         * 
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR startDhcpServer(const NCString& device, const NCString& ip, const UINT32 prefixLength);

        /**
         * @brief Stop DHCP server.
         *
         * @param [in] device : The name of network device.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR stopDhcpServer(const NCString& device);

        /**
         * @brief Restart DHCP server.
         *
         * @param [in] device : The name of network device.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR restartDhcpServer(const NCString& device);

        /**
         * @brief Start DNS Cache Server
         */
        virtual NC_ERROR startDnsCacheServer();

        /**
         * @brief Stop DNS Cache Server.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR stopDnsCacheServer();

        /**
         * @brief Update named.conf

         * @param [in] option : The option in named.conf
         * @param [in] dns1 : The primary domain name server
         * @param [in] dns2 : The secondary domain name server
         * 
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR updateNamedConf(const NCString& option, const NCString& dns1, const NCString& dns2);

        /**
         * @brief Notify firewall configure failed.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_ERROR notifyFirewallFailed();

        static NHNetworkHandler* getNetworkHandler();

        NHNetworkDhcpInfo* getDhcpInfoPointer();

        /**
         * @brief Start timer
         *
         * @return none
         *
         * @attention Synchronous I/F.
         */
        VOID startTimer(const UINT32 type);

        /**
         * @brief Stop timer
         *
         * @return none
         *
         * @attention Synchronous I/F.
         */
        VOID stopTimer(const UINT32 type);

        /**
         * @brief Post request to work thread
         *
         * @return none
         *
         * @attention Synchronous I/F.
         */
        VOID postRequest(NHNetworkReqBase* req);

    public:
        NC_BOOL isDhcpdFirstInit();
        VOID setDhcpdFirstInit(const NC_BOOL value);

    protected:
        /**
         * @brief handler onInitialize
         *
         * @return void
         *
         * @note [Synchronized Call]
         */
        virtual VOID onInitialize();

        /**
         * @brief handler onStart
         *
         * @return void
         * @note [Synchronized Call]
         */
        virtual VOID onStart();

        /**
         * @brief handler onStop
         *
         * @return void
         * @note [Synchronized Call]
         */
        virtual VOID onStop();

        /**
         * @brief handler onFinalize
         *
         * @return void
         * @note [Synchronized Call]
         */
        virtual VOID onFinalize();

        /**
         * @brief onStartUp
         *
         * @param[in] buff: param content
         * @param[in] size          size
         * @return void
         * @note [Synchronized Call]
         * @note empty implememt in base, if handler want to do something, must overload it
         */
        virtual VOID onStartUp(void* buff, uint32_t size);

        /**
         * @brief onHibernate
         *
         * @param[in] buff: param content
         * @param[in] size: size
         * @return void
         * @note [Synchronized Call]
         * @note empty implememt in base, if handler want to do something, must overload it
         */
        virtual VOID onHibernate(void* buff, uint32_t size);

        /**
         * @brief onAwake
         *
         * @param[in] buff: param content
         * @param[in] size: size
         * @return void
         * @note [Synchronized Call]
         * @note empty implememt in base, if handler want to do something, must overload it
         */
        virtual VOID onAwake(void* buff, uint32_t size);

    private:
        /// set property for AvcLan static IP
        VOID setAvclanIpProperty();
        // if netd crash and restart, do something
        VOID checkNetdCrash();

        static NHNetworkHandler* m_networkHandler;
        NCQueuingThread<NHNetworkReqBase>* m_reqThread;
        NHNetworkTimerThread* m_timerThread;
        NHNetworkDhcpInfo* m_dhcpInfo;

        NC_BOOL m_bDhcpd_firstInit;

        NC_DISABLE_COPY(NHNetworkHandler);
    };

} /* namespace nutshell */

#endif /* NHNETWORKHANDLER_H */
/* EOF */
