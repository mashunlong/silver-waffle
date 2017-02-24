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
 * @file NCNetworkManager.h
 * @brief Declaration file of class NCNetworkManager.
 *
 * This file includes the declaration of class NCNetworkManager, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKMANAGER_H
#define NCNETWORKMANAGER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <map>
#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCSyncObj.h>
#include <nceventsys/NCRunnableThread.h>
#include <evipservice/NEServiceStub.h>
#include "NCNetworkManagerDef.h"
#include "NCNetworkReq.h"
#include "NCNetworkEventHandler.h"

namespace nutshell
{
    class NCNetworkPluginManager;
    class NCHttpClients;
    class NCNetworkDeviceType;
    class NCNetworkAccessStack;
    class NCNetworkStatus;
    class NCNetworkAvcUnit;
    class NCNetworkSignalListener;
    class NCNetworkThread;
    class NCNetworkTimerThread;
    class NCNetworkDiagLog;
    class NCUsbManager;
    class NCUsbGadgetnetReplier;

    /**
     * @brief Class of NCNetworkManager.
     *
     * This class will accept the command from proxy to manage network.
     */
    class NCNetworkManager : public NEServiceStub
    {

    public:

        NCNetworkManager();
        virtual ~NCNetworkManager();

        /// Derived from NEIpcService, service life cycle
        VOID onInit();
        VOID onHibernate();
        VOID onAwake();
        VOID onDeinit();
        VOID onStop();

        static ncsp<NCNetworkManager>::sp Instance();
        /// Derived from NCBinderService, receive sync request
        virtual NC_BOOL onCommand(UINT32 code, const NCParcel& data, NCParcel* reply, UINT32 flags);

        DeviceIpInfo getAvcLanIpInfo();
        VOID setAvcLanIpInfo(const DeviceIpInfo& info);

        DeviceIpInfo getRemoteAvcLanIpInfo();

        DeviceIpInfo getAdbDebugIpInfo();
        RouteInfo getRouteInfo();

        NC_BOOL getFirewallFlag(const UINT32 type);
        VOID setFirewallFlag(const UINT32 type, NC_BOOL flag);
        VOID setDefaultNcmFlag(NC_BOOL defaultFlag, NC_BOOL ncmFlag);

        NC_BOOL getFirewallFailedFlag(const UINT32 type);
        VOID setFirewallFailedFlag(const NC_BOOL defaultFailedFlag, const NC_BOOL ncmFailedFlag);

        NCNetworkDeviceType* getNetworkDeviceType();
        NCNetworkAccessStack* getAccessStack();
        NCNetworkStatus* getNetworkStatus();
        NCNetworkAvcUnit* getNetworkAvcUnit();
        NCNetworkPluginManager* getNetworkPluginManager();
        NCNetworkThread* getNetworkThread();
        NCNetworkDiagLog* getDiagLogPointer();
        NCRunnableThread* getEventThreadPointer();

        VOID postRequest(NCNetworkReqBase* req);
        VOID insertRequest(NCNetworkReqBase* req);
        
        VOID postNcmThreadRequest(NCNetworkReqBase* req);

        VOID startTimer(const UINT32 timerType);
        VOID stopTimer(const UINT32 timerType);

        VOID startWifiApTimer();

        NC_BOOL getWifiApFlag(const UINT32 type);
        VOID setWifiApFlag(const UINT32 type, const NC_BOOL flag);

        UINT32 getRequestSequenceId();

        NCString getExtunitVendor();
        VOID setExtunitVendor(const NCString& vendor);

    private:
        NC_BOOL checkNetworkManagerStatus(const UINT32 status);
        VOID setNetworkManagerStatus(const UINT32 status);

        static ncsp<NCNetworkManager>::wp instance;

        NCNetworkThread* m_reqThread;
        NCNetworkThread* m_ncmThread;
        NCRunnableThread* m_eventThread;
        NCNetworkTimerThread* m_timerThread;
        NCNetworkEventHandlerStrongHolder m_eventHandler;
        ncsp<NCNetworkPluginManager>::sp m_pluginManager;
        NCHttpClients* m_httpClientService;

        ncsp<NCUsbManager>::sp m_connectUsbManagerProxy;
        ncsp<NCUsbGadgetnetReplier>::sp m_connectUsbGadgetnetReplier;

        NCNetworkDeviceType* m_deviceType; 
        NCNetworkAccessStack* m_accessStack;
        NCNetworkStatus* m_netStatus;
        NCNetworkAvcUnit* m_networkAvcUnit;
        NCNetworkSignalListener* m_signalListener;
        NCNetworkDiagLog* m_diagLog;

        DeviceIpInfo m_avcLanIp;
        NCSyncObj m_syncAvcLanIp;

        DeviceIpInfo m_remoteAvclanIp;
        NCSyncObj m_syncRemoteAvclanIp;

        DeviceIpInfo m_adbDebugIp;

        RouteInfo m_vrServerRoute;

        UINT32 m_serviceStatus;
        NCSyncObj m_syncServiceStatus;

        NC_BOOL m_firewallDefaultFlag;
        NC_BOOL m_firewallNcmFlag;
        NC_BOOL m_firewallDcmFlag;
        NCSyncObj m_syncFirewall;

        NC_BOOL m_firewallDefaultFailedFlag;
        NC_BOOL m_firewallNcmFailedFlag;
        NCSyncObj m_syncFirewallFailed;

        NC_BOOL m_wifiApFlagRequest;
        NC_BOOL m_wifiApFlagOnTimer;
        NCSyncObj m_syncWifiApFlag;

        UINT32 m_reqSeqId;
        NCSyncObj m_syncReqSeqId;

        NCString m_extUnitVendor;
        NCSyncObj m_syncExtUnitVendor;

        NC_DISABLE_COPY(NCNetworkManager);
    };

} /* namespace nutshell */
#endif /* NCNETWORKMANAGER_H */
/* EOF */
