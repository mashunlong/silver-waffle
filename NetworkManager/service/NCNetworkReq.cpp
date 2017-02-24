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

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#include <ncore/NCLog.h>
#include <ncore/NCFile.h>
#include <cutils/properties.h>
#include <ncore/NCDir.h>
#include <ncore/NCAutoSync.h>
#include <ncore/NCError.h>
#include <ncore/NCStartPerformance.h>
#include <ncore/NCError.h>
#include <system/NCSystem.h>
#include <system/NCEnvironment.h>
#include <networkmanager/NCNetworkDef.h>
#include <connectmanager/NCConnect.h>
#include "NCNetworkManagerDef.h"
#include "NCNetworkHandler.h"
#include "NCNetworkDeviceType.h"
#include "NCNetworkStatus.h"
#include "NCNetworkPluginManager.h"
#include "NCNetworkManager.h"
#include "NCNetworkNotify.h"
#include "NCNetworkAccessStack.h"
#include "networkmanager/NCNetworkDataReplier.h"
#include "NCNetworkThread.h"
#include "NCNetworkAvcUnit.h"
#include "NCNetworkEventReq.h"
#include "NCNetworkReq.h"

#define NC_LOG_TAG "NCNetworkManager"

#define NC_NETWORK_VAR_DB_DIR "/var/db/"
#define NC_NETWORK_DHCLIENT_LEASES "/var/db/dhclient.leases"
#define NC_NETWORK_DHCPD_LEASES "/var/db/dhcpd.leases"
#define NCNETWORK_DCU_FIREWALL "/etc/iptables_dcu.sh"
#define NCNETWORK_MEU_FIREWALL "/etc/iptables_meu.sh"
#define NCNETWORK_PROPERTY_EXTUNIT_NEEDED "iauto.hal.extunit.needed"
#define NCNETWORK_PROPERTY_EXTUNIT_VENDOR "iauto.hal.extunit.vendor"
#define NCNETWORK_DHCPD_CONFIG_ORIGIN_FILE "/etc/dhcpd.conf.orgin"
#define NCNETWORK_DHCPD_CONFIG_FILE "/tmp/dhcpd.conf"

#define PACKET_SIZE 4096

namespace nutshell
{
    const UINT NCNETWORK_DB_FILE_PERMISSIONS = 0x0640; 
    
    NCNetworkReqBase::NCNetworkReqBase(const UINT32 reqType)
    : m_reqType(reqType)
    , m_reqState(REQ_INIT)
    , m_thread(NULL)
    {

    }

    NCNetworkReqBase::~NCNetworkReqBase()
    {

    }

    UINT32
    NCNetworkReqBase::getFuncType()
    {
        return m_reqType;
    }

    VOID
    NCNetworkReqBase::doTimeout()
    {
        NCLOGD("NCNetworkReqBase::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        
        m_reqState = REQ_TIMEOUT;

        if (m_thread) {
            m_thread->notifyRequest();
        }
    }

    UINT32
    NCNetworkReqBase::getReqState()
    {
        NCLOGD("NCNetworkReqBase::getReqState, reqState [%d]", m_reqState);
        return m_reqState;
    }

    VOID
    NCNetworkReqBase::setReqState(const UINT32 state)
    {
        NCLOGD("NCNetworkReqBase::setReqState, state [%d]", state);
        m_reqState = state;
    }

    VOID
    NCNetworkReqBase::lockReqState()
    {
        NCLOGD("NCNetworkReqBase::lockReqState, Start");
        m_syncState.syncStart();
    }

    VOID
    NCNetworkReqBase::unlockReqState()
    {
        NCLOGD("NCNetworkReqBase::unlockReqState, Start");
        m_syncState.syncEnd();
    }

    NCNetworkSetAdbDebugIpReq::NCNetworkSetAdbDebugIpReq()
    : NCNetworkReqBase(RequestType_SetAdbDebugIp)
    {

    }

    NCNetworkSetAdbDebugIpReq::~NCNetworkSetAdbDebugIpReq()
    {

    }

    VOID
    NCNetworkSetAdbDebugIpReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
#if defined NCNETWORK_TARGET_PRODUCT_EMULATOR
        NCLOGD("NCNetworkSetAdbDebugIpReq::doAction, Emulator");
        return;
#endif
        NCLOGD("NCNetworkSetAdbDebugIpReq::doAction - Start");
        m_thread = pThread;
        NCNetworkHandler handler;
        DeviceIpInfo info = NCNetworkManager::Instance()->getAdbDebugIpInfo();
        handler.setStaticIpMaskMtu(info.device, info.ip, info.prefixLength, 0);
        pThread->waitRequest(1000*5);
    }


    NCNetworkSetAvcLanIpReq::NCNetworkSetAvcLanIpReq()
    : NCNetworkReqBase(RequestType_SetAvcLanIp)
    {

    }

    NCNetworkSetAvcLanIpReq::~NCNetworkSetAvcLanIpReq()
    {

    }

    VOID
    NCNetworkSetAvcLanIpReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkSetAvcLanIpReq::doAction - Start");
        m_thread = pThread;

        if (NCNetworkManager::Instance()->getNetworkStatus()->status(USB_AVCLAN)) {
            NCLOGD("NCNetworkSetAvcLanIpReq::doAction, Ncm ip has been configured");
            return;
        }

        NCNetworkHandler handler;
        if (!handler.checkAvcLanStatus()) {
            NCLOGD("NCNetworkSetAvcLanIpReq::doAction - AvcLan status is failed");
            return;
        }

