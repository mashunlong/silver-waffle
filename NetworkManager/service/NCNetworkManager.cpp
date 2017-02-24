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

#include <list>
#include <dlfcn.h>
#include <ncore/NCFile.h>
#include <ncore/NCIniFile.h>
#include <ncore/NCIniFileParser.h>
#include <netutils/ifc.h>
#include <ncore/NCLog.h>
#include <ncore/NCError.h>
#include <ncore/NCAutoSync.h>

#include <cutils/properties.h>
#include <system/PfServiceDefine.h>
#include <nceventsys/NCEvSysContext.h>
#include <nhinterface/NINetworkHandler.h>
#include <nhinterface/NIAvcLanHandler.h>
#include <nhinterface/NIBtHfpHandler.h>
#include <connectmanager/NCConnect.h>
#include <connectmanager/NCUsbManager.h>
#include "NCNetworkUsbGadgetnetReplier.h"

#include "networkmanager/NCNetworkDef.h"
#include "NCNetworkNotify.h"
#include "NCNetworkPluginCallbackImpl.h"
#include "NCNetworkPluginManager.h"
#include "NCNetworkDeviceType.h"
#include "NCNetworkAccessStack.h"
#include "NCNetworkStatus.h"
#include "NCNetworkSignal.h"
#include "NCNetworkAvcUnit.h"
#include "NCHttpClients.h"
#include "NCNetworkManagerDef.h"
#include "NCNetworkSignalListener.h"
#include "NCNetworkHandler.h"
#include "NCNetworkThread.h"
#include "NCNetworkReqConfirm.h"
#include "NCNetworkTimerThread.h"
#include "NCNetworkDiagLog.h"
#include "NCNetworkEventReq.h"
#include "NCNetworkManager.h"

#define NC_LOG_TAG "NCNetworkManager"
#define NCNETWORK_EVENT_THREAD "network.event"
#define NCNETWORK_SIGNAL_LISTENER_NAME "network_signal_listener"
#define NCNETWORK_ADB_DEBUG 1

namespace nutshell
{
    ncsp<NCNetworkManager>::wp NCNetworkManager::instance = NULL;

    NCNetworkManager::NCNetworkManager()
    : NEServiceStub(PFSERVICE_NETWORKMANAGER_NAME)
    , m_reqThread(NULL)
    , m_ncmThread(NULL)
    , m_eventThread(NULL)
    , m_timerThread(NULL)
    , m_eventHandler(NULL)
    , m_pluginManager(NULL)
    , m_httpClientService(NULL)
    , m_connectUsbManagerProxy(NULL)
    , m_connectUsbGadgetnetReplier(NULL)
    , m_deviceType(NULL)
    , m_accessStack(NULL)
    , m_netStatus(NULL)
    , m_signalListener(NULL)
    , m_networkAvcUnit(NULL)
    , m_diagLog(NULL)
    , m_serviceStatus(NetworkManagerStatus_Unknown)
    , m_firewallDefaultFlag(NC_FALSE)
    , m_firewallNcmFlag(NC_FALSE)
    , m_firewallDcmFlag(NC_FALSE)
    , m_firewallDefaultFailedFlag(NC_FALSE)
    , m_firewallNcmFailedFlag(NC_FALSE)
    , m_wifiApFlagRequest(NC_FALSE)
    , m_wifiApFlagOnTimer(NC_FALSE)
    , m_reqSeqId(0)
    , m_extUnitVendor("unknown")
    {
    }

    NCNetworkManager::~NCNetworkManager()
    {
    }

    VOID
    NCNetworkManager::onInit()
    {
        NCLOGD("NCNetworkManager::onInit - Start");
        setNetworkManagerStatus(NetworkManagerStatus_OnInitStart);

        if (NULL == m_reqThread) {
            m_reqThread = new NCNetworkThread(NCNETWORK_REQ_THREAD);
        }

        if (NULL == m_ncmThread) {
            m_ncmThread = new NCNetworkThread(NCNETWORK_NCM_THREAD);
        }

        if (NULL == m_eventThread) {
            m_eventThread = new NCRunnableThread();
        }

        if (NULL == m_timerThread) {
            m_timerThread = new NCNetworkTimerThread();
        }

        if (m_eventHandler == NULL) {
            m_eventHandler = new NCNetworkEventHandler(m_eventThread->looper());
        }

        if (m_pluginManager == NULL) {
            m_pluginManager = new NCNetworkPluginManager(new NCNetworkPluginCallbackImpl());
        }

        if (NULL == m_deviceType) {
            m_deviceType = new NCNetworkDeviceType();
        }

        if (NULL == m_accessStack) {
            m_accessStack = new NCNetworkAccessStack();
        }

        if (NULL == m_netStatus) {
            m_netStatus = new NCNetworkStatus();
        }

        if (NULL == m_networkAvcUnit) {
            m_networkAvcUnit = new NCNetworkAvcUnit();
        }

        if (NULL == m_httpClientService) {
            m_httpClientService = new NCHttpClients();
        }

        if (m_connectUsbManagerProxy == NULL) {
            m_connectUsbManagerProxy = new NCUsbManager();
        }

        if (m_connectUsbGadgetnetReplier == NULL) {
            m_connectUsbGadgetnetReplier = new NCNetworkUsbGadgetnetReplier();
        }

        if (NULL == m_signalListener) {
            m_signalListener = new NCNetworkSignalListener(NCNETWORK_SIGNAL_LISTENER_NAME);
        }

        if (NULL == m_diagLog) {
            m_diagLog = new NCNetworkDiagLog();
        }

        instance = this;

        setNetworkManagerStatus(NetworkManagerStatus_OnInitEnd);
    }

