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
 * @file NHNetworkReq.h
 * @brief Declaration file of class NHNetworkReqBase.
 *
 * @attention used for C++ only.
 */

#ifndef NHNETWORKREQ_H
#define NHNETWORKREQ_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCQueuingThread.h>
#include "nhnetwork/NHNetworkDefine.h"

namespace nutshell
{
    /**
     * @brief Request base class for NHNetworkHandler.
     *
     * This class will be inherited by other request class.
     */
    class NHNetworkReqBase
    {
    public:
        NHNetworkReqBase() {}
        virtual ~NHNetworkReqBase() {}

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread) = 0;

    private:
        NC_DISABLE_COPY(NHNetworkReqBase);
    };

    /**
     * @brief This class is used to request IP address.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkDoDhcpReq : public NHNetworkReqBase
    {
    public:
        NHNetworkDoDhcpReq(const NCString& device);
        virtual ~NHNetworkDoDhcpReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;

        NC_DISABLE_COPY(NHNetworkDoDhcpReq);
    };

    /**
     * @brief This class is used to stop IP request.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkStopDhcpReq : public NHNetworkReqBase
    {
    public:
        NHNetworkStopDhcpReq(const NCString& device);
        virtual ~NHNetworkStopDhcpReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;

        NC_DISABLE_COPY(NHNetworkStopDhcpReq);
    };

    /**
     * @brief This class is used to release DHCP lease.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkReleaseDhcpLeaseReq : public NHNetworkReqBase
    {
    public:
        NHNetworkReleaseDhcpLeaseReq(const NCString& device);
        virtual ~NHNetworkReleaseDhcpLeaseReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;

        NC_DISABLE_COPY(NHNetworkReleaseDhcpLeaseReq);
    };

    /**
     * @brief This class is used to set static IP address.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkSetStaticIpReq : public NHNetworkReqBase
    {
    public:
        NHNetworkSetStaticIpReq(const NCString& device, const NCString& staticIp);
        virtual ~NHNetworkSetStaticIpReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);
        UINT32 setIpAddr();

    private:
        NCString m_device;
        NCString m_staticIp;

        NC_DISABLE_COPY(NHNetworkSetStaticIpReq);
    };

    /**
     * @brief This class is used to set mask.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkSetMaskReq : public NHNetworkReqBase
    {
    public:
        NHNetworkSetMaskReq(const NCString& device, const UINT32& prefixLength);
        virtual ~NHNetworkSetMaskReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);
        UINT32 setMask();

    private:
        NCString m_device;
        UINT32 m_prefixLength;

        NC_DISABLE_COPY(NHNetworkSetMaskReq);
    };

    /**
     * @brief This class is used to set MTU.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkSetMtuReq : public NHNetworkReqBase
    {
    public:
        NHNetworkSetMtuReq(const NCString& device, const UINT32 mtu);
        virtual ~NHNetworkSetMtuReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);
        UINT32 setMtu();

    private:
        NCString m_device;
        UINT32 m_mtu;

        NC_DISABLE_COPY(NHNetworkSetMtuReq);
    };

    /**
     * @brief This class is used to set static ip, mask and mtu.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkSetStaticIpMaskMtuReq : public NHNetworkReqBase
    {
    public:
        NHNetworkSetStaticIpMaskMtuReq(const NCString& device, const NCString& staticIp, const UINT32& prefixLength, const UINT32& mtu);
        virtual ~NHNetworkSetStaticIpMaskMtuReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;
        NCString m_staticIp;
        UINT32 m_prefixLength;
        UINT32 m_mtu;

        NC_DISABLE_COPY(NHNetworkSetStaticIpMaskMtuReq);
    };

    /**
     * @brief This class is used to set gateway.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkSetGatewayReq : public NHNetworkReqBase
    {
    public:
        NHNetworkSetGatewayReq(const NCString& device, const NCString& gateway);
        virtual ~NHNetworkSetGatewayReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;
        NCString m_gateway;

        NC_DISABLE_COPY(NHNetworkSetGatewayReq);
    };

    /**
     * @brief This class is used to add default route.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkAddDefauteRouteReq : public NHNetworkReqBase
    {
    public:
        NHNetworkAddDefauteRouteReq(const NCString& device, const NCString& gateway, const NC_BOOL updateFlag = NC_FALSE);
        virtual ~NHNetworkAddDefauteRouteReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;
        NCString m_gateway;
        NC_BOOL m_updateFlag;

        NC_DISABLE_COPY(NHNetworkAddDefauteRouteReq);
    };

    /**
     * @brief This class is used to remove default route.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkRemoveDefauteRouteReq : public NHNetworkReqBase
    {
    public:
        NHNetworkRemoveDefauteRouteReq(const NCString& device);
        virtual ~NHNetworkRemoveDefauteRouteReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;

        NC_DISABLE_COPY(NHNetworkRemoveDefauteRouteReq);
    };

    /**
     * @brief This class is used to add route.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkAddRouteReq : public NHNetworkReqBase
    {
    public:
        NHNetworkAddRouteReq(const NCString& device, const NHNetworkRouteInfo& route);
        virtual ~NHNetworkAddRouteReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;
        NHNetworkRouteInfo m_route;

        NC_DISABLE_COPY(NHNetworkAddRouteReq);
    };

    /**
     * @brief This class is used to remove route.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkRemoveRouteReq : public NHNetworkReqBase
    {
    public:
        NHNetworkRemoveRouteReq(const NCString& device, const NHNetworkRouteInfo& route);
        virtual ~NHNetworkRemoveRouteReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;
        NHNetworkRouteInfo m_route;

        NC_DISABLE_COPY(NHNetworkRemoveRouteReq);
    };

    /**
     * @brief This class is used to close all firewall port.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkSetFirewallDefaultRuleReq : public NHNetworkReqBase
    {
    public:
        NHNetworkSetFirewallDefaultRuleReq();
        virtual ~NHNetworkSetFirewallDefaultRuleReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NHNetworkSetFirewallDefaultRuleReq);
    };

    /**
     * @brief This class is used to open the specified firewall port.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkOpenFirewallPortReq : public NHNetworkReqBase
    {
    public:
        NHNetworkOpenFirewallPortReq(const NCString& protocol, UINT32 port);
        virtual ~NHNetworkOpenFirewallPortReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_protocol;
        UINT32 m_port;

        NC_DISABLE_COPY(NHNetworkOpenFirewallPortReq);
    };

    /**
     * @brief This class is used to close the specified firewall port.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkCloseFirewallPortReq : public NHNetworkReqBase
    {
    public:
        NHNetworkCloseFirewallPortReq(const NCString& protocol, UINT32 port);
        virtual ~NHNetworkCloseFirewallPortReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_protocol;
        UINT32 m_port;

        NC_DISABLE_COPY(NHNetworkCloseFirewallPortReq);
    };

    /**
     * @brief This class is used to set NAT table.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkSetNatTableReq : public NHNetworkReqBase
    {
    public:
        NHNetworkSetNatTableReq(UINT32 setMode, const NCString& deviceName);
        virtual ~NHNetworkSetNatTableReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        UINT32 m_setMode;
        NCString m_deviceName;

        NC_DISABLE_COPY(NHNetworkSetNatTableReq);
    };

    /**
     * @brief This class is used to get information of network device.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkDeviceInfoReq : public NHNetworkReqBase
    {
    public:
        NHNetworkDeviceInfoReq(const NCString& device);
        virtual ~NHNetworkDeviceInfoReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;

        NC_DISABLE_COPY(NHNetworkDeviceInfoReq);
    };


    /**
     * @brief This class is used to get information of connected client.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkConnectedClientInfoReq : public NHNetworkReqBase
    {
    public:
        NHNetworkConnectedClientInfoReq(const NCString& mac);
        virtual ~NHNetworkConnectedClientInfoReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_mac;

        NC_DISABLE_COPY(NHNetworkConnectedClientInfoReq);
    };


    /**
     * @brief This class is used to start DHCP server.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkStartDhcpServerReq : public NHNetworkReqBase
    {
    public:
        NHNetworkStartDhcpServerReq(const NCString& device, const NCString& ip, const UINT32& prefixLength);
        virtual ~NHNetworkStartDhcpServerReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;
        NCString m_ip;
        UINT32 m_prefixLength;

        NC_DISABLE_COPY(NHNetworkStartDhcpServerReq);
    };

    /**
     * @brief This class is used to stop DHCP server.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkStopDhcpServerReq : public NHNetworkReqBase
    {
    public:
        NHNetworkStopDhcpServerReq(const NCString& device);
        virtual ~NHNetworkStopDhcpServerReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;

        NC_DISABLE_COPY(NHNetworkStopDhcpServerReq);
    };

    /**
     * @brief This class is used to restart DHCP server.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkRestartDhcpServerReq : public NHNetworkReqBase
    {
    public:
        NHNetworkRestartDhcpServerReq(const NCString& device);
        virtual ~NHNetworkRestartDhcpServerReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_device;

        NC_DISABLE_COPY(NHNetworkRestartDhcpServerReq);
    };

    

    /**
     * @brief This class is used to start DNS Cache Server.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkStartDnsCacheServerReq : public NHNetworkReqBase
    {
    public:
        NHNetworkStartDnsCacheServerReq();
        virtual ~NHNetworkStartDnsCacheServerReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NHNetworkStartDnsCacheServerReq);
    };

    /**
     * @brief This class is used to stop DNS Cache Server.
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkStopDnsCacheServerReq : public NHNetworkReqBase
    {
    public:
        NHNetworkStopDnsCacheServerReq();
        virtual ~NHNetworkStopDnsCacheServerReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NHNetworkStopDnsCacheServerReq);
    };


    /**
     * @brief This class is used to update remote dns in in /tmp/named.conf
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkUpdateRemoteDnsReq : public NHNetworkReqBase
    {
    public:
        NHNetworkUpdateRemoteDnsReq(const NCString& dns1, const NCString& dns2);
        virtual ~NHNetworkUpdateRemoteDnsReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_dns1;
        NCString m_dns2;

        NC_DISABLE_COPY(NHNetworkUpdateRemoteDnsReq);

    };

    /**
     * @brief This class is used to update gbook dns in /tmp/named.conf
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
     class NHNetworkUpdateGbookDnsReq : public NHNetworkReqBase
     {
     public:
        NHNetworkUpdateGbookDnsReq(const NCString& dns1, const NCString& dns2);
        virtual ~NHNetworkUpdateGbookDnsReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NCString m_dns1;
        NCString m_dns2;

        NC_DISABLE_COPY(NHNetworkUpdateGbookDnsReq);
     };

    /**
     * @brief This class is used to reset dhclient info when netd restart
     *
     * This class will be posted in NCQueuingThread, which will run the doAction function.
     */
    class NHNetworkNetdCrashReq : public NHNetworkReqBase
    {
    public:
        NHNetworkNetdCrashReq();
        virtual ~NHNetworkNetdCrashReq();

        virtual VOID doAction(NCQueuingThread<NHNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NHNetworkNetdCrashReq);
    };

} /* namespace nutshell */

#endif /* NHNETWORKREQ_H */
/* EOF */