        NCString usbName;
        DeviceIpInfo info = NCNetworkManager::Instance()->getAvcLanIpInfo();
#if defined NCNETWORK_TARGET_PRODUCT_DCU
        usbName = handler.getAvcLanUsbName();
        if (NCString("") == usbName) {
            NCLOGD("NCNetworkSetAvcLanIpReq::doAction, Get usb name failed");
            NCNetworkManager::Instance()->startTimer(TimerType_AvcLanIp);
            return;
        }
        else {
            info.device = usbName;
            NCNetworkManager::Instance()->setAvcLanIpInfo(info);
        }
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
        usbName = info.device;
#endif        
        NCLOGD("NCNetworkSetAvcLanIpReq::doAction, Usb name [%s], ip [%s], prefixLength [%d]", usbName.getString(), info.ip.getString(), info.prefixLength);
        handler.setStaticIpMaskMtu(usbName, info.ip, info.prefixLength, 0);
        pThread->waitRequest(1000*5);
    }

    VOID
    NCNetworkSetAvcLanIpReq::doTimeout()
    {
        NCLOGD("NCNetworkSetAvcLanIpReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        m_reqState = REQ_TIMEOUT;
        
        NCNetworkManager::Instance()->startTimer(TimerType_AvcLanIp);

        if (m_thread) {
            m_thread->notifyRequest();
        }
    }

    NCNetworkAddRouteVRServer::NCNetworkAddRouteVRServer()
    : NCNetworkReqBase(RequestType_AddRouteVRServer)
    {

    }

    NCNetworkAddRouteVRServer::~NCNetworkAddRouteVRServer()
    {

    }

    VOID
    NCNetworkAddRouteVRServer::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkAddRouteVRServer::doAction, Start");
        if (NCNetworkManager::Instance()->getNetworkStatus()->status(VR_SERVER)) {
            NCLOGD("NCNetworkAddRouteVRServer::doAction, VRServer route has been configured");
            return;
        }

        m_thread = pThread;
        RouteInfo info = NCNetworkManager::Instance()->getRouteInfo();
        NCNetworkHandler handler;
        handler.addRoute(info);
        pThread->waitRequest(1000*10);
    }

    VOID
    NCNetworkAddRouteVRServer::doTimeout()
    {
        NCLOGD("NCNetworkAddRouteVRServer::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        m_reqState = REQ_TIMEOUT;

        NCNetworkHandler handler;
        NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(USB_AVCLAN));

        if (m_thread) {
            m_thread->notifyRequest();
        }
    }


    NCNetworkRequestAccessReq::NCNetworkRequestAccessReq(const AccessParameter& param)
    : NCNetworkReqBase(RequestType_Access)
    , m_param(param)
    {

    }

    NCNetworkRequestAccessReq::~NCNetworkRequestAccessReq()
    {

    }

    VOID
    NCNetworkRequestAccessReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkRequestAccessReq::doAction, Start");
        NCLOGD("reqId [%d], resourceId [%s], deviceType [%s], wifiApFlag [%d], remoteFlag [%d]", 
            m_param.reqId, m_param.resourceId.getString(), m_param.deviceType.getString(), m_param.wifiApFlag, m_param.remoteFlag);
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkPluginManager* myWorkPluginManager = NCNetworkManager::Instance()->getNetworkPluginManager();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        NCRunnableThread* myEventThread = NCNetworkManager::Instance()->getEventThreadPointer();
        if (NULL == myStatus || NULL == myWorkPluginManager || NULL == myDeviceType 
            || NULL == myEventThread) {
            NCLOGD("NCNetworkRequestAccessReq::doAction, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        m_thread = pThread;
        NCNetworkNotify notify;
        NCString currNetwork = myStatus->currentNetwork();
        if (WIFI_STA == m_param.deviceType) {
            if (WIFI_STA == currNetwork) {
                NCLOGD("NCNetworkRequestAccessReq::doAction, Wifi is available");
                AccessParameter tempParam = m_param;
                tempParam.result = NetworkErrCode_Success;
                myEventThread->looper().postRunnable(new NCNetworkReplyRequestMapReq(tempParam));
                return;
            }
            else {
                NC_BOOL wifiStatus = myStatus->status(WIFI_STA);
                if (!wifiStatus) {
                    NCLOGD("NCNetworkRequestAccessReq::doAction, Wifi is not available");
                    AccessParameter tempParam = m_param;
                    tempParam.result = NetworkErrCode_Fail;
                    myEventThread->looper().postRunnable(new NCNetworkReplyRequestMapReq(tempParam));
                    return;
                }

#if defined NCNETWORK_TARGET_PRODUCT_DCU
                NCString wifiName = "";
                myDeviceType->getName(WIFI_STA, wifiName);
                NCLOGD("NCNetworkRequestAccessReq::doAction, Get wifi name [%s]", wifiName.getString());

                myEventThread->looper().postRunnable(new NCNetworkAddRequestMapReq(RequestMapType_Callback, m_param));
                myStatus->setDialState(m_param.deviceType, DialState_Connecting, DialType_None);
                NCNetworkHandler handler;
                handler.networkDeviceInfo(wifiName);
                pThread->waitRequest(1000 * 10);
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
                NCLOGD("NCNetworkRequestAccessReq::doAction, In meu, wait wifi connected in dcu");
                myEventThread->looper().postRunnable(new NCNetworkAddRequestMapReq(RequestMapType_Callback, m_param));
                myStatus->setDialState(m_param.deviceType, DialState_Connecting, DialType_None);
                pThread->waitRequest(1000 * 10);
#endif
            }
        }
        else if (DCM == m_param.deviceType || BT_DUN == m_param.deviceType) {
            while (myWorkPluginManager->isReleasing(m_param.deviceType)) {
                NCLOGD("NCNetworkRequestAccessReq::doAction, isReleasing is true");
                sleep(1);
            }

            lockReqState();
            setReqState(REQ_DIALSTART);
            if (currNetwork != m_param.deviceType) {
                myStatus->setLinkState(m_param.deviceType, DialState_Connecting, m_param.dealType);
                myStatus->setDialState(m_param.deviceType, DialState_Connecting, m_param.dealType);
            }

            NCLOGD("NCNetworkRequestAccessReq::doAction, Request, reqid [%d]", m_param.reqId);
            myEventThread->looper().postRunnable(new NCNetworkAddRequestMapReq(RequestMapType_Init, m_param));
            myWorkPluginManager->requestNetworkAccess(m_param);

            lockObject();
            std::vector<AccessParameter>::iterator iter;
            for (iter = m_reqAccessList.begin(); iter != m_reqAccessList.end(); ++iter) {
                NCLOGD("NCNetworkRequestAccessReq::doAction, list is not empty");
                AccessParameter temParam = *iter;
                NCLOGD("reqId [%d], resourceId [%s], deviceType [%s], wifiApFlag [%d], remoteFlag [%d]", 
                    temParam.reqId, temParam.resourceId.getString(), temParam.deviceType.getString(), temParam.wifiApFlag, temParam.remoteFlag);
                if (RequestType_Access == temParam.reqType) {
                    NCLOGD("NCNetworkRequestAccessReq::doAction, Request");
                    myEventThread->looper().postRunnable(new NCNetworkAddRequestMapReq(RequestMapType_Init, temParam));
                    myWorkPluginManager->requestNetworkAccess(temParam);
                }
                else if (RequestType_Release == temParam.reqType) {
                    NCLOGD("NCNetworkRequestAccessReq::doAction, Update");
                    myEventThread->looper().postRunnable(new NCNetworkAddRequestMapReq(RequestMapType_Init, temParam));
                    myWorkPluginManager->updateNetworkAccess(temParam.deviceType, temParam.resourceId, temParam.reqId);
                }
                else if (RequestType_Disconnect == temParam.reqType) {
                    NCLOGD("NCNetworkRequestAccessReq::doAction, Disconnect");
                    myStatus->setLinkState(temParam.deviceType, DialState_Disconnecting, DialType_None);
                    myEventThread->looper().postRunnable(new NCNetworkAddRequestMapReq(RequestMapType_Init, temParam));
                    myWorkPluginManager->releaseNetworkAccess(temParam.deviceType, temParam.reqId);
                }
            }

            unlockObject();
            unlockReqState(); 
            pThread->waitRequest(1000 * 600);
        }
    }

    VOID
    NCNetworkRequestAccessReq::doTimeout()
    {
        NCLOGD("NCNetworkRequestAccessReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            NCLOGD("NCNetworkRequestAccessReq::doTimeout, Confirmed, return");
            return;
        }
        
        m_reqState = REQ_TIMEOUT;

        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCRunnableThread* myEventThread = NCNetworkManager::Instance()->getEventThreadPointer();
        if (NULL == myStatus || NULL == myEventThread) {
            NCLOGD("NCNetworkRequestAccessReq::doAction, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (WIFI_STA == m_param.deviceType) {
            myStatus->setDialState(m_param.deviceType, DialState_Init, DialType_None);
        }
        else {
            NCString currNetwork = myStatus->currentNetwork();
            if (currNetwork != m_param.deviceType) {
                myStatus->setLinkState(m_param.deviceType, DialState_Init, DialType_None);
                myStatus->setDialState(m_param.deviceType, DialState_Init, DialType_None);
            }
        }

        NCNetworkHandler handler;
        NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(m_param.deviceType));
        myEventThread->looper().postRunnable(new NCNetworkPopAllRequestMapReq(RequestMapType_Callback, PopResultFlag_Timeout));
        myEventThread->looper().postRunnable(new NCNetworkPopAllRequestMapReq(RequestMapType_Init, PopResultFlag_Timeout));
        m_thread->notifyRequest();
    }

    NCString
    NCNetworkRequestAccessReq::getDeviceType()
    {
        return m_param.deviceType;
    }

    VOID
    NCNetworkRequestAccessReq::addRequestList(const AccessParameter& param)
    {
        NCLOGD("NCNetworkRequestAccessReq::addRequestList, Start");
        m_reqAccessList.push_back(param);
    }

    VOID
    NCNetworkRequestAccessReq::lockObject()
    {
        NCLOGD("NCNetworkRequestAccessReq::lockObject, Start");
        m_syncList.syncStart();
    }

    VOID
    NCNetworkRequestAccessReq::unlockObject()
    {
        NCLOGD("NCNetworkRequestAccessReq::lockObject, End");
        m_syncList.syncEnd();
    }


    NCNetworkReleaseReq::NCNetworkReleaseReq(const AccessParameter& param)
    : NCNetworkReqBase(RequestType_Release)
    , m_param(param)
    {

    }

    NCNetworkReleaseReq::~NCNetworkReleaseReq()
    {

    }

    VOID
    NCNetworkReleaseReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkReleaseReq::doAction, Start, resourceid [%s], deviceType [%s]", m_param.resourceId.getString(), 
            m_param.deviceType.getString());
        m_thread = pThread;
        NCNetworkPluginManager* myWorkPluginManager = NCNetworkManager::Instance()->getNetworkPluginManager();
        NCRunnableThread* myEventThread = NCNetworkManager::Instance()->getEventThreadPointer();
        if (NULL == myWorkPluginManager || NULL == myEventThread) {
            NCLOGD("NCNetworkReleaseReq::doAction, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            AccessParameter tempParam = m_param;
            tempParam.result = NetworkErrCode_Fail;
            myEventThread->looper().postRunnable(new NCNetworkReplyRequestMapReq(tempParam));
            return;
        }
        
        if (WIFI_STA == m_param.deviceType) {
            AccessParameter tempParam = m_param;
            tempParam.result = NetworkErrCode_Success;
            myEventThread->looper().postRunnable(new NCNetworkReplyRequestMapReq(tempParam));
        }
        else if (DCM == m_param.deviceType || BT_DUN == m_param.deviceType) {
            myEventThread->looper().postRunnable(new NCNetworkAddRequestMapReq(RequestMapType_Init, m_param));
            myWorkPluginManager->updateNetworkAccess(m_param.deviceType, m_param.resourceId, m_param.reqId);
            m_thread->waitRequest(1000 * 600);
        }
    }

    VOID
    NCNetworkReleaseReq::doTimeout()
    {
        NCLOGD("NCNetworkReleaseReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            NCLOGD("NCNetworkReleaseReq::doTimeout, Confirmed, return");
            return;
        }
        
        m_reqState = REQ_TIMEOUT;

        NCRunnableThread* myEventThread = NCNetworkManager::Instance()->getEventThreadPointer();
        if (NULL == myEventThread) {
            NCLOGD("NCNetworkReleaseReq::doTimeout, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        NCNetworkHandler handler;
        NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(m_param.deviceType));
        myEventThread->looper().postRunnable(new NCNetworkPopAllRequestMapReq(RequestMapType_Init, PopResultFlag_Timeout));
        m_thread->notifyRequest();
    }

    NCNetworkPluginReleaseAccessReq::NCNetworkPluginReleaseAccessReq(const AccessParameter& param)
    : NCNetworkReqBase(RequestType_PluginReleaseAccess)
    , m_param(param)
    {
        
    }

    NCNetworkPluginReleaseAccessReq::~NCNetworkPluginReleaseAccessReq()
    {

    }

    VOID
    NCNetworkPluginReleaseAccessReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkPluginReleaseAccessReq::doAction - Start, deviceType[%s]", m_param.deviceType.getString());
        m_thread = pThread;
        NCNetworkPluginManager* myWorkPluginManager = NCNetworkManager::Instance()->getNetworkPluginManager();
        NCRunnableThread* myEventThread = NCNetworkManager::Instance()->getEventThreadPointer();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        if (NULL == myWorkPluginManager || NULL == myEventThread || NULL == myStatus) {
            NCLOGD("NCNetworkPluginReleaseAccessReq::doAction, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (WIFI_STA == m_param.deviceType) {
            myEventThread->looper().postRunnable(new NCNetworkAddRequestMapReq(RequestMapType_Callback, m_param));
        }
        else if (DCM == m_param.deviceType || BT_DUN == m_param.deviceType) {
            myStatus->setLinkState(m_param.deviceType, DialState_Disconnecting, DialType_None);
            myEventThread->looper().postRunnable(new NCNetworkAddRequestMapReq(RequestMapType_Init, m_param));
            myWorkPluginManager->releaseNetworkAccess(m_param.deviceType, m_param.reqId);
            m_thread->waitRequest(1000 * 600);
        }
    }

    VOID
    NCNetworkPluginReleaseAccessReq::doTimeout()
    {
        NCLOGD("NCNetworkPluginReleaseAccessReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            NCLOGD("NCNetworkPluginReleaseAccessReq::doTimeout, Confirmed, return");
            return;
        }
        
        m_reqState = REQ_TIMEOUT;

        NCRunnableThread* myEventThread = NCNetworkManager::Instance()->getEventThreadPointer();
        if (NULL == myEventThread) {
            NCLOGD("NCNetworkPluginReleaseAccessReq::doTimeout, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        NCNetworkHandler handler;
        NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(m_param.deviceType));
        myEventThread->looper().postRunnable(new NCNetworkPopAllRequestMapReq(RequestMapType_Init, PopResultFlag_Timeout));
        m_thread->notifyRequest();
    }

    NCNetworkSetSlipDeviceReq::NCNetworkSetSlipDeviceReq(const NCNetworkAccess::SlipDeviceInfo& info)
    : NCNetworkReqBase(RequestType_SetSlipDevice)
    , m_deviceType(info.deviceType)
    , m_deviceName(info.deviceName)
    , m_ip(info.ip)
    , m_mask(info.mask)
    , m_mtu(info.mtu)
    {

    }

    NCNetworkSetSlipDeviceReq::~NCNetworkSetSlipDeviceReq()
    {

    }

    VOID
    NCNetworkSetSlipDeviceReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkSetSlipDeviceReq::doAction - Start, deviceType [%s], deviceName [%s] ip [%s], mask [%d], mtu [%d]", 
            m_deviceType.getString(), m_deviceName.getString(), m_ip.getString(), m_mask, m_mtu);
        m_thread = pThread;
        NCNetworkManager::Instance()->getNetworkDeviceType()->add(m_deviceName, m_deviceType);
        NCNetworkHandler handler;
        handler.setStaticIpMaskMtu(m_deviceName, m_ip, m_mask, m_mtu);
        pThread->waitRequest(1000*10);
    }


    NCNetworkRequestIpReq::NCNetworkRequestIpReq(const NCString& resourceId, const NCString& deviceType, const NCString& deviceName)
    : NCNetworkReqBase(RequestType_RequestIp)
    , m_resourceId(resourceId)
    , m_deviceType(deviceType)
    , m_deviceName(deviceName)
    {

    }

    NCNetworkRequestIpReq::~NCNetworkRequestIpReq()
    {

    }

    VOID
    NCNetworkRequestIpReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkRequestIpReq::doAction - Start, deviceName [%s], deviceType [%s]", m_deviceName.getString(), m_deviceType.getString());
        m_thread = pThread;
        NCNetworkNotify notify;
        if (WIFI_STA == m_deviceType) {
            NCString currNetwork = NCNetworkManager::Instance()->getNetworkStatus()->currentNetwork();
            if (NCString("") != currNetwork) {
                NCLOGD("NCNetworkRequestIpReq::doAction, Current network is not empty");
                notify.notifyRequestIpResult(m_deviceType, NetworkErrCode_Fail);
                return;
            }

            NCConnect connectProxy;
            if (!connectProxy.verifyPermission(m_resourceId)) {
                NCLOGD("NCNetworkRequestIpReq::doAction, Verify permission failed");
                notify.notifyRequestIpResult(m_deviceType, NetworkErrCode_Fail);
                return;
            }
            else {
                // Notify wifi start connecting
                NCNetworkManager::Instance()->getNetworkStatus()->setDialState(m_deviceType, DialState_Connecting, DialType_None);
            }
        }

        property_set(NCNETWORK_DHCLIENT_CANCEL_FLAG, "off");
        NCNetworkManager::Instance()->getNetworkDeviceType()->add(m_deviceName, m_deviceType);

        NCNetworkHandler handler;
        handler.requestIp(m_deviceName);
        pThread->waitRequest(1000*50);
    }

    VOID
    NCNetworkRequestIpReq::doTimeout()
    {
        NCLOGD("NCNetworkRequestIpReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        m_reqState = REQ_TIMEOUT;

        NCNetworkNotify notify;
        if (WIFI_STA == m_deviceType) {
            // Notify wifi start failed
            NCNetworkManager::Instance()->getNetworkStatus()->setDialState(m_deviceType, DialState_Init, DialType_None);
        }

        notify.notifyRequestIpResult(m_deviceType, NetworkErrCode_Timeout);

        NCNetworkHandler handler;
        NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(m_deviceType));
        
        if (m_thread) {
            m_thread->notifyRequest();
        }
    }

    NCString
    NCNetworkRequestIpReq::getDeviceType()
    {
        return m_deviceType;
    }

    NCString
    NCNetworkRequestIpReq::getDeviceName()
    {
        return m_deviceName;
    }

    NCNetworkReleaseIpReq::NCNetworkReleaseIpReq(const NCString& deviceType, const NCString& deviceName)
    : NCNetworkReqBase(RequestType_ReleaseIp)
    , m_deviceType(deviceType)
    , m_deviceName(deviceName)
    {

    }

    NCNetworkReleaseIpReq::~NCNetworkReleaseIpReq()
    {

    }

    VOID
    NCNetworkReleaseIpReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkReleaseIpReq::doAction - Start, deviceName [%s], deviceType [%s]", m_deviceName.getString(), m_deviceType.getString());
        m_thread = pThread;
        NCNetworkManager::Instance()->getNetworkDeviceType()->add(m_deviceName, m_deviceType);

        NCString currNetwork = NCNetworkManager::Instance()->getNetworkStatus()->currentNetwork();
        if (WIFI_STA == m_deviceType && WIFI_STA == currNetwork) {
            NCNetworkNotify notify;
            NCNetworkManager::Instance()->getNetworkStatus()->setDialState(m_deviceType, DialState_Disconnecting, DialType_None);
        }

        NCNetworkHandler handler;
        handler.releaseIp(m_deviceName);
        pThread->waitRequest(1000*20);
    }

    VOID
    NCNetworkReleaseIpReq::doTimeout()
    {
        NCLOGD("NCNetworkReleaseIpReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        m_reqState = REQ_TIMEOUT;
        
        NCNetworkNotify notify;
        NCString currNetwork = NCNetworkManager::Instance()->getNetworkStatus()->currentNetwork();
        if (WIFI_STA == m_deviceType && WIFI_STA == currNetwork) {
            NCNetworkManager::Instance()->getNetworkStatus()->setDialState(m_deviceType, DialState_Init, DialType_None);
        }

        notify.notifyReleaseIpResult(m_deviceName, NetworkErrCode_Timeout);

        NCNetworkHandler handler;
        NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(m_deviceType));
        
        if (m_thread) {
            m_thread->notifyRequest();
        }
    }

    NCString
    NCNetworkReleaseIpReq::getDeviceType()
    {
        return m_deviceType;
    }

    NCString
    NCNetworkReleaseIpReq::getDeviceName()
    {
        return m_deviceName;
    }


    NCNetworkStartDhcpServerReq::NCNetworkStartDhcpServerReq(const NCString& deviceType, const NCString& deviceName)
    : NCNetworkReqBase(RequestType_StartDhcpServer)
    , m_deviceType(deviceType)
    , m_deviceName(deviceName)
    {

    }

    NCNetworkStartDhcpServerReq::~NCNetworkStartDhcpServerReq()
    {

    }

    VOID
    NCNetworkStartDhcpServerReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkStartDhcpServerReq::doAction - Start, deviceName [%s], deviceType [%s]", m_deviceName.getString(), m_deviceType.getString());
        m_thread = pThread;
        NCNetworkManager::Instance()->getNetworkDeviceType()->add(m_deviceName, m_deviceType);

        NCNetworkHandler handler;
        NCString ip;
        UINT32 prefixLength;
        if (handler.readWifiApStaticIp(ip, prefixLength)) {
            handler.startDhcpServer(m_deviceName, ip, prefixLength);
            pThread->waitRequest(1000*20);
        }
        else {
            NCLOGD("NCNetworkStartDhcpServerReq::doAction, read wifiAp IP failed");
            NCErrorLog(NetworkErrCode_StartDhcpServerFailed, handler.deviceTypeToNumber(m_deviceType));
            NCNetworkNotify notify;
            notify.notifyStartDhcpServerResult(m_deviceType, NetworkErrCode_Fail);
        }
    }

    VOID
    NCNetworkStartDhcpServerReq::doTimeout()
    {
        NCLOGD("NCNetworkStartDhcpServerReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        m_reqState = REQ_TIMEOUT;
        
        NCNetworkNotify notify;
        notify.notifyStartDhcpServerResult(m_deviceType, NetworkErrCode_Timeout);

        NCNetworkHandler handler;
        NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(m_deviceType));
        
        if (m_thread) {
            m_thread->notifyRequest();
        }
    }


    NCNetworkStopDhcpServerReq::NCNetworkStopDhcpServerReq(const NCString& deviceType, const NCString& deviceName)
    : NCNetworkReqBase(RequestType_StopDhcpServer)
    , m_deviceType(deviceType)
    , m_deviceName(deviceName)
    {

    }

    NCNetworkStopDhcpServerReq::~NCNetworkStopDhcpServerReq()
    {

    }

    VOID
    NCNetworkStopDhcpServerReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkStopDhcpServerReq::doAction - Start, deviceName [%s], deviceType [%s]", m_deviceName.getString(), m_deviceType.getString());
        m_thread = pThread;

        NC_BOOL status = NCNetworkManager::Instance()->getNetworkStatus()->status(m_deviceType);
        if (!status) {
            NCLOGD("NCNetworkStopDhcpServerReq::doAction, Dhcp server is not running");
            NCNetworkNotify notify;
            notify.notifyStopDhcpServerResult(m_deviceType, NetworkErrCode_Success);
            return;
        }

        NCNetworkManager::Instance()->getNetworkDeviceType()->add(m_deviceName, m_deviceType);

        NCNetworkHandler handler;
        handler.stopDhcpServer(m_deviceName);
        pThread->waitRequest(1000*20);
    }

    VOID
    NCNetworkStopDhcpServerReq::doTimeout()
    {
        NCLOGD("NCNetworkStopDhcpServerReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        m_reqState = REQ_TIMEOUT;
        
        NCNetworkNotify notify;
        notify.notifyStopDhcpServerResult(m_deviceType, NetworkErrCode_Timeout);

        NCNetworkHandler handler;
        NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(m_deviceType));
        
        if (m_thread) {
            m_thread->notifyRequest();
        }
    }


    NCNetworkDeviceInfoReq::NCNetworkDeviceInfoReq(const NCString& deviceType)
    : NCNetworkReqBase(RequestType_DeviceInfo)
    , m_deviceType(deviceType)
    {

    }

    NCNetworkDeviceInfoReq::~NCNetworkDeviceInfoReq()
    {

    }

    VOID
    NCNetworkDeviceInfoReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkDeviceInfoReq::doAction - Start, deviceType [%s]", m_deviceType.getString());
        m_thread = pThread;
        NCString deviceName;
        NC_BOOL find = NCNetworkManager::Instance()->getNetworkDeviceType()->getName(m_deviceType, deviceName);
        if (find) {
            NCNetworkHandler handler;
            handler.networkDeviceInfo(deviceName);
            pThread->waitRequest(1000*10);
        }
    }

    VOID
    NCNetworkDeviceInfoReq::doTimeout()
    {
        NCLOGD("NCNetworkDeviceInfoReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        m_reqState = REQ_TIMEOUT;
        
        NCNetworkNotify notify;
        NCNetworkDataReplier::NetworkDeviceInfo info;
        notify.notifyDeviceInfo(m_deviceType, info);

        NCNetworkHandler handler;
        NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(m_deviceType));
        
        if (m_thread) {
            m_thread->notifyRequest();
        }
    }


    NCNetworkConnectedClientInfo::NCNetworkConnectedClientInfo(const NCString& mac)
    : m_mac(mac)
    {

    }

    NCNetworkConnectedClientInfo::~NCNetworkConnectedClientInfo()
    {

    }

    VOID
    NCNetworkConnectedClientInfo::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkConnectedClientInfo::doAction - Start");
        NCNetworkHandler handler;
        handler.connectedClientInfo(m_mac);
    }


    NCNetworkInitResolvConfReq::NCNetworkInitResolvConfReq()
    {

    }

    NCNetworkInitResolvConfReq::~NCNetworkInitResolvConfReq()
    {

    }

    VOID
    NCNetworkInitResolvConfReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkInitResolvConfReq::doAction, Start");
        NCString etcOrginFile =  NCNETWORK_RESOLV_CONF_ORGIN;
        NCString varFile =  NCNETWORK_RESOLV_CONF;
        if (NCFile::exists(varFile)) {
            NCLOGD("NCNetworkInitResolvConfReq::doAction - /tmp/resolv.conf is exist");
            NCFile::remove(varFile);
        }

        NCFile::copy(etcOrginFile, varFile);
    }

    NCNetworkInitDhcpLeaseFileReq::NCNetworkInitDhcpLeaseFileReq()
    {

    }

    NCNetworkInitDhcpLeaseFileReq::~NCNetworkInitDhcpLeaseFileReq()
    {

    }

    VOID
    NCNetworkInitDhcpLeaseFileReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkInitDhcpLeaseFileReq::doAction, Start");
        NCDir cDir;
        if (!cDir.exists(NC_NETWORK_VAR_DB_DIR)) {
            cDir.makePath(NC_NETWORK_VAR_DB_DIR);
        }

        NCFile dhclientFile(NC_NETWORK_DHCLIENT_LEASES);
        if (!dhclientFile.exists()) {
            NCLOGD("NCNetworkInitDhcpLeaseFileReq::doAction, dhclient.leases is not exist");
            if (dhclientFile.open(NC_FOM_Append)) {
                NCLOGD("NCNetworkInitDhcpLeaseFileReq::doAction, creat dhclient.leases success");
                dhclientFile.setPermissions(NCNETWORK_DB_FILE_PERMISSIONS);
                dhclientFile.close();
            }
        }
    }

    NCNetworkInitDhcpPidFileReq::NCNetworkInitDhcpPidFileReq()
    {

    }

    NCNetworkInitDhcpPidFileReq::~NCNetworkInitDhcpPidFileReq()
    {

    }

    VOID
    NCNetworkInitDhcpPidFileReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkInitDhcpPidFileReq::doAction - Start");
        NCString dhclientPidFile = DHCPCLIENT_PID_FILE;
        NCString dhcpdPidFile = DHCPD_PID_FILE;

        NCFile dhclientObj(dhclientPidFile);
        if (dhclientObj.exists()) {
            dhclientObj.remove();
        }

        NCFile dhcpdObj(dhcpdPidFile);
        if (dhcpdObj.exists()) {
            dhcpdObj.remove();
        }
    }

    NCNetworkInitDhcpdConfReq::NCNetworkInitDhcpdConfReq()
    {

    }

    NCNetworkInitDhcpdConfReq::~NCNetworkInitDhcpdConfReq()
    {

    }

    VOID
    NCNetworkInitDhcpdConfReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkInitDhcpdConfReq::doAction, Start");
        NCString originFile = NCNETWORK_DHCPD_CONFIG_ORIGIN_FILE;
        NCString varFile = NCNETWORK_DHCPD_CONFIG_FILE;
        if (NCFile::exists(varFile)) {
            NCLOGD("NCNetworkInitDhcpdConfReq::doAction, /var/dhcpd.conf is exist, remove it");
            NCFile::remove(varFile);
        }

        NCFile::copy(originFile, varFile);
    }

    NCNetworkUpdateDhcpdConfReq::NCNetworkUpdateDhcpdConfReq(const NCString& dns1, const NCString& dns2)
    : NCNetworkReqBase(RequestType_RestartDhcpServer)
    , m_dns1(dns1)
    , m_dns2(dns2)
    {

    }

    NCNetworkUpdateDhcpdConfReq::~NCNetworkUpdateDhcpdConfReq()
    {

    }

    VOID
    NCNetworkUpdateDhcpdConfReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, Start, m_dns1 [%s], m_dns2 [%s]", m_dns1.getString(), m_dns2.getString());
        m_thread = pThread;
        NCString configFile = NCNETWORK_DHCPD_CONFIG_FILE;
        NCString emptyDns = "0.0.0.0";
        if (NCString("") == m_dns1) {
            m_dns1 = emptyDns;
        }

        if (NCString("") == m_dns2) {
            m_dns2 = emptyDns;
        }

        NCCHAR tempFile[64] = { 0 };
        snprintf(tempFile, sizeof(tempFile)-1, "%s~", configFile.getString());
        if (NCFile::exists(tempFile)) {
            NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, /tmp/dhcpd.conf~ is exist, remove it");
            NCFile::remove(tempFile);
        }

        FILE* oldPid = fopen(configFile.getString(), "r");
        if (NULL == oldPid) {
            NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction - Open %s failed", configFile.getString());
            return;
        }

        FILE* newPid = fopen(tempFile, "at");
        if (NULL == newPid) {
            NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction - Open %s failed", tempFile);
            fclose(oldPid);
            return;
        }

        NCCHAR options[] = "option domain-name-servers";
        NCCHAR buf[128] = { 0 };
        NCNetworkHandler handler;
        NC_BOOL restartFlag;

        while (NULL != fgets(buf, sizeof(buf)-1, oldPid)) {
            if (strncmp(options, buf, strlen(options))) {
                fprintf(newPid, "%s", buf);
            }
            else {
                NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, find options success");
                NCCHAR replace[128] = { 0 };
                if (emptyDns != m_dns1 && emptyDns == m_dns2) {
                    snprintf(replace, sizeof(replace)-1, "%s %s;", options, m_dns1.getString());
                }
                else if (emptyDns == m_dns1 && emptyDns != m_dns2) {
                    snprintf(replace, sizeof(replace)-1, "%s %s;", options, m_dns2.getString());
                }
                else if (emptyDns != m_dns1 && emptyDns != m_dns2) {
                    snprintf(replace, sizeof(replace)-1, "%s %s, %s;", options, m_dns1.getString(), m_dns2.getString());
                }
                else {
                    NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, dns1 and dns2 are NULL");
                    NCString ip;
                    UINT32 prefixLength;
                    if (handler.readWifiApStaticIp(ip, prefixLength)) {
                        NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, Read default dns success, dns [%s]", ip.getString());
                        snprintf(replace, sizeof(replace)-1, "%s %s;", options, ip.getString());
                    }
                    else {
                        NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, Read default dns failed");
                        snprintf(replace, sizeof(replace)-1, "%s", buf);
                        replace[strlen(replace) - 1] = 0;
                    }
                }

                fprintf(newPid, "%s\n", replace);

                NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, buf [%s]", buf);
                NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, replace [%s]", replace);
                if (strncmp(buf, replace, strlen(buf)-1)) {
                    NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, Start restart dhcpd");
                    restartFlag = NC_TRUE;
                }
                else {
                    restartFlag = NC_FALSE;
                }

            }
        }

        fclose(oldPid);
        fclose(newPid);

        NCFile::remove(configFile);
        NCFile::move(tempFile, configFile);

        NC_BOOL wifiApStatus = NCNetworkManager::Instance()->getNetworkStatus()->status(WIFI_AP);
        NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, restart flag [%d], wifi ap status [%d]", restartFlag, wifiApStatus);
        if (restartFlag && wifiApStatus) {
            NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, Restart dhcp server");
            NCString deviceName;
            NCNetworkManager::Instance()->getNetworkDeviceType()->getName(WIFI_AP, deviceName);
            NCLOGD("NCNetworkUpdateDhcpdConfReq::doAction, Get device name [%s]", deviceName.getString());
            handler.restartDhcpServer(deviceName);
            m_thread->waitRequest(1000*10);
        }
    }

    VOID
    NCNetworkUpdateDhcpdConfReq::doTimeout()
    {
        NCLOGD("NCNetworkUpdateDhcpdConfReq::doTimeout, Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        m_reqState = REQ_TIMEOUT;
        
        NCNetworkHandler handler;
        NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(WIFI_AP));
        
        if (m_thread) {
            m_thread->notifyRequest();
        }
    }

    NCNetworkInitNamedConfReq::NCNetworkInitNamedConfReq()
    {

    }

    NCNetworkInitNamedConfReq::~NCNetworkInitNamedConfReq()
    {

    }

    VOID
    NCNetworkInitNamedConfReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkInitNamedConfReq::doAction - Start");
        NCString etcOrginFile =  "/etc/named.conf.orgin";
        NCString varFile =  "/var/named.conf";

        NCFile varFileObj(varFile);
        if (varFileObj.exists()) {
            varFileObj.remove();
        }

        NCFile::copy(etcOrginFile, varFile);
    }

    NCNetworkSetGbookServerDnsReq::NCNetworkSetGbookServerDnsReq(const NCString& dcmDns1, const NCString& dcmDns2, const NCString& wifiDns1, const NCString& wifiDns2)
    : m_dcmDns1(dcmDns1)
    , m_dcmDns2(dcmDns2)
    , m_wifiDns1(wifiDns1)
    , m_wifiDns2(wifiDns2)
    {

    }

    NCNetworkSetGbookServerDnsReq::~NCNetworkSetGbookServerDnsReq()
    {

    }

    VOID
    NCNetworkSetGbookServerDnsReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