    VOID
    NCNetworkManager::onHibernate()
    {
        NCLOGD("NCNetworkManager::onHibernate - Start");
        setNetworkManagerStatus(NetworkManagerStatus_OnHibernateStart);

        if (NULL != m_eventThread) {
            m_eventThread->startThread(NCNETWORK_EVENT_THREAD);
        }

        NCNetworkHandler handler;
        if (!handler.readAvcLanIp(NCNETWORK_AVCLANIP_CONFIG_SECTION, m_avcLanIp)) {
            NCLOGD("NCNetworkManager::onHibernate - read AvcLan IP failed");
            handler.readAvcLanDefaultIp(m_avcLanIp);
        }

        // read adb debug info from config file
        if (!handler.readAdbDebugIp(m_adbDebugIp)) {
            NCLOGD("NCNetworkManager::onHibernate, Read adb debug ip failed");
            m_adbDebugIp.device = "eth0";
            m_adbDebugIp.ip = "192.168.5.111";
            m_adbDebugIp.prefixLength = 24;
        }

#if defined NCNETWORK_TARGET_PRODUCT_DCU
        if (!handler.readAvcLanIp(NCNEWORK_AVCLAN_IP_MEU_SECTION, m_remoteAvclanIp)) {
            NCLOGD("NCNetworkManager::onHibernate - Read remote avclan ip failed");
            handler.readRemoteAvcLanDefaultIp(m_remoteAvclanIp);
        }
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
        if (!handler.readVRServerRouteInfo(m_vrServerRoute)) {
            NCLOGD("NCNetworkManager::onHibernate, Read VRServer route info failed");
            m_vrServerRoute.deviceName = "gadncm0";
            m_vrServerRoute.ip = "192.168.1.1";
            m_vrServerRoute.prefixLength = 32;
            m_vrServerRoute.gateway = "192.168.128.129";
        }

        if (!handler.readAvcLanIp(NCNEWORK_AVCLAN_IP_DCU_SECTION, m_remoteAvclanIp)) {
            NCLOGD("NCNetworkManager::onHibernate - Read remote avclan ip failed");
            handler.readRemoteAvcLanDefaultIp(m_remoteAvclanIp);
        }
#endif

        NCEvSysContext cEvSysContext;
        std::list<NCString> filter;

        NCString eventSpace = GET_NIEVENTNAMESPACE(NIAvcLanHandler);
        UINT32 eventId = NIAvcLanHandler::EventId_ConnectStatus;
        NCString strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NIAvcLanHandler::EventId_IndividualApparatus;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventSpace = GET_NIEVENTNAMESPACE(NIBtHfpHandler);
        eventId = NIBtHfpHandler::CFM_HFP_GET_OPERATOR_NAME;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NIBtHfpHandler::IND_HFP_SERVICE;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventSpace = GET_NIEVENTNAMESPACE(NINetworkHandler);
        eventId = NINetworkHandler::EventId_DoDhcp;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NINetworkHandler::EventId_ReleaseDhcpLease;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NINetworkHandler::EventId_NetworkDeviceInfo;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NINetworkHandler::EventId_ConnectedClientInfo;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NINetworkHandler::EventId_RemoveDefaultRoute;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NINetworkHandler::EventId_AddDefaultRoute;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NINetworkHandler::EventId_StartDhcpServer;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NINetworkHandler::EventId_StopDhcpServer;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NINetworkHandler::EventId_RestartDhcpServer;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NINetworkHandler::EventId_SetStaticIpMaskMtu;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventId = NINetworkHandler::EventId_AddRoute;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventId);
        filter.push_back(strFilterItem);

