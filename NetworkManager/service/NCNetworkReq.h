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
 * @file NCNetworkReq.h
 * @brief Declaration file of class NCNetworkReqBase.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKREQ_H
#define NCNETWORKREQ_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCSpThread.h>
#include <ncore/NCWaitObj.h>
#include <vector>
#include <networkmanager/NCNetworkAccess.h>
#include "NCNetworkManagerDef.h"
#include "NCNetworkPluginCallbackImpl.h"

namespace nutshell
{

    /**
     * @brief Request base class for NCNetworkManager.
     *
     * This class will be inherited by other request class.
     */
    class NCNetworkReqBase : public NCRefBase
    {
    public:
        NCNetworkReqBase(const UINT32 reqType = 0);
        virtual ~NCNetworkReqBase();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread) = 0;
        virtual UINT32 getFuncType();
        virtual VOID doTimeout();
        virtual UINT32 getReqState();
        virtual VOID setReqState(const UINT32 state);
        virtual VOID lockReqState();
        virtual VOID unlockReqState();
        
        virtual NCString getDeviceType()
        {
            return NCString("");
        }

        virtual NCString getDeviceName()
        {
            return NCString("");
        }
        
        virtual UINT32 getLastRequest()
        {
            return DialState_Init;
        }

        virtual NC_BOOL getRemoteFlag()
        {
            return NC_FALSE;
        }

        virtual VOID lockObject()
        {

        }

        virtual VOID unlockObject()
        {

        }

        virtual VOID addRequestList(const AccessParameter& param) 
        {

        }

    protected:
        
        UINT32 m_reqType;
        UINT32 m_reqState;
        NCSpThread<NCNetworkReqBase>* m_thread;
        NCSyncObj m_syncState;