#if defined NCNETWORK_TARGET_PRODUCT_DCU
        NCLOGD("NCNetworkSetGbookServerDnsReq::doAction, Dcu do not need gbook server dns");
        return;
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
        NCLOGD("NCNetworkSetGbookServerDnsReq::doAction, Start, dcmDns1 [%s], dcmDns2 [%s], wifiDns1 [%s], wifiDns2 [%s]", m_dcmDns1.getString(), m_dcmDns2.getString(), 
            m_wifiDns1.getString(), m_wifiDns2.getString());
        NCNetworkManager::Instance()->getNetworkStatus()->setGbookServerDns(m_dcmDns1, m_dcmDns2, m_wifiDns1, m_wifiDns2);

        NCString currNetwork = NCNetworkManager::Instance()->getNetworkStatus()->currentNetwork();
        NCNetworkHandler handler;
        if (NCString(DCM) == currNetwork) {
            NCLOGD("NCNetworkSetGbookServerDnsReq::doAction, Update gbook dns for dcm");
            handler.updateNamedConf(NCNETWORK_GBOOK_DNS_ZONE, m_dcmDns1, m_dcmDns2);
        }
        else if (NCString(BT_DUN) == currNetwork || NCString(WIFI_STA) == currNetwork) {
            NCLOGD("NCNetworkSetGbookServerDnsReq::doAction, Update gbook dns for wifi or dun");
            handler.updateNamedConf(NCNETWORK_GBOOK_DNS_ZONE, m_wifiDns1, m_wifiDns2);
        }
        else {
            NCLOGD("NCNetworkSetGbookServerDnsReq::doAction, Only store gbook dns");
        }