        eventSpace = NCNETWORK_WIFI_EVENT_SPACENAME;
        NCString eventIdStr = NCNETWORK_WIFI_EVENT_P2P_DEVICE_DISCONNECTED;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventIdStr);
        filter.push_back(strFilterItem);

        eventIdStr = NCNETWORK_WIFI_EVENT_P2P_DEVICE_CONNECTED;
        strFilterItem = cEvSysContext.buildEventFullName(eventSpace, eventIdStr);
        filter.push_back(strFilterItem);

        cEvSysContext.registerHandler(m_eventHandler, filter);

        if (m_pluginManager != NULL) {
            m_pluginManager->onInit();
        }

        // Start avc unit
        if (NULL != m_networkAvcUnit) {
            m_networkAvcUnit->start(m_eventThread->looper());
        }

        // Start network signal listener
        if (NULL != m_signalListener) {
            NCString uriPrefix = NCString(SYSTEM_INFO_URI) + NCString("?");

            NCUri uriWifiStrength(uriPrefix + NCString(WIFI_STRENGTH_KEY));
            NCUri uriDcmStrength(uriPrefix + NCString(DCM_STRTHGTH_KEY));
            NCUri uriBtStrength(uriPrefix + NCString(BT_STRTHGTH_KEY));
            NCUri uriDcmDev(uriPrefix + NCString(DCM_DEV_Key));
            NCUri uriDunDev(uriPrefix + NCString(BTDUN_DEV_KEY));

            std::list<NCUri> uriList;
            uriList.push_back(uriWifiStrength);
            uriList.push_back(uriDcmStrength);
            uriList.push_back(uriBtStrength);
            uriList.push_back(uriDcmDev);
            uriList.push_back(uriDunDev);

            m_signalListener->listenUri(uriList);
        }

        // Config firewall default rule
        postRequest(new NCNetworkStartFirewallReq(FirewallType_Default));

        // Initialize /var/db/dhclient.leases
        postRequest(new NCNetworkInitDhcpLeaseFileReq());

        // bind ConnectManager and set replier
        m_connectUsbManagerProxy->setReplyReceiver(m_connectUsbGadgetnetReplier);
        m_connectUsbManagerProxy->bindService();

        setNetworkManagerStatus(NetworkManagerStatus_OnHibernateEnd);
    }

    VOID
    NCNetworkManager::onAwake()
    {
        NCLOGD("NCNetworkManager::onAwake - Start");
        setNetworkManagerStatus(NetworkManagerStatus_onAwakeStart);

        // Start diag log
        if (NULL != m_diagLog) {
            m_diagLog->startDiagLog();
        }

        // Notify plugin onAwake
        if (m_pluginManager != NULL) {
            m_pluginManager->onStart();
        }

        // Init /tmp/resolv.conf
        postRequest(new NCNetworkInitResolvConfReq());

        // Initialize /var/db/dhclient.leases
        postRequest(new NCNetworkInitDhcpLeaseFileReq());

        // Initialize /var/run/dhclient.pid and /var/run/dhcpd.pid
        postRequest(new NCNetworkInitDhcpPidFileReq());

        // Initialize /var/dhcpd.conf
        postRequest(new NCNetworkInitDhcpdConfReq());

#if NCNETWORK_ADB_DEBUG
        postRequest(new NCNetworkSetAdbDebugIpReq());
#endif

        setNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd);
    }

    VOID
    NCNetworkManager::onDeinit()
    {
        NCLOGD("NCNetworkManager::onDeinit - Start");
        setNetworkManagerStatus(NetworkManagerStatus_onDeinitStart);

        if (NULL != m_reqThread) {
            delete m_reqThread;
            m_reqThread = NULL;
        }

        if (NULL != m_ncmThread) {
            delete m_ncmThread;
            m_ncmThread = NULL;
        }

        if (NULL != m_networkAvcUnit) {
            delete m_networkAvcUnit;
            m_networkAvcUnit = NULL;
        }

        if (NULL !=  m_eventThread) {
            delete m_eventThread;
            m_eventThread = NULL;
        }

        if (NULL != m_timerThread) {
            delete m_timerThread;
            m_timerThread = NULL;
        }

        if (NULL != m_deviceType) {
            delete m_deviceType;
            m_deviceType = NULL;
        }

        if (NULL != m_accessStack) {
            delete m_accessStack;
            m_accessStack = NULL;
        }

        if (NULL != m_netStatus) {
            delete m_netStatus;
            m_netStatus = NULL;
        }

        if (NULL != m_httpClientService) {
            delete m_httpClientService;
            m_httpClientService = NULL;
        }

        if (NULL != m_signalListener) {
            delete m_signalListener;
            m_signalListener = NULL;
        }

        if (NULL != m_diagLog) {
            delete m_diagLog;
            m_diagLog = NULL;
        }

        setNetworkManagerStatus(NetworkManagerStatus_onDeinitEnd);
    }

    VOID
    NCNetworkManager::onStop()
    {
        NCLOGD("NCNetworkManager::onStop - Start");
        setNetworkManagerStatus(NetworkManagerStatus_onStopStart);

        NCEvSysContext cEvSysContext;
        std::list<NCString> filter;
        cEvSysContext.unregisterHandler(m_eventHandler, filter);

        // unbind ConnectManager
        m_connectUsbManagerProxy->unbindService();

        // Stop avc unit
        if (NULL != m_networkAvcUnit) {
            m_networkAvcUnit->stop();
        }

        if (NULL != m_reqThread) {
            m_reqThread->stopThread(new NCNetworkStopCurrentProcessReq(), NC_TRUE);
        }

        if (NULL != m_eventThread) {
            m_eventThread->stopThread();
        }

        if (NULL != m_timerThread) {
            m_timerThread->stopThread();
        }

        if (NULL != m_diagLog) {
            m_diagLog->stopDiagLog();
        }

        setNetworkManagerStatus(NetworkManagerStatus_onStopEnd);
    }

    ncsp<NCNetworkManager>::sp
    NCNetworkManager::Instance()
    {
        return instance.promote();
    }

    NC_BOOL
    NCNetworkManager::onCommand(UINT32 code, const NCParcel& data, NCParcel* reply, UINT32 flags)
    {
        NC_BOOL ret = NC_FALSE;

        if (code > FunctionId_HttpClienBegin && code < FunctionId_HttpClienEnd) {
            m_httpClientService->onCommand(code, data, reply, flags);
            ret = NC_TRUE;
            return ret;
        }

        switch (code) {
        case FunctionId_Request:
        {
            NCLOGD("NCNetworkManager::onCommand - Request");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - Request, NetworkManager is not ready");
                break;
            }

            AccessParameter param;
            param.reqId = getRequestSequenceId();
            param.reqType = RequestType_Access;
            param.resourceId = data.readCString();
            param.deviceType = data.readCString();
            param.dealType = data.readInt32();
            param.commPriority = data.readInt32();

            if (NULL != m_eventThread) {
                m_eventThread->looper().postRunnable(new NCNetworkCheckRequstReq(param));
            }

            ret = NC_TRUE;
        }
        break;
        case FunctionId_RemoteRequest:
        {
            NCLOGD("NCNetworkManager::onCommand - RemoteRequest");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - RemoteRequest, NetworkManager is not ready");
                break;
            }

            AccessParameter param;
            param.reqId = getRequestSequenceId();
            param.reqType = RequestType_Access;
            param.resourceId = data.readCString();
            param.deviceType = data.readCString();
            param.dealType = data.readInt32();
            param.commPriority = data.readInt32();
            param.remoteFlag = NC_TRUE;

            if (NULL != m_eventThread) {
                m_eventThread->looper().postRunnable(new NCNetworkCheckRequstReq(param));
            }
            
            ret = NC_TRUE;
        }
        break;
        case FunctionId_RequestWithSequenceId:
        {
            NCLOGD("NCNetworkManager::onCommand - RequestWithSequenceId");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_OnHibernateEnd)) {
                NCLOGD("NCNetworkManager::onCommand - RequestWithSequenceId, NetworkManager is not ready");
                reply->writeInt32(0);
                break;
            }

            AccessParameter param;
            param.reqId = getRequestSequenceId();
            param.reqType = RequestType_Access;
            param.deviceType = data.readCString();
            param.resourceId = data.readCString();
            param.dealType = data.readInt32();
            param.commPriority = data.readInt32();
            param.reqIdFlag = NC_TRUE;

            if (NULL != m_eventThread) {
                m_eventThread->looper().postRunnable(new NCNetworkCheckRequstReq(param));
            }
            
            if (NULL != reply) { 
                reply->writeInt32(param.reqId);
            }

            ret = NC_TRUE;
        }
        break;
        case FunctionId_Release:
        {
            NCLOGD("NCNetworkManager::onCommand - Release");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - Release, NetworkManager is not ready");
                break;
            }

            AccessParameter param;
            param.reqId = getRequestSequenceId();
            param.reqType = RequestType_Release;
            param.resourceId = data.readCString();

            if (NULL != m_eventThread) {
                m_eventThread->looper().postRunnable(new NCNetworkCheckReleaseReq(param));
            }

            ret = NC_TRUE;
        }
        break;
        case FunctionId_ReleaseWithSequenceId:
        {
            NCLOGD("NCNetworkManager::onCommand - ReleaseWithSequenceId");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - ReleaseWithSequenceId, NetworkManager is not ready");
                reply->writeInt32(0);
                break;
            }

            AccessParameter param;
            param.reqId = getRequestSequenceId();
            param.reqType = RequestType_Release;
            param.resourceId = data.readCString();
            param.reqIdFlag = NC_TRUE;

            if (NULL != m_eventThread) {
                m_eventThread->looper().postRunnable(new NCNetworkCheckReleaseReq(param));
            }

            if (NULL != reply) { 
                reply->writeInt32(param.reqId);
            }

            ret = NC_TRUE;
        }
        break;
        case FunctionId_Disconnect:
        {
            NCLOGD("NCNetworkManager::onCommand - Disconnect");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - Disconnect, NetworkManager is not ready");
                break;
            }

            AccessParameter param;
            param.reqId = getRequestSequenceId();
            param.reqType = RequestType_Disconnect;
            if (NULL != m_netStatus) {
                param.deviceType = m_netStatus->currentNetwork();
            }

            if (NULL != m_eventThread) {
                m_eventThread->looper().postRunnable(new NCNetworkCheckDisconnectReq(param));
            }

            ret = NC_TRUE;
        }
        break;
        case FunctionId_DisconnectNetwork:
        {
            NCLOGD("NCNetworkManager::onCommand - DisconnectNetwork");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - DisconnectNetwork, NetworkManager is not ready");
                break;
            }

            AccessParameter param;
            param.reqId = getRequestSequenceId();
            param.reqType = RequestType_Disconnect;
            param.deviceType = data.readCString();

            if (NULL != m_eventThread) {
                m_eventThread->looper().postRunnable(new NCNetworkCheckDisconnectReq(param));
            }

            ret = NC_TRUE;
        }
        break;
        case FunctionId_DisconnectWithSequenceId:
        {
            NCLOGD("NCNetworkManager::onCommand - DisconnectNetwork");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - DisconnectNetwork, NetworkManager is not ready");
                break;
            }

            AccessParameter param;
            param.reqId = getRequestSequenceId();
            param.reqType = RequestType_Disconnect;
            param.deviceType = data.readCString();
            param.reqIdFlag = NC_TRUE;

            if (NULL != m_eventThread) {
                m_eventThread->looper().postRunnable(new NCNetworkCheckDisconnectReq(param));
            }

            if (NULL != reply) {
                reply->writeInt32(param.reqId);
            }

            ret = NC_TRUE;
        }
        break;
        case FunctionId_RemoteDisconnect:
        {
            NCLOGD("NCNetworkManager::onCommand - RemoteDisconnect");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - RemoteDisconnect, NetworkManager is not ready");
                break;
            }

            AccessParameter param;
            param.reqId = getRequestSequenceId();
            param.reqType = RequestType_Disconnect;
            param.deviceType = data.readCString();
            param.remoteFlag = NC_TRUE;

            if (NULL != m_eventThread) {
                m_eventThread->looper().postRunnable(new NCNetworkCheckDisconnectReq(param));
            }

            ret = NC_TRUE;
        }
        break;
        case FunctionId_SetSlipDevice:
        {
            NCLOGD("NCNetworkManager::onCommand - SetSlipDevice");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_OnHibernateEnd)) {
                NCLOGD("NCNetworkManager::onCommand - SetSlipDevice, NetworkManager is not ready");
                break;
            }

            NCNetworkAccess::SlipDeviceInfo info;
            info.deviceName = data.readCString();
            info.deviceType = data.readCString();
            info.ip = data.readCString();
            info.mask = data.readInt32();
            info.mtu = data.readInt32();
            postRequest(new NCNetworkSetSlipDeviceReq(info));
            ret = NC_TRUE;
        }
        break;
        case FunctionId_NotifySlipDisconnect:
        {
            NCLOGD("NCNetworkManager::onCommand - NotifySlipDisconnect");
            NCString deviceName = data.readCString();
            NCString deviceType;
            if (m_deviceType->getType(deviceName, deviceType)) {
                m_netStatus->setStatus(deviceType, NC_FALSE);
            }
            ret = NC_TRUE;
        }
        break;
        case FunctionId_NotifyDisconnectResult:
        {
            NCLOGD("NCNetworkManager::onCommand - NotifyDisconnectResult");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - NotifyDisconnectResult, NetworkManager is not ready");
                break;
            }

            AccessParameter param;
            param.reqId = getRequestSequenceId();
            param.reqType = RequestType_NotifyDisconnectResult;
            param.deviceType = data.readCString();

            if (NULL != m_eventThread) {
                m_eventThread->looper().postRunnable(new NCNetworkCheckDisconnectReq(param));
            }

            ret = NC_TRUE;
        }
        break;
        case FunctionId_RequestIp:
        {
            NCLOGD("NCNetworkManager::onCommand - RequestIp");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - RequestIp, NetworkManager is not ready");
                break;
            }

            NCString deviceType(data.readCString());
            NCString deviceName(data.readCString());

            m_reqThread->removeSimilarDhcpRequestInList(RequestType_RequestIp, deviceType, deviceName);
            postRequest(new NCNetworkRequestIpReq("", deviceType, deviceName));
            ret = NC_TRUE;
        }
        break;
        case FunctionId_RequestIpResourceId:
        {
            NCLOGD("NCNetworkManager::onCommand - RequestIpResourceId");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - RequestIpResourceId, NetworkManager is not ready");
                break;
            }

            NCString resourceId(data.readCString());
            NCString deviceType(data.readCString());
            NCString deviceName(data.readCString());

            m_reqThread->removeSimilarDhcpRequestInList(RequestType_RequestIp, deviceType, deviceName);
            postRequest(new NCNetworkRequestIpReq(resourceId, deviceType, deviceName));
            ret = NC_TRUE;
        }
        break;
        case FunctionId_ReleaseIp:
        {
            NCLOGD("NCNetworkManager::onCommand - ReleaseIp");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - ReleaseIp, NetworkManager is not ready");
                break;
            }

            NCString deviceType(data.readCString());
            NCString deviceName(data.readCString());
            m_reqThread->removeSimilarDhcpRequestInList(RequestType_ReleaseIp, deviceType, deviceName);
            postRequest(new NCNetworkReleaseIpReq(deviceType, deviceName));
            ret = NC_TRUE;
        }
        break;
        case FunctionId_StartDhcpServer:
        {
            NCLOGD("NCNetworkManager::onCommand - StartDhcpServer");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - StartDhcpServer, NetworkManager is not ready");
                break;
            }

            NCString deviceType(data.readCString());
            NCString curDhcpServer = m_netStatus->currentDhcpServer();
            if (NCString("") != curDhcpServer && deviceType != curDhcpServer) {
                NCLOGD("NCNetworkManager::onCommand - StartDhcpServer, Current server [%s] != request server [%s]", curDhcpServer.getString(), deviceType.getString());
                NCString serverName;
                m_deviceType->getName(curDhcpServer, serverName);
                postRequest(new NCNetworkStopDhcpServerReq(curDhcpServer, serverName));
            }
            
            NCString deviceName(data.readCString());
            postRequest(new NCNetworkStartDhcpServerReq(deviceType, deviceName));
            ret = NC_TRUE;
        }
        break;
        case FunctionId_StopDhcpServer:
        {
            NCLOGD("NCNetworkManager::onCommand - StopDhcpServer");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - StopDhcpServer, NetworkManager is not ready");
                break;
            }

            NCString deviceType(data.readCString());
            NCString deviceName(data.readCString());
            postRequest(new NCNetworkStopDhcpServerReq(deviceType, deviceName));
            ret = NC_TRUE;
        }
        break;
        case FunctionId_CheckDhcpServerStatus:
        {
            NCLOGD("NCNetworkManager::onCommand - CheckDhcpServerStatus");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - CheckDhcpServerStatus, NetworkManager is not ready");
                break;
            }

            NC_BOOL status = NC_FALSE;
            NCString pidFile = DHCPD_PID_FILE;
            if (NCFile::exists(pidFile)) {
                status = NC_TRUE;
            }

            reply->writeInt32(status);
            ret = NC_TRUE;
        }
        break;
        case FunctionId_StartMonitorDataComm:
        {
            NCLOGD("NCNetworkManager::onCommand - StartMonitorDataComm");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - StartMonitorDataComm, NetworkManager is not ready");
                break;
            }

            NCString deviceType(data.readCString());
            NCString deviceName(data.readCString());
            postRequest(new NCNetworkStartMonitorDataCommReq(deviceType, deviceName));
            ret = NC_TRUE;

        }
        break;
        case FunctionId_StopMonitorDataComm:
        {
            NCLOGD("NCNetworkManager::onCommand - StopMonitorDataComm");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_onAwakeEnd)) {
                NCLOGD("NCNetworkManager::onCommand - StopMonitorDataComm, NetworkManager is not ready");
                break;
            }

            NCString deviceType(data.readCString());
            NCString deviceName(data.readCString());
            postRequest(new NCNetworkStopMonitorDataCommReq(deviceType, deviceName));
            ret = NC_TRUE;
        }
        break;
        case FunctionId_DeviceInfo:
        {
            NCLOGD("NCNetworkManager::onCommand - DeviceInfo");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_OnHibernateEnd)) {
                NCLOGD("NCNetworkManager::onCommand - DeviceInfo, NetworkManager is not ready");
                break;
            }

            NCString deviceType(data.readCString());
            postRequest(new NCNetworkDeviceInfoReq(deviceType));
            ret = NC_TRUE;
        }
        break;
        case FunctionId_ConnectedClientInfo:
        {
            NCLOGD("NCNetworkManager::onCommand - ConnectedClientInfo");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_OnHibernateEnd)) {
                NCLOGD("NCNetworkManager::onCommand - ConnectedClientInfo, NetworkManager is not ready");
                break;
            }

            NCString mac(data.readCString());
            postRequest(new NCNetworkConnectedClientInfo(mac));
            ret = NC_TRUE;
        }
        break;
        case FunctionId_CurrentUseNetwork:
        {
            NCLOGD("NCNetworkManager::onCommand - CurrentUseNetwork");
            NCString currNet = m_netStatus->currentNetwork();
            reply->writeCString(currNet.getString());
            ret = NC_TRUE;
        }
        break;
        case FunctionId_DeviceStatus:
        {
            NCLOGD("NCNetworkManager::onCommand - DeviceStatus");
            NCString deviceType(data.readCString());
            NC_BOOL status = m_netStatus->status(deviceType);
            reply->writeInt32(status);
            ret = NC_TRUE;
        }
        break;
        case FunctionId_NetworkSignal:
        {
            NCLOGD("NCNetworkManager::onCommand - NetworkSignal");
            NCString deviceType(data.readCString());
            NCNetworkSignal mySignal;
            NC_BOOL status = mySignal.signal(deviceType);
            reply->writeInt32(status);
            ret = NC_TRUE;
        }
        break;
        case FunctionId_UpdateGbookServerDns:
        {
            NCLOGD("NCNetworkManager::onCommand - Update gbook server dns");
            if (!checkNetworkManagerStatus(NetworkManagerStatus_OnHibernateEnd)) {
                NCLOGD("NCNetworkManager::onCommand - ConnectedClientInfo, NetworkManager is not ready");
                break;
            }

            NCString dcmDns1(data.readCString());
            NCString dcmDns2(data.readCString());
            NCString wifiDns1(data.readCString());
            NCString wifiDns2(data.readCString());
            postRequest(new NCNetworkSetGbookServerDnsReq(dcmDns1, dcmDns2, wifiDns1, wifiDns2));
            ret = NC_TRUE;
        }
        break;
        default:
        {
            NCLOGD("NCNetworkManager::onCommand - Error code[0x%x]", code);
        }
        break;
        }
        return ret;
    }

    DeviceIpInfo
    NCNetworkManager::getAvcLanIpInfo()
    {
        NCAutoSync lock(m_syncAvcLanIp);
        return m_avcLanIp;
    }

    VOID
    NCNetworkManager::setAvcLanIpInfo(const DeviceIpInfo& info)
    {
        NCAutoSync lock(m_syncAvcLanIp);
        m_avcLanIp = info;
    }

    DeviceIpInfo
    NCNetworkManager::getRemoteAvcLanIpInfo()
    {
        NCAutoSync lock(m_syncRemoteAvclanIp);
        return m_remoteAvclanIp;
    }

    DeviceIpInfo
    NCNetworkManager::getAdbDebugIpInfo()
    {
        return m_adbDebugIp;
    }


    RouteInfo
    NCNetworkManager::getRouteInfo()
    {
        return m_vrServerRoute;
    }

    NC_BOOL
    NCNetworkManager::getFirewallFlag(const UINT32 type)
    {
        NCAutoSync lock(m_syncFirewall);
        if (FirewallType_Default == type) {
            return m_firewallDefaultFlag;
        }
        else if (FirewallType_Ncm == type) {
            return m_firewallNcmFlag;
        }
        else if (FirewallType_Dcm == type) {
            return m_firewallDcmFlag;
        }
        else {
            NCLOGD("NCNetworkManager::getFirewallFlag, type [%d] failed", type);
            return NC_FALSE;
        }
    }

    VOID
    NCNetworkManager::setFirewallFlag(const UINT32 type, NC_BOOL flag)
    {
        NCAutoSync lock(m_syncFirewall);
        if (FirewallType_Default == type) {
            m_firewallDefaultFlag = flag;
        }
        else if (FirewallType_Ncm == type) {
            m_firewallNcmFlag = flag;
        }
        else if (FirewallType_Dcm == type) {
            m_firewallDcmFlag = flag;
        }
        else {
            NCLOGD("NCNetworkManager::setFirewallFlag, type [%d] failed", type);
        }
    }

    VOID
    NCNetworkManager::setDefaultNcmFlag(NC_BOOL defaultFlag, NC_BOOL ncmFlag)
    {
        NCLOGD("NCNetworkManager::setDefaultNcmFlag, Start, defaultFlag [%d], ncmFlag [%d]", defaultFlag, ncmFlag);
        NCAutoSync lock(m_syncFirewall);
        setFirewallFlag(FirewallType_Default, defaultFlag);
        setFirewallFlag(FirewallType_Ncm, ncmFlag);
    }

    NC_BOOL
    NCNetworkManager::getFirewallFailedFlag(const UINT32 type)
    {
        NCAutoSync lock(m_syncFirewallFailed);
        if (FirewallType_Default == type) {
            return m_firewallDefaultFailedFlag;
        }
        else if (FirewallType_Ncm == type) {
            return m_firewallNcmFailedFlag;
        }
        else {
            NCLOGD("NCNetworkManager::getFirewallFailedFlag, type [%d] failed", type);
            return NC_FALSE;
        }
    }

    VOID
    NCNetworkManager::setFirewallFailedFlag(const NC_BOOL defaultFailedFlag, const NC_BOOL ncmFailedFlag)
    {
        NCAutoSync lock(m_syncFirewallFailed);
        m_firewallDefaultFailedFlag = defaultFailedFlag;
        m_firewallNcmFailedFlag = ncmFailedFlag;
    }

    NCNetworkDeviceType*
    NCNetworkManager::getNetworkDeviceType()
    {
        return m_deviceType;
    }

    NCNetworkAccessStack*
    NCNetworkManager::getAccessStack()
    {
        return m_accessStack;
    }

    NCNetworkStatus*
    NCNetworkManager::getNetworkStatus()
    {
        return m_netStatus;
    }

    NCNetworkAvcUnit*
    NCNetworkManager::getNetworkAvcUnit()
    {
        return m_networkAvcUnit;
    }

    NCNetworkPluginManager*
    NCNetworkManager::getNetworkPluginManager()
    {
        return m_pluginManager.get();
    }

    NCNetworkThread*
    NCNetworkManager::getNetworkThread()
    {
        return m_reqThread;
    }

    NCNetworkDiagLog*
    NCNetworkManager::getDiagLogPointer()
    {
        return m_diagLog;
    }

    NCRunnableThread*
    NCNetworkManager::getEventThreadPointer()
    {
        return m_eventThread;
    }

    VOID
    NCNetworkManager::postRequest(NCNetworkReqBase* req)
    {
        NCLOGD("NCNetworkManager::postRequest - Start");
        if (NULL != m_reqThread && NULL != req) {
            m_reqThread->postRequest(req);
        }
        else {
            NCLOGD("NCNetworkManager::postRequest, failed");
        }
    }

    VOID
    NCNetworkManager::postNcmThreadRequest(NCNetworkReqBase* req)
    {
        NCLOGD("NCNetworkManager::postNcmThreadRequest - Start");
        if (NULL != m_ncmThread && NULL != req) {
            m_ncmThread->postRequest(req);
        }
        else {
            NCLOGD("NCNetworkManager::postNcmThreadRequest, failed");
        }
    }

    VOID
    NCNetworkManager::insertRequest(NCNetworkReqBase* req)
    {
        NCLOGD("NCNetworkManager::insertRequest - Start");
        if (NULL != m_reqThread && NULL != req) {
            m_reqThread->insertRequest(req);
        }
        else {
            NCLOGD("NCNetworkManager::insertRequest, failed");
        }
    }

    VOID
    NCNetworkManager::startTimer(const UINT32 timerType)
    {
        m_timerThread->startTimer(timerType);
    }

    VOID
    NCNetworkManager::stopTimer(const UINT32 timerType)
    {
        m_timerThread->stopTimer(timerType);
    }

    VOID
    NCNetworkManager::startWifiApTimer()
    {
        NCLOGD("NCNetworkManager::startWifiApTimer, Start");
        if (DCM == m_netStatus->currentNetwork()) {
            NCLOGD("NCNetworkManager::startWifiApTimer, Current network is dcm");
            return;
        }

        if (!m_netStatus->status(WIFI_AP)) {
            NCLOGD("NCNetworkManager::startWifiApTimer, WIFI AP is not running");
            return;
        }

        if (!m_netStatus->getWifiApConnectedStatus()) {
            NCLOGD("NCNetworkManager::startWifiApTimer, No device connect to WIFI AP");
            return;
        }

        if (!NCFile::exists(WIFI_AP_TRAFFIC_RX_FILE) || !NCFile::exists(WIFI_AP_TRAFFIC_TX_FILE)) {
            NCLOGD("NCNetworkManager::startWifiApTimer, WLAN0 is not exists");
            return;
        }

        NCConnect connectProxy;
        if (!connectProxy.verifyPermission(WIFI_AP_RESOURCE_ID)) {
            NCLOGD("NCNetworkManager::startWifiApTimer, WIFI AP has not permission");
            return;
        }

        NC_BOOL storeRet = m_netStatus->storeWifiApTraffic();
        if (storeRet) {
            m_timerThread->startTimer(TimerType_WifiAp);
        }
    }

    NC_BOOL
    NCNetworkManager::getWifiApFlag(const UINT32 type)
    {
        NCAutoSync lock(m_syncWifiApFlag);
        if (WifiApTimerFlag_OnTimer == type) {
            return m_wifiApFlagOnTimer;
        }
        else {
            NCLOGD("NCNetworkManager::getWifiApFlag, type [%d] failed", type);
            return NC_FALSE;
        }
    }

    VOID
    NCNetworkManager::setWifiApFlag(const UINT32 type, const NC_BOOL flag)
    {
        NCAutoSync lock(m_syncWifiApFlag);
        if (WifiApTimerFlag_OnTimer == type) {
            m_wifiApFlagOnTimer = flag;
        }
        else {
            NCLOGD("NCNetworkManager::setWifiApFlag, type [%d] failed", type);
        }

    }

    UINT32
    NCNetworkManager::getRequestSequenceId()
    {
        NCAutoSync lock(m_syncReqSeqId);
        if (m_reqSeqId == MAXDWORD) {
            m_reqSeqId = 1;
        }
        else {
            ++m_reqSeqId;
        }

        NCLOGD("NCNetworkManager::getRequestSequenceId, id = [%d]", m_reqSeqId);
        return m_reqSeqId;
    }

    NCString
    NCNetworkManager::getExtunitVendor()
    {
        NCLOGD("NCNetworkManager::getExtunitVendor, Start");
        NCAutoSync lock(m_syncExtUnitVendor);
        NCLOGD("NCNetworkManager::getExtunitVendor, extunitVendor [%s]", m_extUnitVendor.getString());
        return m_extUnitVendor;
    }

    VOID
    NCNetworkManager::setExtunitVendor(const NCString& vendor)
    {
        NCLOGD("NCNetworkManager::setExtunitVendor, Start, vendor [%s]", vendor.getString());
        NCAutoSync lock(m_syncExtUnitVendor);
        m_extUnitVendor = vendor;
    }

    NC_BOOL
    NCNetworkManager::checkNetworkManagerStatus(const UINT32 status)
    {
        NCLOGD("NCNetworkManager::checkNetworkManagerStatus, Start, Request status [%d]", status);
        NCAutoSync lock(m_syncServiceStatus);
        NCLOGD("NCNetworkManager::checkNetworkManagerStatus, Start, Current status [%d]", m_serviceStatus);
        if (NetworkManagerStatus_OnHibernateEnd == status) {
            if (NetworkManagerStatus_OnHibernateEnd == m_serviceStatus || NetworkManagerStatus_onAwakeStart == m_serviceStatus 
                || NetworkManagerStatus_onAwakeEnd == m_serviceStatus) {
                return NC_TRUE;
            }
            else {
                return NC_FALSE;
            }
        }
        else if (NetworkManagerStatus_onAwakeEnd) {
            if (NetworkManagerStatus_onAwakeEnd == m_serviceStatus) {
                return NC_TRUE;
            }
            else {
                return NC_FALSE;
            }
        }
        else {
            NCLOGD("NCNetworkManager::checkNetworkManagerStatus, Parameters error");
            return NC_FALSE;
        }
    }


    VOID
    NCNetworkManager::setNetworkManagerStatus(const UINT32 status)
    {
        NCLOGD("NCNetworkManager::setNetworkManagerStatus, Start, status [%d]", status);
        NCAutoSync lock(m_syncServiceStatus);
        m_serviceStatus = status;
    }



} /* namespace nutshell */
/* EOF */