    private:
        NCNetworkReqBase(const NCNetworkReqBase&);
        NCNetworkReqBase& operator=(const NCNetworkReqBase&);
    };

    /**
     * @brief This class is used to set eth0 IP for adb debug
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkSetAdbDebugIpReq : public NCNetworkReqBase
    {
    public:
        NCNetworkSetAdbDebugIpReq();
        virtual ~NCNetworkSetAdbDebugIpReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NCNetworkSetAdbDebugIpReq(const NCNetworkSetAdbDebugIpReq&);
        NCNetworkSetAdbDebugIpReq& operator=(const NCNetworkSetAdbDebugIpReq&);
    };

    /**
     * @brief This class is used to set AVCLAN static IP.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkSetAvcLanIpReq : public NCNetworkReqBase
    {
    public:
        NCNetworkSetAvcLanIpReq();
        virtual ~NCNetworkSetAvcLanIpReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();

    private:
        NCNetworkSetAvcLanIpReq(const NCNetworkSetAvcLanIpReq&);
        NCNetworkSetAvcLanIpReq& operator=(const NCNetworkSetAvcLanIpReq&);
    };

    /**
     * @brief This class is used to add route for VRServer in Meu.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkAddRouteVRServer : public NCNetworkReqBase
    {
    public:
        NCNetworkAddRouteVRServer();
        virtual ~NCNetworkAddRouteVRServer();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();

    private:
        NC_DISABLE_COPY(NCNetworkAddRouteVRServer);
    };

    /**
     * @brief This class is used to request access network.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkRequestAccessReq : public NCNetworkReqBase
    {
    public:
        NCNetworkRequestAccessReq(const AccessParameter& param);
        virtual ~NCNetworkRequestAccessReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();

        virtual NCString getDeviceType();
        virtual VOID addRequestList(const AccessParameter& param);

        virtual VOID lockObject();
        virtual VOID unlockObject();

    private:
        AccessParameter m_param;
        std::vector<AccessParameter> m_reqAccessList;
        NCSyncObj m_syncList;

        NCNetworkRequestAccessReq(const NCNetworkRequestAccessReq&);
        NCNetworkRequestAccessReq& operator=(const NCNetworkRequestAccessReq&);
    };

    /**
     * @brief This class is used to release network access
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
     class NCNetworkReleaseReq : public NCNetworkReqBase
     {
     public:
        NCNetworkReleaseReq(const AccessParameter& param);
        virtual ~NCNetworkReleaseReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();

    private:
        AccessParameter m_param;

        NCNetworkReleaseReq(const NCNetworkReleaseReq&);
        NCNetworkReleaseReq& operator=(const NCNetworkReleaseReq&);
     };

    /**
     * @brief This class is used to call releaseAccess() in dcm/dun plugin
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkPluginReleaseAccessReq : public NCNetworkReqBase
    {
    public:
        NCNetworkPluginReleaseAccessReq(const AccessParameter& param);
        virtual ~NCNetworkPluginReleaseAccessReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();

    private:
        AccessParameter m_param;

        NCNetworkPluginReleaseAccessReq(const NCNetworkPluginReleaseAccessReq&);
        NCNetworkPluginReleaseAccessReq& operator=(const NCNetworkPluginReleaseAccessReq&);
    };

    /**
     * @brief This class is used to configure slip device
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkSetSlipDeviceReq : public NCNetworkReqBase
    {
    public:
        NCNetworkSetSlipDeviceReq(const NCNetworkAccess::SlipDeviceInfo& info);
        virtual ~NCNetworkSetSlipDeviceReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NCString m_deviceType;
        NCString m_deviceName;
        NCString m_ip;
        UINT32 m_mask;
        UINT32 m_mtu;

        NCNetworkSetSlipDeviceReq(const NCNetworkSetSlipDeviceReq&);
        NCNetworkSetSlipDeviceReq& operator=(const NCNetworkSetSlipDeviceReq&);

    };

    /**
     * @brief This class is used to request IP.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkRequestIpReq : public NCNetworkReqBase
    {
    public:
        NCNetworkRequestIpReq(const NCString& resourceId, const NCString& deviceType, const NCString& deviceName);
        virtual ~NCNetworkRequestIpReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();

        virtual NCString getDeviceType();
        virtual NCString getDeviceName();

    private:
        NCString m_resourceId;
        NCString m_deviceType;
        NCString m_deviceName;

        NCNetworkRequestIpReq(const NCNetworkRequestIpReq&);
        NCNetworkRequestIpReq& operator=(const NCNetworkRequestIpReq&);
    };


    /**
     * @brief This class is used to release IP.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkReleaseIpReq : public NCNetworkReqBase
    {
    public:
        NCNetworkReleaseIpReq(const NCString& deviceType, const NCString& deviceName);
        virtual ~NCNetworkReleaseIpReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();

        virtual NCString getDeviceType();
        virtual NCString getDeviceName();

    private:
        NCString m_deviceType;
        NCString m_deviceName;

        NCNetworkReleaseIpReq(const NCNetworkReleaseIpReq&);
        NCNetworkReleaseIpReq& operator=(const NCNetworkReleaseIpReq&);
    };

    /**
     * @brief This class is used to start DHCP server.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkStartDhcpServerReq : public NCNetworkReqBase
    {
    public:
        NCNetworkStartDhcpServerReq(const NCString& deviceType, const NCString& deviceName);
        virtual ~NCNetworkStartDhcpServerReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();

    private:
        NCString m_deviceType;
        NCString m_deviceName;

        NCNetworkStartDhcpServerReq(const NCNetworkStartDhcpServerReq&);
        NCNetworkStartDhcpServerReq& operator=(const NCNetworkStartDhcpServerReq&);
    };

    /**
     * @brief This class is used to stop DHCP server.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkStopDhcpServerReq : public NCNetworkReqBase
    {
    public:
        NCNetworkStopDhcpServerReq(const NCString& deviceType, const NCString& deviceName);
        virtual ~NCNetworkStopDhcpServerReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();

    private:
        NCString m_deviceType;
        NCString m_deviceName;

        NCNetworkStopDhcpServerReq(const NCNetworkStopDhcpServerReq&);
        NCNetworkStopDhcpServerReq& operator=(const NCNetworkStopDhcpServerReq&);
    };

    /**
     * @brief This class is used to get device info.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkDeviceInfoReq : public NCNetworkReqBase
    {
    public:
        NCNetworkDeviceInfoReq(const NCString& deviceType);
        virtual ~NCNetworkDeviceInfoReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();

    private:
        NCString m_deviceType;

        NCNetworkDeviceInfoReq(const NCNetworkDeviceInfoReq&);
        NCNetworkDeviceInfoReq& operator=(const NCNetworkDeviceInfoReq&);
    };

    /**
     * @brief This class is used to get connected client info.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkConnectedClientInfo : public NCNetworkReqBase
    {
    public:
        NCNetworkConnectedClientInfo(const NCString& mac);
        virtual ~NCNetworkConnectedClientInfo();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NCString m_mac;

        NC_DISABLE_COPY(NCNetworkConnectedClientInfo);
    };

    /**
     * @brief This class is used to initialize /etc/resolv.conf
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkInitResolvConfReq :public NCNetworkReqBase
    {
    public:
        NCNetworkInitResolvConfReq();
        virtual ~NCNetworkInitResolvConfReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
    private:
        NC_DISABLE_COPY(NCNetworkInitResolvConfReq);
    };

    /**
     * @brief This class is used to initialize /var/db/dhclient.lease and /var/db/dhcpd.lease
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkInitDhcpLeaseFileReq : public NCNetworkReqBase
    {
    public:
        NCNetworkInitDhcpLeaseFileReq();
        virtual ~NCNetworkInitDhcpLeaseFileReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NCNetworkInitDhcpLeaseFileReq);
    };

    /**
     * @brief This class is used to initialize /var/run/dhclient.pid and /var/db/dhcpd.pid
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkInitDhcpPidFileReq : public NCNetworkReqBase
    {
    public:
        NCNetworkInitDhcpPidFileReq();
        virtual ~NCNetworkInitDhcpPidFileReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NCNetworkInitDhcpPidFileReq);
    };

    /**
     * @brief This class is used to initialize /var/dhcpd.conf
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkInitDhcpdConfReq : public NCNetworkReqBase
    {
    public:
        NCNetworkInitDhcpdConfReq();
        virtual ~NCNetworkInitDhcpdConfReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NCNetworkInitDhcpdConfReq);
    };

    /**
     * @brief This class is used to update /var/dhcpd.conf
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkUpdateDhcpdConfReq : public NCNetworkReqBase
    {
    public:
        NCNetworkUpdateDhcpdConfReq(const NCString& dns1, const NCString& dns2);
        virtual ~NCNetworkUpdateDhcpdConfReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();
        
    private:
        NCString m_dns1;
        NCString m_dns2;
        
        NC_DISABLE_COPY(NCNetworkUpdateDhcpdConfReq);
    };

    /**
     * @brief This class is used to initialize /var/named.conf
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkInitNamedConfReq : public NCNetworkReqBase
    {
    public:
        NCNetworkInitNamedConfReq();
        virtual ~NCNetworkInitNamedConfReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NCNetworkInitNamedConfReq);
    };


    /**
     * @brief This class is update Gbook server dns information
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkSetGbookServerDnsReq : public NCNetworkReqBase
    {
    public:
        NCNetworkSetGbookServerDnsReq(const NCString& dcmDns1, const NCString& dcmDns2, const NCString& wifiDns1, const NCString& wifiDns2);
        virtual ~NCNetworkSetGbookServerDnsReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NCString m_dcmDns1;
        NCString m_dcmDns2;
        NCString m_wifiDns1;
        NCString m_wifiDns2;
        
        NC_DISABLE_COPY(NCNetworkSetGbookServerDnsReq);
    };

    /**
     * @brief This class is used to start DNS cache server
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkStartDnsCacheServerReq : public NCNetworkReqBase
    {
    public:
        NCNetworkStartDnsCacheServerReq();
        virtual ~NCNetworkStartDnsCacheServerReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NCNetworkStartDnsCacheServerReq);
    };

    /**
     * @brief This class is used to start firewall
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkStartFirewallReq : public NCNetworkReqBase
    {
    public:
        NCNetworkStartFirewallReq(const UINT32 type);
        virtual ~NCNetworkStartFirewallReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        UINT32 m_type;
        NC_DISABLE_COPY(NCNetworkStartFirewallReq);
    };

    /**
     * @brief This class is used to stop dhclient, dhcpd and named processes.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkStopCurrentProcessReq : public NCNetworkReqBase
    {
    public:
        NCNetworkStopCurrentProcessReq();
        virtual ~NCNetworkStopCurrentProcessReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NCNetworkStopCurrentProcessReq);
    };

    /**
     * @brief This class is used to set current default route
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkSetDefaultNetworkReq : public NCNetworkReqBase
    {
    public:
        NCNetworkSetDefaultNetworkReq(const UINT32 lastRequest, const DeviceRouteDnsInfo& info);
        virtual ~NCNetworkSetDefaultNetworkReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual VOID doTimeout();
        virtual UINT32 getLastRequest();
        virtual NCString getDeviceType();

    private:
        UINT32 m_lastRequest;
        NCString m_deviceType;
        NCString m_deviceName;
        NCString m_gateway;
        NCString m_dns1;
        NCString m_dns2;

        NC_DISABLE_COPY(NCNetworkSetDefaultNetworkReq);
    };


    /**
     * @brief This class is used to remove current default route
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkClearDefaultNetworkReq : public NCNetworkReqBase
    {
    public:
        NCNetworkClearDefaultNetworkReq(const UINT32 lastRequest, const NCString& deviceType, const NC_BOOL remoteFlag);
        virtual ~NCNetworkClearDefaultNetworkReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);
        virtual NCString getDeviceType();
        virtual UINT32 getLastRequest();
        virtual NC_BOOL getRemoteFlag();
        virtual VOID doTimeout();

    private:
        UINT32 m_lastRequest;
        NCString m_deviceType;
        NC_BOOL m_remoteFlag;

        NC_DISABLE_COPY(NCNetworkClearDefaultNetworkReq);
    };


    /**
     * @brief This class is used to check if NCM connected 
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkCheckNcmConnectReq : public NCNetworkReqBase
    {
    public:
        NCNetworkCheckNcmConnectReq();
        virtual ~NCNetworkCheckNcmConnectReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        UINT16 checkSum(UINT16 *addr, INT len);
        INT ping(const CHAR *ips, INT timeout);

        NC_DISABLE_COPY(NCNetworkCheckNcmConnectReq);
    };


    /**
     * @brief This class is used to set Extunit vendor.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkSetExtunitVendorReq : public NCNetworkReqBase
    {
    public:
        NCNetworkSetExtunitVendorReq();
        virtual ~NCNetworkSetExtunitVendorReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NC_DISABLE_COPY(NCNetworkSetExtunitVendorReq);
    };

    /**
     * @brief This class is used to start monitor data communication.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkStartMonitorDataCommReq : public NCNetworkReqBase
    {
    public:
        NCNetworkStartMonitorDataCommReq(const NCString& deviceType, const NCString& deviceName);
        virtual ~NCNetworkStartMonitorDataCommReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NCString m_deviceType;
        NCString m_deviceName;

        NC_DISABLE_COPY(NCNetworkStartMonitorDataCommReq);
    };

    /**
     * @brief This class is used to stop monitor data communication.
     *
     * This class will be posted in NCSpThread, which will run the doAction function.
     */
    class NCNetworkStopMonitorDataCommReq : public NCNetworkReqBase
    {
    public:
        NCNetworkStopMonitorDataCommReq(const NCString& deviceType, const NCString& deviceName);
        virtual ~NCNetworkStopMonitorDataCommReq();

        virtual VOID doAction(NCSpThread<NCNetworkReqBase>* pThread);

    private:
        NCString m_deviceType;
        NCString m_deviceName;
        
        NC_DISABLE_COPY(NCNetworkStopMonitorDataCommReq);
    };

} /* namespace nutshell */

#endif /* NCNETWORKREQ_H */
/* EOF */