#endif

    }

    NCNetworkStartDnsCacheServerReq::NCNetworkStartDnsCacheServerReq()
    {

    }

    NCNetworkStartDnsCacheServerReq::~NCNetworkStartDnsCacheServerReq()
    {

    }

    VOID
    NCNetworkStartDnsCacheServerReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkStartDnsCacheServerReq::doAction - Start");
        NCNetworkHandler handler;
        handler.startDnsCacheServer();
    }

    // Start firewall
    NCNetworkStartFirewallReq::NCNetworkStartFirewallReq(const UINT32 type)
    : m_type(type)
    {

    }

    NCNetworkStartFirewallReq::~NCNetworkStartFirewallReq()
    {

    }

    VOID
    NCNetworkStartFirewallReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkStartFirewallReq::doAction, Start, type [%d]", m_type);
#if defined NCNETWORK_TARGET_PRODUCT_EMULATOR
        NCLOGD("NCNetworkStartFirewallReq::doAction, Emulator");
        return;
#endif

        NCString script;
#if defined NCNETWORK_TARGET_PRODUCT_DCU
        script = NCNETWORK_DCU_FIREWALL;
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
        script = NCNETWORK_MEU_FIREWALL;
#endif

        NC_BOOL defaultFlag = NCNetworkManager::Instance()->getFirewallFlag(FirewallType_Default);
        NC_BOOL ncmFlag = NCNetworkManager::Instance()->getFirewallFlag(FirewallType_Ncm);

        NC_BOOL defaultFailedFlag = NCNetworkManager::Instance()->getFirewallFailedFlag(FirewallType_Default);
        NC_BOOL ncmFailedFlag = NCNetworkManager::Instance()->getFirewallFailedFlag(FirewallType_Ncm);


        NCNetworkHandler handler;
        NCString usbName;
        NCCHAR configCmd[128] = { 0 };
        NC_BOOL configDefault = NC_FALSE;
        NC_BOOL configNcm = NC_FALSE;

        NC_BOOL logNcmFlag = NC_FALSE;
        NC_BOOL logNcmIndividualFlag = NC_FALSE;

        if (FirewallType_Default == m_type) {
            if (defaultFlag) {
                NCLOGD("NCNetworkStartFirewallReq::doAction, Default rules is already configured");
                return;
            }

            if (defaultFailedFlag) {
                NCLOGD("NCNetworkStartFirewallReq::doAction, Start default firewall already failed");
                return;
            }

            snprintf(configCmd, sizeof(configCmd)-1, ". %s %d", script.getString(), FirewallType_Default);
            configDefault = NC_TRUE;
        }
        else if (FirewallType_Ncm == m_type) {
            if (ncmFlag) {
                NCLOGD("NCNetworkStartFirewallReq::doAction, Ncm rules is already configured");
                return;
            }

            if (ncmFailedFlag) {
                NCLOGD("NCNetworkStartFirewallReq::doAction, Start ncm firewall already failed");
                return;
            }

            if (defaultFailedFlag) {
                NCLOGD("NCNetworkStartFirewallReq::doAction, Start default firewall already failed");
                if (handler.checkIndividualApparatus()) {
                    return;
                }
                else {
                    NCLOGD("NCNetworkStartFirewallReq::doAction, Start init firewall");
#if defined NCNETWORK_TARGET_PRODUCT_DCU
                    usbName = handler.getAvcLanUsbName();
                    if (NCString("") == usbName) {
                        NCLOGD("NCNetworkStartFirewallReq::doAction, Get usb name failed");
                        return;
                    }
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
                    DeviceIpInfo info = NCNetworkManager::Instance()->getAvcLanIpInfo();
                    usbName = info.device;
                    if (NCString("") == usbName) {
                        NCLOGD("NCNetworkStartFirewallReq::doAction, Get usb name failed");
                        return;
                    }
#endif

                    // Initialize firewall rules
                    NCCHAR initCmd[128] = { 0 };
                    snprintf(initCmd, sizeof(initCmd)-1, ". %s %d %s", script.getString(), FirewallType_InitFirewall, usbName.getString());
                    INT initRet = system(initCmd);
                    if (handler.checkSystemReturn(initRet)) {
                        NCLOGD("NCNetworkStartFirewallReq::doAction, Init firewall success");
                    }
                    else {
                        NCLOGD("NCNetworkStartFirewallReq::doAction, Init firewall failed");
                    }

                    NCNetworkManager::Instance()->setFirewallFailedFlag(NC_TRUE, NC_TRUE);
                }

                return;
            }

            NCLOGD("NCNetworkStartFirewallReq::doAction, Start config rules");
#if defined NCNETWORK_TARGET_PRODUCT_DCU
            NCCHAR value[PROPERTY_VALUE_MAX] = { 0 };
            property_get(NCNETWORK_PROPERTY_EXTUNIT_NEEDED, value, NULL);
            if (NCString(value) == NCString("yes")) {
                NCLOGD("NCNetworkStartFirewallReq::doAction, DCU need MEU");
                if (handler.checkAvcLanStatus()) {
                    usbName = handler.getAvcLanUsbName();
                    if (NCString("") == usbName) {
                        NCLOGD("NCNetworkStartFirewallReq::doAction, Get usb name failed");
                        return;
                    }

                    if (defaultFlag) {
                        snprintf(configCmd, sizeof(configCmd)-1, ". %s %d %s", script.getString(), FirewallType_DcuMeu, usbName.getString());
                    }
                    else {
                        snprintf(configCmd, sizeof(configCmd)-1, ". %s %d %s", script.getString(), FirewallType_DefaultDcuMeu, usbName.getString());
                    }

                    logNcmFlag = NC_TRUE;
                }
                else {
                    if (!handler.checkIndividualApparatus()) {
                        return;
                    }

                    NCLOGD("NCNetworkStartFirewallReq::doAction, Dcu only mode");
                    if (defaultFlag) {
                        snprintf(configCmd, sizeof(configCmd)-1, ". %s %d", script.getString(), FirewallType_DcuOnly);
                        logNcmFlag = NC_FALSE;
                    }
                    else {
                        snprintf(configCmd, sizeof(configCmd)-1, ". %s %d", script.getString(), FirewallType_DefaultDcuOnly);
                        logNcmFlag = NC_TRUE;
                        logNcmIndividualFlag = NC_TRUE;
                    }
                }
            }
            else {
                NCLOGD("NCNetworkStartFirewallReq::doAction, Dcu only mode");
                if (defaultFlag) {
                    snprintf(configCmd, sizeof(configCmd)-1, ". %s %d", script.getString(), FirewallType_DcuOnly);
                    logNcmFlag = NC_FALSE;
                }
                else {
                    snprintf(configCmd, sizeof(configCmd)-1, ". %s %d", script.getString(), FirewallType_DefaultDcuOnly);
                    logNcmFlag = NC_TRUE;
                    logNcmIndividualFlag = NC_TRUE;
                }
            }
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
            DeviceIpInfo info = NCNetworkManager::Instance()->getAvcLanIpInfo();
            usbName = info.device;
            if (NCString("") == usbName) {
                NCLOGD("NCNetworkStartFirewallReq::doAction, Get usb name failed");
                return;
            }

            if (defaultFlag) {
                snprintf(configCmd, sizeof(configCmd)-1, ". %s %d %s", script.getString(), FirewallType_DcuMeu, usbName.getString());
            }
            else {
                snprintf(configCmd, sizeof(configCmd)-1, ". %s %d %s", script.getString(), FirewallType_DefaultDcuMeu, usbName.getString());
            }

            logNcmFlag = NC_TRUE;
#endif

            configNcm = NC_TRUE;
        }
        else {
            NCLOGD("NCNetworkStartFirewallReq::doAction, Parameters error");
            return;
        }

        NCSystem systemProxy;
        if (configDefault || configNcm) {
            NCSTARTPERF_KSTART;
            if (configDefault) {
                NCSTARTWARN_OUT_1S("Network", "To set Firewall:", "default");
            }
            else if (logNcmFlag) {
                NCSTARTWARN_OUT_1S("Network", "To set Firewall:", logNcmIndividualFlag ? "default" : "NCM");
            }
            NCSTARTPERF_END;

            INT configRet = system(configCmd);
            if (handler.checkSystemReturn(configRet)) {
                if (configDefault) {
                    NCLOGD("NCNetworkStartFirewallReq::doAction, Config default rules success");
                    NCNetworkManager::Instance()->setDefaultNcmFlag(NC_TRUE, NC_FALSE);
                }
                else {
                    NCLOGD("NCNetworkStartFirewallReq::doAction, Config all rules success");
                    NCNetworkManager::Instance()->setDefaultNcmFlag(NC_TRUE, NC_TRUE);
                    systemProxy.firewallRecovery();
                }
            }
            else {
                NCCHAR clearCmd[128] = { 0 };
                if (configDefault) {
                    NCLOGD("NCNetworkStartFirewallReq::doAction, Config default rules failed");
                    snprintf(clearCmd, sizeof(clearCmd)-1, ". %s %d", script.getString(), FirewallType_ClearDefault);
                    NCNetworkManager::Instance()->setFirewallFailedFlag(NC_TRUE, NC_FALSE);
                    NCErrorLog(NetworkErrCode_Firewall_StartFailed, 0);
                }
                else {
                    if (handler.checkIndividualApparatus()) {
                        snprintf(clearCmd, sizeof(clearCmd)-1, ". %s %d", script.getString(), FirewallType_ClearDefault);
                    }
                    else {
                        snprintf(clearCmd, sizeof(clearCmd)-1, ". %s %d %s", script.getString(), FirewallType_ClearNcm, usbName.getString());
                    }

                    NCNetworkManager::Instance()->setFirewallFailedFlag(NC_TRUE, NC_TRUE);

                    NCErrorLog(NetworkErrCode_Firewall_StartFailed, 0);
                    if (!systemProxy.firewallReset()) {
                        NCLOGD("NCNetworkStartFirewallReq::doAction, Firewall reset return false");
                        handler.notifyFirewallFailed();
                    }
                }
                
                INT clearReturn = system(clearCmd);
                if (handler.checkSystemReturn(clearReturn)) {
                    NCLOGD("NCNetworkStartFirewallReq::doAction, Clear rules success");
                }
                else {
                    NCLOGD("NCNetworkStartFirewallReq::doAction, Clear rules failed");
                }
            }
        }

    }

    NCNetworkStopCurrentProcessReq::NCNetworkStopCurrentProcessReq()
    {

    }

    NCNetworkStopCurrentProcessReq::~NCNetworkStopCurrentProcessReq()
    {

    }

    VOID
    NCNetworkStopCurrentProcessReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkStopCurrentProcessReq::doAction, Start");
        // Stop named process
        NCNetworkHandler handler;
        handler.stopDnsCacheServer();

        // Stop dhclient process
        NCCHAR pidFile[] = DHCPCLIENT_PID_FILE;
        FILE* fp = fopen(pidFile, "r");
        if (NULL == fp) {
            NCLOGD("NCNetworkStopCurrentProcessReq::doAction, Open pid file failed");
        }
        else {
            NCCHAR buf[128] = { 0 };
            while (NULL != fgets(buf, sizeof(buf) -1, fp)) {
                NCCHAR device[32] = { 0 };
                sscanf(buf, "%31s ", device);
                NCLOGD("NCNetworkStopCurrentProcessReq::doAction, devie [%s]", device);
                handler.releaseIp(device);
            }
            fclose(fp);
        }

        // Stop dhcpd process
        handler.stopDhcpServer("");
    }


    NCNetworkSetDefaultNetworkReq::NCNetworkSetDefaultNetworkReq(const UINT32 lastRequest, const DeviceRouteDnsInfo& info)
    : NCNetworkReqBase(RequestType_SetDefaultNetwork)
    , m_lastRequest(lastRequest)
    , m_deviceType(info.deviceType)
    , m_deviceName(info.deviceName)
    , m_gateway(info.gateway)
    , m_dns1(info.dns1)
    , m_dns2(info.dns2)
    {

    }

    NCNetworkSetDefaultNetworkReq::~NCNetworkSetDefaultNetworkReq()
    {

    }

    VOID
    NCNetworkSetDefaultNetworkReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkSetDefaultNetworkReq::setDefaultRoute, Start, lastRequest [%d], deviceType [%s], deviceName [%s], gateway [%s], dns1 [%s], dns2[%s]", 
            m_lastRequest, m_deviceType.getString(), m_deviceName.getString(), m_gateway.getString(), m_dns1.getString(), m_dns2.getString());
        m_thread = pThread;
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        if (NULL == myStatus || NULL == myDeviceType) {
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            NCLOGD("NCNetworkRequestAccessReq::doAction, Get pointer failed");
            return;
        }

        myDeviceType->add(m_deviceName, m_deviceType);
        myStatus->setDialState(m_deviceType, DialState_Connecting, DialType_None);

        NCNetworkHandler handler;
        handler.clearNatTable();

        // update dns
        if (NCString("") != m_dns1 || NCString("") != m_dns2) {
            handler.updateNamedConf(NCNETWORK_NAMED_DNS_OPTIONS, m_dns1, m_dns2);
        }

#if defined NCNETWORK_TARGET_PRODUCT_MEU
        // update gbook dns
        NCString dns1, dns2;
        if (WIFI_STA == m_deviceType || BT_DUN == m_deviceType) {
            myStatus->getGbookWifiDns(dns1, dns2);
        }
        else if (DCM == m_deviceType) {
            myStatus->getGbookDcmDns(dns1, dns2);
        }

        if (NCString("") != dns1 || NCString("") != dns2) {
            handler.updateNamedConf(NCNETWORK_GBOOK_DNS_ZONE, dns1, dns2);
        }

        handler.setNatTable(m_deviceType);
#endif

#if defined NCNETWORK_TARGET_PRODUCT_DCU
        NCString vendor = NCNetworkManager::Instance()->getExtunitVendor();
        NCLOGD("NCNetworkSetDefaultNetworkReq::setDefaultRoute, Vendor [%s]", vendor.getString());
        if (NCString("pana") == vendor && DCM == m_deviceType) {
            NCLOGD("NCNetworkSetDefaultNetworkReq::setDefaultRoute, Pana meu, do not set NAT");
        }
        else {
            handler.setNatTable(m_deviceType);
        }
#endif

        handler.addDefaultRoute(m_deviceName, m_gateway);
        m_thread->waitRequest(1000*10);
    }


    VOID
    NCNetworkSetDefaultNetworkReq::doTimeout()
    {
        NCLOGD("NCNetworkSetDefaultNetworkReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        else {
            m_reqState = REQ_TIMEOUT;
        }

        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkAvcUnit* myAvcUnit = NCNetworkManager::Instance()->getNetworkAvcUnit();
        NCRunnableThread* myEventThread = NCNetworkManager::Instance()->getEventThreadPointer();
        if (NULL == myStatus || NULL == myAvcUnit || NULL == myEventThread) {
            NCLOGD("NCNetworkSetDefaultNetworkReq::doAction, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        NCNetworkHandler handler;
        handler.removeDefaultRoute(m_deviceName);
        handler.clearNatTable();

        myStatus->setCurrentNetwork("");
        myStatus->setDialState(m_deviceType, DialState_Init, DialType_None);

        NCNetworkNotify notify;
        if (handler.checkIndividualApparatus()) {
            NCLOGD("NCNetworkSetDefaultNetworkReq::doTimeout, DCU is individual apparatus");
            if (WIFI_STA == m_deviceType && RequestType_RequestIp == m_lastRequest) {
                notify.notifyRequestIpResult(m_deviceType, NetworkErrCode_Timeout);
            }
            else {
                myEventThread->looper().postRunnable(new NCNetworkPopAllRequestMapReq(RequestMapType_Callback, PopResultFlag_Timeout));
            }
        }
        else {
            NCLOGD("NCNetworkSetDefaultNetworkReq::doTimeout, DCU with MEU mode");
            NC_BOOL isDevExist = handler.isCarryNetworkDevice(m_deviceType);
            NCLOGD("NCNetworkSetDefaultNetworkReq::doTimeout, isDevExist [%d]", isDevExist);
#if defined NCNETWORK_TARGET_PRODUCT_DCU
            if (!isDevExist) {
                myAvcUnit->setFlag(NC_FALSE);
                myEventThread->looper().postRunnable(new NCNetworkPopRequestMapReq(RequestMapType_Init, RequestType_Access, m_deviceType, 
                    NetworkErrCode_Timeout));

                myEventThread->looper().postRunnable(new NCNetworkPopRequestMapReq(RequestMapType_Callback, RequestType_Access, m_deviceType, 
                    NetworkErrCode_Timeout));
            }
            else {
                if (WIFI_STA == m_deviceType && RequestType_RequestIp == m_lastRequest) {
                    notify.notifyRequestIpResult(m_deviceType, NetworkErrCode_Timeout);
                }
                else {
                    myEventThread->looper().postRunnable(new NCNetworkPopAllRequestMapReq(RequestMapType_Callback, PopResultFlag_Timeout));
                }
            }
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
            if (!isDevExist) {
                myAvcUnit->setFlag(NC_FALSE);
                myEventThread->looper().postRunnable(new NCNetworkPopRequestMapReq(RequestMapType_Init, RequestType_Access, m_deviceType, 
                    NetworkErrCode_Timeout));

                myEventThread->looper().postRunnable(new NCNetworkPopRequestMapReq(RequestMapType_Callback, RequestType_Access, m_deviceType, 
                    NetworkErrCode_Timeout));
            }
            else {
                myEventThread->looper().postRunnable(new NCNetworkPopAllRequestMapReq(RequestMapType_Callback, PopResultFlag_Timeout));
            }
#endif 
        }

        m_thread->notifyRequest();
    }

    UINT32
    NCNetworkSetDefaultNetworkReq::getLastRequest()
    {
        return m_lastRequest;
    }

    NCString
    NCNetworkSetDefaultNetworkReq::getDeviceType()
    {
        return m_deviceType;
    }


    NCNetworkClearDefaultNetworkReq::NCNetworkClearDefaultNetworkReq(const UINT32 lastRequest, const NCString& deviceType, const NC_BOOL remoteFlag)
    : NCNetworkReqBase(RequestType_ClearDefaultNetwork)
    , m_lastRequest(lastRequest)
    , m_deviceType(deviceType)
    , m_remoteFlag(remoteFlag)
    {

    }

    NCNetworkClearDefaultNetworkReq::~NCNetworkClearDefaultNetworkReq()
    {

    }

    VOID
    NCNetworkClearDefaultNetworkReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkClearDefaultNetworkReq::doAction, Start, last request [%d], device type [%s], remote flag [%d]", 
            m_lastRequest, m_deviceType.getString(), m_remoteFlag);
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        NCRunnableThread* myEventThread = NCNetworkManager::Instance()->getEventThreadPointer();
        if (NULL == myStatus || NULL == myDeviceType || NULL == myEventThread) {
            NCLOGD("NCNetworkClearDefaultNetworkReq::doAction, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        m_thread = pThread;
        NCString currNetwork = myStatus->currentNetwork();
        NCLOGD("NCNetworkClearDefaultNetworkReq::doAction, Current network [%s]", currNetwork.getString());

        if (m_deviceType != currNetwork) {
            NCLOGD("NCNetworkClearDefaultNetworkReq::doAction, Can not clear current network");
            myEventThread->looper().postRunnable(new NCNetworkPopRequestMapReq(RequestMapType_Callback, RequestType_Disconnect, m_deviceType, NetworkErrCode_Success));
            return;
        }

        NCString deviceName;
        myDeviceType->getName(m_deviceType, deviceName);
        NCLOGD("NCNetworkClearDefaultNetworkReq::doAction, Device name [%s]", deviceName.getString());

        NCNetworkHandler handler;
        handler.setFirewallForDcm(m_deviceType, deviceName);

        myStatus->setDialState(m_deviceType, DialState_Disconnecting, DialType_None);
        handler.removeDefaultRoute(deviceName);
        m_thread->waitRequest(1000*10);
    }

    NCString
    NCNetworkClearDefaultNetworkReq::getDeviceType()
    {
        return m_deviceType;
    }

    UINT32
    NCNetworkClearDefaultNetworkReq::getLastRequest()
    {
        NCLOGD("NCNetworkClearDefaultNetworkReq::getLastRequest, remoteFlag [%d]", m_remoteFlag);
        return m_lastRequest;
    }

    NC_BOOL
    NCNetworkClearDefaultNetworkReq::getRemoteFlag()
    {
        NCLOGD("NCNetworkClearDefaultNetworkReq::getRemoteFLag, remoteFlag [%d]", m_remoteFlag);
        return m_remoteFlag;
    }

    VOID
    NCNetworkClearDefaultNetworkReq::doTimeout()
    {
        NCLOGD("NCNetworkClearDefaultNetworkReq::doTimeout - Start");
        NCAutoSync syncState(m_syncState);
        if (REQ_CONFIRMED == m_reqState) {
            return;
        }
        m_reqState = REQ_TIMEOUT;

        NCRunnableThread* myEventThread = NCNetworkManager::Instance()->getEventThreadPointer();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        if (NULL == myEventThread || NULL == myStatus) {
            NCLOGD("NCNetworkClearDefaultNetworkReq::doTimeout, Get pointer failed");
            return;
        }

        NCNetworkNotify notify;
        myStatus->setDialState(m_deviceType, DialState_Disconnected, DialType_None);
        myStatus->setCurrentNetwork("");
        myEventThread->looper().postRunnable(new NCNetworkPopRequestMapReq(RequestMapType_Callback, RequestType_Disconnect, m_deviceType, NetworkErrCode_Success));

#if defined NCNETWORK_TARGET_PRODUCT_DCU
        if (DCM == m_deviceType) {
            NCNetworkManager::Instance()->startWifiApTimer();
        }
#endif

        if (m_thread) {
            m_thread->notifyRequest();
        }
    }


    NCNetworkCheckNcmConnectReq::NCNetworkCheckNcmConnectReq()
    {

    }

    NCNetworkCheckNcmConnectReq::~NCNetworkCheckNcmConnectReq()
    {

    }

    VOID
    NCNetworkCheckNcmConnectReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkCheckNcmConnectReq::doAction, Start");
#if defined NCNETWORK_TARGET_PRODUCT_EMULATOR
        NCLOGD("NCNetworkCheckNcmConnectReq::doAction, Emulator");
        return;
#endif

        DeviceIpInfo remoteInfo = NCNetworkManager::Instance()->getRemoteAvcLanIpInfo();
        if (NCString("") == remoteInfo.ip) {
            NCLOGD("NCNetworkCheckNcmConnectReq::doAction, Remote ip error");
            return;
        }

        while (ping(remoteInfo.ip.getString(), 1000)) {
            NCLOGD("NCNetworkCheckNcmConnectReq::doAction, Socket connect failed");
        }
        
        NCSTARTPERF_KSTART;
        NCSTARTWARN_OUT("Network", "NCM connected.");
        NCSTARTPERF_END;

        NCNetworkNotify notify;
        notify.notifyNcmIpFinished();
    }

    UINT16
    NCNetworkCheckNcmConnectReq::checkSum(UINT16 *addr, INT len)
    {
        INT nleft = len;
        INT sum = 0;
        UINT16 *w = addr;
        UINT16 answer = 0;

        while (nleft > 1) {
            sum += *w++;
            nleft -= 2;
        }

        if (nleft == 1) {
            *(UCHAR *)(&answer) = *(UCHAR *)w;
            sum += answer;
        }

        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        answer = ~sum;

        return answer;
    }

    INT
    NCNetworkCheckNcmConnectReq::ping(const CHAR *ips, INT timeout)
    {
        NCLOGD("NCNetworkCheckNcmConnectReq::ping, Start, ips [%s], timeout [%d]", ips, timeout);
        struct timeval timeo;
        INT sockfd;
        struct sockaddr_in addr;
        struct sockaddr_in from;
          
        struct timeval *tval = NULL;
        struct ip *iph = NULL;
        struct icmp *icmp = NULL;
      
        CHAR sendpacket[PACKET_SIZE];
        CHAR recvpacket[PACKET_SIZE];
          
        INT n;
        pid_t pid;
        INT maxfds = 0;
        fd_set readfds;
        
        // set IP info
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ips);
      
        sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (sockfd < 0) {
            NCLOGD("NCNetworkCheckNcmConnectReq::ping, Socket error");
            return -1;
        }
          
        // set timeout
        timeo.tv_sec = timeout / 1000;
        timeo.tv_usec = timeout % 1000;
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo)) == -1) {
            NCLOGD("NCNetworkCheckNcmConnectReq::ping, Setsockopt error");
            close(sockfd);
            return -1;
        }
    
        // make PING packet
        memset(sendpacket, 0, sizeof(sendpacket));
    
        pid = getpid();
        INT i, packsize;
        icmp = (struct icmp*)sendpacket;
        icmp->icmp_type = ICMP_ECHO;
        icmp->icmp_code = 0;
        icmp->icmp_seq = 0;
        icmp->icmp_id = pid;
        packsize = 8 + 56;
        tval = (struct timeval *)icmp->icmp_data;
        gettimeofday(tval, NULL);
        icmp->icmp_cksum = checkSum((UINT16 *)icmp, packsize);
      
        // send packet
        n = sendto(sockfd, (CHAR *)&sendpacket, packsize, 0, (struct sockaddr *)&addr, sizeof(addr));
        if (n < 1) {
            NCLOGD("NCNetworkCheckNcmConnectReq::ping, Sendto error");
            close(sockfd);
            return -1;
        }
      
        // receive packet
        while (1) {
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);
            maxfds = sockfd + 1;
            n = select(maxfds, &readfds, NULL, NULL, &timeo);
            if (n <= 0) {
                NCLOGD("NCNetworkCheckNcmConnectReq::ping, Select timeout error");
                close(sockfd);
                return -1;
            }
      
            memset(recvpacket, 0, sizeof(recvpacket));
            INT fromlen = sizeof(from);
            n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from, (socklen_t*)&fromlen);
            if (n < 1) {
                NCLOGD("NCNetworkCheckNcmConnectReq::ping, Recvfrom error");
                close(sockfd);
                return -1;
            }
              
            // check the PING packet source
            CHAR *from_ip = (CHAR *)inet_ntoa(from.sin_addr);
             if (strcmp(from_ip, ips) != 0) {
                NCLOGD("NCNetworkCheckNcmConnectReq::ping, From ip [%s] is incorrect", from_ip);
                continue;
             }
            
            iph = (struct ip *)recvpacket;
            icmp = (struct icmp *)(recvpacket + (iph->ip_hl << 2));

           // check PING packet type
            if (icmp->icmp_type == ICMP_ECHOREPLY && icmp->icmp_id == pid) {
                NCLOGD("NCNetworkCheckNcmConnectReq::ping, Reply success");
                break;
            }
            else {
                NCLOGD("NCNetworkCheckNcmConnectReq::ping, Reply type or id incorrect");
                continue;
            }
        }
          
        // close socket
        close(sockfd);
        return 0;
    }

    NCNetworkSetExtunitVendorReq::NCNetworkSetExtunitVendorReq()
    {

    }

    NCNetworkSetExtunitVendorReq::~NCNetworkSetExtunitVendorReq()
    {

    }

    VOID
    NCNetworkSetExtunitVendorReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkSetExtunitVendorReq::doAction, Start");
        NCCHAR value[PROPERTY_VALUE_MAX] = { 0 };
        UINT32 i = 0;
        while (i < 40) {
            property_get(NCNETWORK_PROPERTY_EXTUNIT_VENDOR, value, NULL);
            if (value[0] == 0) {
                usleep(50 * 1000);
                ++i;
            }
            else {
                NCLOGD("NCNetworkSetExtunitVendorReq::doAction, i [%d], value [%s]", i, value);
                NCNetworkManager::Instance()->setExtunitVendor(value);
                return;
            }
        }
    }

    NCNetworkStartMonitorDataCommReq::NCNetworkStartMonitorDataCommReq(const NCString& deviceType, const NCString& deviceName)
    : NCNetworkReqBase(RequestType_StartMonitorDataComm)
    , m_deviceType(deviceType)
    , m_deviceName(deviceName)
    {

    }

    NCNetworkStartMonitorDataCommReq::~NCNetworkStartMonitorDataCommReq()
    {

    }

    VOID
    NCNetworkStartMonitorDataCommReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkStartMonitorDataCommReq::doAction, Start, deviceType [%s], deviceName [%s]", 
            m_deviceType.getString(), m_deviceName.getString());
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        if (NULL == myStatus) {
            NCLOGD("NCNetworkStartMonitorDataCommReq::doAction, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (WIFI_AP == m_deviceType) {
            myStatus->setWifiApConnectedStatus(NC_TRUE);
            NCNetworkManager::Instance()->startWifiApTimer();
        }
        else {
            NCLOGD("NCNetworkStartMonitorDataCommReq::doAction, deviceType error");
        }
    }

    NCNetworkStopMonitorDataCommReq::NCNetworkStopMonitorDataCommReq(const NCString& deviceType, const NCString& deviceName)
    : NCNetworkReqBase(RequestType_StopMonitorDataComm)
    , m_deviceType(deviceType)
    , m_deviceName(deviceName)
    {

    }

    NCNetworkStopMonitorDataCommReq::~NCNetworkStopMonitorDataCommReq()
    {

    }

    VOID
    NCNetworkStopMonitorDataCommReq::doAction(NCSpThread<NCNetworkReqBase>* pThread)
    {
        NCLOGD("NCNetworkStopMonitorDataCommReq::doAction, Start, deviceType [%s], deviceName [%s]", 
            m_deviceType.getString(), m_deviceName.getString());
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        if (NULL == myStatus) {
            NCLOGD("NCNetworkStopMonitorDataCommReq::doAction, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (WIFI_AP == m_deviceType) {
            myStatus->setWifiApConnectedStatus(NC_FALSE);
            NCNetworkManager::Instance()->stopTimer(TimerType_WifiAp);
        }
        else {
            NCLOGD("NCNetworkStopMonitorDataCommReq::doAction, deviceType error");
        }
    }

} /* namespace nutshell */
/* EOF */
