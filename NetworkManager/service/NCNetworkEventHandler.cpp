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

#include <ncore/NCLog.h>
#include <ncore/NCTimeDefine.h>
#include <ncore/NCError.h>
#include <nceventsys/NCEvent.h>
#include <nhinterface/NINetworkHandler.h>
#include <nhinterface/NINetworkDefine.h>
#include <nhinterface/NIAvcLanHandler.h>
#include <nhinterface/NIBtHfpHandler.h>
#include <NDNetworkEvent_generated.h>
#include <NDAvcLanEvent_generated.h>
#include <NDBtHfp_generated.h>
#include <system/NCSystem.h>
#include <system/NCEnvironment.h>

#include "NCNetworkManager.h"
#include "NCNetworkNotify.h"
#include "NCNetworkDeviceType.h"
#include "NCNetworkStatus.h"
#include "NCNetworkAvcUnit.h"
#include "NCNetworkReqConfirm.h"
#include "NCNetworkAccessStack.h"
#include "NCNetworkHandler.h"
#include "NCNetworkDiagLog.h"
#include "NCNetworkPluginManager.h"
#include "NCNetworkThread.h"
#include "NCNetworkEventReq.h"
#include "NCNetworkEventHandler.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{
    NCNetworkEventHandler::NCNetworkEventHandler(const NCRunnableLooper& looper)
    : NCEventHandler(looper)
    {
    }

    NCNetworkEventHandler::~NCNetworkEventHandler()
    {
    }

    VOID
    NCNetworkEventHandler::handleEvent(const NCEvent& event)
    {
        NCLOGD("NCNetworkEventHandler::handleEvent - Start");
        NCNetworkNotify* notify = new NCNetworkNotify();

        NCString eventSpace = event.space();

        size_t size = event.extraDataSize();
        char *buff = new char[size];
        memset(buff, 0, size);
        event.getExtraData(buff, size);

        flatbuffers::Verifier verifier((uint8_t*)buff, size);

        if (eventSpace == NCString(GET_NIEVENTNAMESPACE(NINetworkHandler))) {
            UINT32 eventId = event.identity();
            switch (eventId) {
            case NINetworkHandler::EventId_DoDhcp:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - Receive event, DoDhcp");
                if (verifier.VerifyBuffer<NDNetworkDoDhcp>()) {
                    const NDNetworkDoDhcp* data = flatbuffers::GetRoot<NDNetworkDoDhcp>(buff);
                    if (NULL != data) {
                        UINT32 result = data->result();
                        NCString deviceName = data->name()->c_str();
                        NCString gateway = data->gateway()->c_str();
                        NCString dns1 = data->dns1()->c_str();
                        NCString dns2 = data->dns2()->c_str();

                        doDhcpEvent(deviceName, gateway, dns1, dns2, result);
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::handleEvent, DoDhcp, Parse NDNetworkDoDhcp failed");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, DoDhcp, Verify NDNetworkDoDhcp failed");
                }
            }
            break;
            case NINetworkHandler::EventId_ReleaseDhcpLease:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - Receive event, ReleaseDhcpLease");
                if (verifier.VerifyBuffer<NDNetworkReleaseDhcpLease>()) {
                    const NDNetworkReleaseDhcpLease* data = flatbuffers::GetRoot<NDNetworkReleaseDhcpLease>(buff);
                    if (NULL != data) {
                        UINT32 result = data->result();
                        NCString deviceName = data->name()->c_str();
                        doReleaseIpEvent(deviceName, result);
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::handleEvent, ReleaseDhcpLease, Parse NDNetworkReleaseDhcpLease failed");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, ReleaseDhcpLease, Verify NDNetworkReleaseDhcpLease failed");
                }
            }
            break;
            case NINetworkHandler::EventId_SetStaticIpMaskMtu:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - EventId_SetStaticIpMaskMtu, Start");
                if (verifier.VerifyBuffer<NDNetworkSetStaticIpMaskMtu>()) {
                    const NDNetworkSetStaticIpMaskMtu* data = flatbuffers::GetRoot<NDNetworkSetStaticIpMaskMtu>(buff);
                    if (NULL != data) {
                        UINT32 result = data->result();
                        NCString deviceName = data->name()->c_str();
                        doSetStaticIpMaskMtu(deviceName, result);
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::::handleEvent, EventId_SetStaticIpMaskMtu, Parse NDNetworkSetStaticIp failed");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::::handleEvent, EventId_SetStaticIpMaskMtu, Verify NDNetworkSetStaticIp failed");
                }
            }
            break;
            case NINetworkHandler::EventId_RemoveDefaultRoute:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - RemoveDefaultRoute, Start");
                if (verifier.VerifyBuffer<NDNetworkRemoveDefaultRoute>()) {
                    const NDNetworkRemoveDefaultRoute* data = flatbuffers::GetRoot<NDNetworkRemoveDefaultRoute>(buff);
                    if (NULL != data) {
                        UINT32 result = data->result();
                        NCString deviceName = data->name()->c_str();
                        doRmDefaultRoute(deviceName, result);
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::handleEvent, RemoveDefaultRoute, Parse NDNetworkRemoveDefaultRoute failed");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, RemoveDefaultRoute, Verify NDNetworkRemoveDefaultRoute failed");
                }
            }
            break;
            case NINetworkHandler::EventId_AddDefaultRoute:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - Receive event, AddDefaultRoute");
                if (verifier.VerifyBuffer<NDNetworkAddDefaultRoute>()) {
                    const NDNetworkAddDefaultRoute* data = flatbuffers::GetRoot<NDNetworkAddDefaultRoute>(buff);
                    if (NULL != data) {
                        UINT32 result = data->result();
                        NCString deviceName = data->name()->c_str();
                        doAddDefaultRoute(deviceName, result);
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::handleEvent, AddDefaultRoute, Parse NDNetworkAddDefaultRoute failed");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, AddDefaultRoute, Verify NDNetworkAddDefaultRoute failed");
                }
            }
            break;
            case NINetworkHandler::EventId_NetworkDeviceInfo:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - Receive event, NetworkDeviceInfo");
                if (verifier.VerifyBuffer<NDNetworkDevice>()) {
                    NCLOGD("NINetworkHandler::EventId_NetworkDeviceInfo - Check data validity success");
                    const NDNetworkDevice* data = flatbuffers::GetRoot<NDNetworkDevice>(buff);
                    if (NULL != data) {
                        NCLOGD("NCNetworkEventHandler::handleEvent, NetworkDeviceInfo - Parse data success");
                        UINT32 result = data->result();
                        NCString device = data->name()->c_str();
                        NCNetworkDataReplier::NetworkDeviceInfo info;
                        info.ip = data->ip()->c_str();
                        info.mac = data->mac()->c_str();
                        info.prefixLength = data->prefixLength();
                        info.gateway = data->gateway()->c_str();
                        info.dns1 = data->dns1()->c_str();
                        info.dns2 = data->dns2()->c_str();
                        info.serverId = data->serverId()->c_str();

                        doDeviceInfo(device, info, result);
                    }
                }
            }
            break;
            case NINetworkHandler::EventId_ConnectedClientInfo:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - Receive event, ConnectedClientInfo");
                if (verifier.VerifyBuffer<NDNetworkClient>()) {
                    NCLOGD("NINetworkHandler::handleEvent, ConnectedClientInfo - Check data validity success");
                    const NDNetworkClient* data = flatbuffers::GetRoot<NDNetworkClient>(buff);
                    if (NULL != data) {
                        NCLOGD("NCNetworkEventHandler::handleEvent, ConnectedClientInfo - Parse data success");
                        UINT32 result = data->result();
                        if (NC_NOERROR == result) {
                            NCLOGD("NCNetworkEventHandler::handleEvent, ConnectedClientInfo - Result is success");
                            NCNetworkDataReplier::ConnectedClientInfo info;
                            NCString mac = data->mac()->c_str();
                            info.ip = data->ip()->c_str();
                            info.name = data->hostName()->c_str();
                            notify->notifyConnectedClientInfo(mac, info);
                        }
                    }
                }

            }
            break;
            case NINetworkHandler::EventId_StartDhcpServer:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - Receive event, StartDhcpServer");
                if (verifier.VerifyBuffer<NDNetworkStartDhcpServer>()) {
                    const NDNetworkStartDhcpServer* data = flatbuffers::GetRoot<NDNetworkStartDhcpServer>(buff);
                    if (NULL != data) {
                        UINT32 result = data->result();
                        NCString deviceName = data->name()->c_str();
                        doStartDhcpServer(deviceName, result);
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::handleEvent, StartDhcpServer, Parse NDNetworkDoDhcp failed");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, StartDhcpServer, Verify NDNetworkDoDhcp failed");
                }
            }
            break;
            case NINetworkHandler::EventId_StopDhcpServer:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - Receive event, StopDhcpServer");
                if (verifier.VerifyBuffer<NDNetworkStopDhcpServer>()) {
                    const NDNetworkStopDhcpServer* data = flatbuffers::GetRoot<NDNetworkStopDhcpServer>(buff);
                    if (NULL != data) {
                        UINT32 result = data->result();
                        NCString deviceName = data->name()->c_str();
                        doStopDhcpServer(deviceName, result);
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::handleEvent, StopDhcpServer, Parse NDNetworkStopDhcpServer failed");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, StopDhcpServer, Verify NDNetworkStopDhcpServer failed");
                }
            }
            break;
            case NINetworkHandler::EventId_RestartDhcpServer:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - Receive event, RestartDhcpServer");
                if (verifier.VerifyBuffer<NDNetworkStartDhcpServer>()) {
                    const NDNetworkStartDhcpServer* data = flatbuffers::GetRoot<NDNetworkStartDhcpServer>(buff);
                    if (NULL != data) {
                        UINT32 result = data->result();
                        NCString deviceName = data->name()->c_str();
                        doRestartDhcpServer(deviceName, result);
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::handleEvent, RestartDhcpServer, Parse NDNetworkDoDhcp failed");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, RestartDhcpServer, Verify NDNetworkDoDhcp failed");
                }
            }
            break;
            case NINetworkHandler::EventId_AddRoute:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent - Receive event, AddRoute");
                if (verifier.VerifyBuffer<NDNetworkAddRoute>()) {
                    const NDNetworkAddRoute* data = flatbuffers::GetRoot<NDNetworkAddRoute>(buff);
                    if (NULL != data) {
                        UINT32 result = data->result();
                        NCString deviceName = data->name()->c_str();
                        doAddRoute(deviceName, result);
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::handleEvent, AddRoute, Parse NDNetworkDoDhcp failed");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, AddRoute, Verify NDNetworkDoDhcp failed");
                }
            }
            break;
            default:
                break;
            }
        }
        else if (eventSpace == NCString(GET_NIEVENTNAMESPACE(NIAvcLanHandler))) {
            UINT32 eventId = event.identity();
            switch (eventId) {
            case NIAvcLanHandler::EventId_ConnectStatus:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent, AvcLan authentication, Start");
                if (verifier.VerifyBuffer<NDExtAvcConnectStatus>()) {
                    NCLOGD("NCNetworkEventHandler::handleEvent, AvcLan authentication, Verify buffer success");
                    const NDExtAvcConnectStatus* data = flatbuffers::GetRoot<NDExtAvcConnectStatus>(buff);
                    if (NULL != data) {
                        NIAvcLanHandler::ConnectStatus status = (NIAvcLanHandler::ConnectStatus)data->connectStatus();
                        if (status == NIAvcLanHandler::ConnectStatus_AuthenticationSuccessful
                            || status == NIAvcLanHandler::ConnectStatus_CommandEnabled) {
                            NCLOGD("NCNetworkEventHandler::handleEvent - AvcLan authentication, Success");
                            NCNetworkManager::Instance()->postRequest(new NCNetworkSetExtunitVendorReq());
                            NCNetworkManager::Instance()->postRequest(new NCNetworkStartFirewallReq(FirewallType_Ncm));
                            NCNetworkManager::Instance()->postRequest(new NCNetworkSetAvcLanIpReq());
                        }
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, AvcLan authentication, Verify buffer failed");
                }

            }
            break;
            case NIAvcLanHandler::EventId_IndividualApparatus:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent, Individual apparatus message");
                NCNetworkManager::Instance()->postRequest(new NCNetworkStartFirewallReq(FirewallType_Ncm));
            }
            break;
            default:
                break;
            }
        }
        else if (eventSpace == NCNETWORK_WIFI_EVENT_SPACENAME) {
            NCLOGD("NCNetworkEventHandler::handleEvent, NCWIFI");
            NCString eventIdStr = event.identity(NULL);
            if (NCNETWORK_WIFI_EVENT_P2P_DEVICE_DISCONNECTED == eventIdStr) {
                NCLOGD("NCNetworkEventHandler::handleEvent, MirrorLink disconnected");
                // NCNetworkManager::Instance()->getNetworkStatus()->setMirrorLinkStatus(NC_FALSE);
            }
            else if (NCNETWORK_WIFI_EVENT_P2P_DEVICE_CONNECTED == eventIdStr) {
                NCLOGD("NCNetworkEventHandler::handleEvent, MirrorLink connected");
                // NCNetworkManager::Instance()->getNetworkStatus()->setMirrorLinkStatus(NC_TRUE);
            }
        }
        else if (eventSpace == NCString(GET_NIEVENTNAMESPACE(NIBtHfpHandler))) {
            NCLOGD("NCNetworkEventHandler::handleEvent, BT HFP");
            UINT32 eventId = event.identity();
            switch (eventId) {
            case NIBtHfpHandler::CFM_HFP_GET_OPERATOR_NAME:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent, Operator name start");
                if (verifier.VerifyBuffer<NDBtGetOperatorNameCfmEvent>()) {
                    NCLOGD("NCNetworkEventHandler::handleEvent, Operator name, Verify buffer success");
                    const NDBtGetOperatorNameCfmEvent* data = flatbuffers::GetRoot<NDBtGetOperatorNameCfmEvent>(buff);
                    if (NULL != data) {
                        UINT32 status = data->status();
                        NCLOGD("NCNetworkEventHandler::handleEvent, Operator name, status [%d]", status);
                        if (0 == status) {
                            NC_BOOL dunStatus = NCNetworkManager::Instance()->getNetworkStatus()->status(BT_DUN);
                            if (dunStatus) {
                                NCString name = data->name()->c_str();
                                NCLOGD("NCNetworkEventHandler::handleEvent, Operator name, name [%s]", name.getString());
                                NCNetworkManager::Instance()->getNetworkStatus()->setDunCarrierInfo(name);
                            }
                            else {
                                NCLOGD("NCNetworkEventHandler::handleEvent, Operator name, Dun status is False");
                            }
                        }
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::handleEvent, Operator name, Data is null");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, Operator name, Verify buffer failed");
                }
            }
            break;
            case NIBtHfpHandler::IND_HFP_SERVICE:
            {
                NCLOGD("NCNetworkEventHandler::handleEvent, HFP service start");
                if (verifier.VerifyBuffer<NDBtServiceIndEvent>()) {
                    NCLOGD("NCNetworkEventHandler::handleEvent, HFP service, Verify buffer success");
                    const NDBtServiceIndEvent* data = flatbuffers::GetRoot<NDBtServiceIndEvent>(buff);
                    if (NULL != data) {
                        UINT16 value = data->value();
                        NCLOGD("NCNetworkEventHandler::handleEvent, HFP service, value [%d]", value);
                        if (0 == value) {
                            NCString name = "";
                            NCLOGD("NCNetworkEventHandler::handleEvent, HFP service, name [%s]", name.getString());
                            NCNetworkManager::Instance()->getNetworkStatus()->setDunCarrierInfo(name);
                        }
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::handleEvent, HFP service, Data is null");
                    }
                }
                else {
                    NCLOGD("NCNetworkEventHandler::handleEvent, HFP service, Verify buffer failed");
                }
            }
            break;
            default:
                break;
            }
        }
        else {
            NCLOGD("NCNetworkEventHandler::handleEvent - eventspace [%s] is not exit", eventSpace.getString());
        }

        if (NULL != buff) {
            delete[] buff;
            buff = NULL;
        }

        if (notify != NULL) {
            delete notify;
            notify = NULL;
        }
    }

    VOID
    NCNetworkEventHandler::doSetStaticIpMaskMtu(const NCString& deviceName, const UINT32 result)
    {
        NCLOGD("NCNetworkEventHandler::doSetStaticIpMaskMtu, deviceName [%s], result [%d]", deviceName.getString(), result);
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        if (NULL == myStatus || NULL == myWorkThread || NULL == myDeviceType) {
            NCLOGD("NCNetworkEventHandler::doSetStaticIpMaskMtu, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        NCNetworkReqConfirm reqConfirm;
        if (myWorkThread->lockReqState(RequestType_SetAdbDebugIp)) {
            NCLOGD("NCNetworkEventHandler::doSetStaticIpMaskMtu, RequestType_SetAdbDebugIp");
            reqConfirm.confirm();
            myWorkThread->unlockReqState(RequestType_SetAdbDebugIp);
            return;
        }

        if (myWorkThread->lockReqState(RequestType_SetSlipDevice)) {
            NCLOGD("NCNetworkEventHandler::doSetStaticIpMaskMtu, RequestType_SetSlipDevice");
            if (NC_NOERROR == result) {
                NCString deviceType;
                myDeviceType->getType(deviceName, deviceType);
                NCLOGD("NCNetworkEventHandler::doSetStaticIpMaskMtu, deviceType [%s]", deviceType.getString());
                myStatus->setStatus(deviceType, NC_TRUE);
            }

            reqConfirm.confirm();
            myWorkThread->unlockReqState(RequestType_SetSlipDevice);
            return;
        }

        if (myWorkThread->lockReqState(RequestType_SetAvcLanIp)) {
            NCLOGD("NCNetworkEventHandler::doSetStaticIpMaskMtu, RequestType_SetAvcLanIp");
            if (NC_NOERROR == result) {
                myStatus->setStatus(USB_AVCLAN, NC_TRUE);
                NCNetworkManager::Instance()->postNcmThreadRequest(new NCNetworkCheckNcmConnectReq());
#if defined NCNETWORK_TARGET_PRODUCT_MEU
                NCNetworkManager::Instance()->postRequest(new NCNetworkAddRouteVRServer());
#endif
            }
            else {
                NCNetworkManager::Instance()->startTimer(TimerType_AvcLanIp);
            }

            reqConfirm.confirm();
            myWorkThread->unlockReqState(RequestType_SetAvcLanIp);
        }
    }

    VOID
    NCNetworkEventHandler::doDeviceInfo(const NCString& deviceName, const NCNetworkDataReplier::NetworkDeviceInfo& info, const UINT32 result)
    {
        NCLOGD("NCNetworkEventHandler::doDeviceInfo - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkEventHandler::doDeviceInfo - Start, deviceName [%s], gateway [%s], dns1 [%s], dns2 [%s], result [%d]", deviceName.getString(), 
            info.gateway.getString(), info.dns1.getString(), info.dns2.getString(), result);
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        if (NULL == myWorkThread || NULL == myDeviceType) {
            NCLOGD("NCNetworkEventHandler::doDeviceInfo, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        NCNetworkReqConfirm reqConfirm;
        if (myWorkThread->lockReqState(RequestType_Access)) {
            NCLOGD("NCNetworkEventHandler::doDeviceInfo, RequestType_Access");
            NCString accessType = reqConfirm.getDeviceType(RequestType_Access);
            NCString deviceType;
            myDeviceType->getType(deviceName, deviceType);
            if (WIFI_STA == accessType && WIFI_STA == deviceType) {
                DeviceRouteDnsInfo routeInfo;
                routeInfo.deviceType = deviceType;
                routeInfo.deviceName = deviceName;
                routeInfo.gateway = info.gateway;
                routeInfo.dns1 = info.dns1;
                routeInfo.dns2 = info.dns2;
                NCNetworkManager::Instance()->insertRequest(new NCNetworkSetDefaultNetworkReq(RequestType_Access, routeInfo));
                reqConfirm.confirm();
            }

            myWorkThread->unlockReqState(RequestType_Access);
        }

        if (myWorkThread->lockReqState(RequestType_DeviceInfo)) {
            NCLOGD("NCNetworkEventHandler::doDeviceInfo, RequestType_DeviceInfo");
            NCString deviceType;
            myDeviceType->getType(deviceName, deviceType);
            NCNetworkNotify notify;
            notify.notifyDeviceInfo(deviceType, info);
            reqConfirm.confirm();
            myWorkThread->unlockReqState(RequestType_DeviceInfo);
        }
    }

    VOID
    NCNetworkEventHandler::doDhcpEvent(const NCString& deviceName, const NCString& gateway, const NCString& dns1, const NCString& dns2, const UINT32 result)
    {
        NCLOGD("NCNetworkEventHandler::doDhcpEvent - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkEventHandler::doDhcpEvent, deviceName [%s], gateway [%s], dns1 [%s], dns2 [%s], result [%d]", deviceName.getString(), 
            gateway.getString(), dns1.getString(), dns2.getString(), result);
        NCNetworkDiagLog* myDialLog = NCNetworkManager::Instance()->getDiagLogPointer();
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        if (NULL == myDialLog || NULL == myWorkThread || NULL == myDeviceType || NULL == myStatus) {
            NCLOGD("NCNetworkEventHandler::doDhcpEvent, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (!myWorkThread->lockReqState(RequestType_RequestIp)) {
            NCLOGD("NCNetworkEventHandler::doDhcpEvent, Timeout");
            return;
        }

        NCString deviceType;
        myDeviceType->getType(deviceName, deviceType);
        NCLOGD("NCNetworkEventHandler::doDhcpEvent, Get deviceType [%s]", deviceType.getString());

        NCNetworkNotify notify;
        if (NC_NOERROR == result) {
            myStatus->setStatus(deviceType, NC_TRUE);
            // If wifi request ip success, set default route for it
            if (WIFI_STA == deviceType) {
                DeviceRouteDnsInfo routeInfo;
                routeInfo.deviceType = WIFI_STA;
                routeInfo.deviceName = deviceName;
                routeInfo.gateway = gateway;
                routeInfo.dns1 = dns1;
                routeInfo.dns2 = dns2;
                NCNetworkManager::Instance()->insertRequest(new NCNetworkSetDefaultNetworkReq(RequestType_RequestIp, routeInfo));
            }
            else {
                notify.notifyRequestIpResult(deviceType, NetworkErrCode_Success);
            }
        }
        else {
            if (WIFI_STA == deviceType) {
                // Notify wifi connect failed
                myStatus->setDialState(deviceType, DialState_Init, DialType_None);
            }

            notify.notifyRequestIpResult(deviceType, NetworkErrCode_Fail);

            NCNetworkHandler handler;
            NCErrorLog(NetworkErrCode_RequestIpFailed, handler.deviceTypeToNumber(deviceType));

            // Record diag log
            myDialLog->writeDiagLog(handler.deviceTypeToNumber(deviceType), NCNETWORK_DIAG_REQUEST_IP_FAILED, deviceName);
        }

        NCNetworkReqConfirm reqConfirm;
        reqConfirm.confirm();
        myWorkThread->unlockReqState(RequestType_RequestIp);
    }


    VOID
    NCNetworkEventHandler::doReleaseIpEvent(const NCString& deviceName, const UINT32 result)
    {
        NCLOGD("NCNetworkEventHandler::doReleaseIpEvent - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkEventHandler::doReleaseIpEvent, deviceName [%s], result [%d]", deviceName.getString(), result);
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        if (NULL == myWorkThread || NULL == myDeviceType || NULL == myStatus) {
            NCLOGD("NCNetworkEventHandler::doDhcpEvent, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (!myWorkThread->lockReqState(RequestType_ReleaseIp)) {
            NCLOGD("NCNetworkEventHandler::doReleaseIpEvent, Timeout");
            return; 
        }

        NCString deviceType;
        myDeviceType->getType(deviceName, deviceType);
        NCString currNetwork = myStatus->currentNetwork();
        NCLOGD("NCNetworkEventHandler::doReleaseIpEvent, Get device type [%s], Current network [%s]", 
            deviceType.getString(), currNetwork.getString());

        NCNetworkNotify notify;
        NCNetworkHandler handler;
        NCNetworkReqConfirm reqConfirm;
        if (NC_NOERROR == result) {
            myStatus->setStatus(deviceType, NC_FALSE);
            if (WIFI_STA != deviceType) {
                notify.notifyReleaseIpResult(deviceType, NetworkErrCode_Success);
                reqConfirm.confirm();
            }
            else {
                if (WIFI_STA == currNetwork) {
                    myStatus->setCurrentNetwork("");
                    if (handler.checkIndividualApparatus()) {
                        myStatus->setDialState(deviceType, DialState_Disconnected, DialType_None);
                        notify.notifyReleaseIpResult(deviceType, NetworkErrCode_Success);
                        reqConfirm.confirm();
                    }
                    else {
                        NCLOGD("NCNetworkEventHandler::doReleaseIpEvent, Release WIFI_STA, Wait meu disconnect");
                    }
                }
                else {
                    notify.notifyReleaseIpResult(deviceType, NetworkErrCode_Success);
                    reqConfirm.confirm();
                }
            }
        }
        else {
            if (WIFI_STA == deviceType && WIFI_STA == currNetwork) {
                myStatus->setDialState(deviceType, DialState_Init, DialType_None);
            }

            notify.notifyReleaseIpResult(deviceType, NetworkErrCode_Fail);
            NCErrorLog(NetworkErrCode_ReleaseIpFaile, handler.deviceTypeToNumber(deviceType));

            reqConfirm.confirm();
        }

        myWorkThread->unlockReqState(RequestType_ReleaseIp);
    }

    VOID
    NCNetworkEventHandler::doAddDefaultRoute(const NCString& deviceName, const UINT32 result)
    {
        NCLOGD("NCNetworkEventHandler::doAddDefaultRoute - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkEventHandler::doAddDefaultRoute - Start deviceName [%s], result [%d]", deviceName.getString(), result);
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkAvcUnit* myAvcUnit = NCNetworkManager::Instance()->getNetworkAvcUnit();
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        if (NULL == myStatus || NULL == myAvcUnit || NULL == myWorkThread || NULL == myDeviceType) {
            NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (!myWorkThread->lockReqState(RequestType_SetDefaultNetwork)) {
            NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Timeout");
            return;
        }

        NCString deviceType;
        myDeviceType->getType(deviceName, deviceType);
        if (DCM != deviceType && BT_DUN != deviceType && WIFI_STA != deviceType) {
            NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Get deviceType failed");
            myWorkThread->unlockReqState(RequestType_SetDefaultNetwork);
            return;
        }

        NCNetworkReqConfirm reqConfirm;
        UINT32 lastRequest = reqConfirm.getLastRequest(RequestType_SetDefaultNetwork);
        NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, deviceType [%s], lastRequest [%d]", 
            deviceType.getString(), lastRequest);

        NCNetworkNotify notify;
        NCNetworkHandler handler;
        if (handler.checkIndividualApparatus()) {
            NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Individual Apparatus");
            if (NC_NOERROR == result) {
                NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Success");
                handler.clearFirewallForDcm(deviceType);
                myStatus->setCurrentNetwork(deviceType);
                myStatus->setDialState(deviceType, DialState_Connected, DialType_None);
                if (RequestType_Access == lastRequest) {
                    NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Init);
                    popAllReq.doPopAllRequest();
                }
                else if (RequestType_RequestIp == lastRequest) {
                    notify.notifyRequestIpResult(deviceType, NetworkErrCode_Success);
                }
            }
            else {
                NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Failed");
                myStatus->setDialState(deviceType, DialState_Init, DialType_None);
                handler.clearNatTable();
                if (RequestType_Access == lastRequest) {
                    NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Failed);
                    popAllReq.doPopAllRequest();
                }
                else if (RequestType_RequestIp == lastRequest) {
                    notify.notifyRequestIpResult(deviceType, PopResultFlag_Failed);
                }
            }

            reqConfirm.confirm();
            myWorkThread->unlockReqState(RequestType_SetDefaultNetwork);
            return;
        }

        NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Dcu with meu mode");
        NC_BOOL isDevExist = handler.isCarryNetworkDevice(deviceType);
        if (NC_NOERROR == result) {
            NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Success");
            myStatus->setCurrentNetwork(deviceType);
            NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, isDevExist [%d]", isDevExist);
#if defined NCNETWORK_TARGET_PRODUCT_DCU
            if (!isDevExist) {
                myAvcUnit->setFlag(NC_FALSE);
                myStatus->setDialState(deviceType, DialState_Connected, DialType_None);
                NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Init);
                popAllReq.doPopAllRequest();
                reqConfirm.confirm();
            }
            else {
                NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Wait meu set route");
            }
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
            if (!isDevExist) {
                myAvcUnit->setFlag(NC_FALSE);
                myStatus->setDialState(deviceType, DialState_Connected, DialType_None);
                NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Init);
                popAllReq.doPopAllRequest();
                reqConfirm.confirm();
            }
            else {
                NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Wait dcu set DCM route");
                handler.clearFirewallForDcm(deviceType);
            }
#endif
        }
        else {
            NCLOGD("NCNetworkEventHandler::doAddDefaultRoute, Failed");
#if defined NCNETWORK_TARGET_PRODUCT_DCU
            myStatus->setDialState(deviceType, DialState_Init, DialType_None);
            if (!isDevExist) {
                myAvcUnit->setFlag(NC_FALSE);
                NCNetworkPopRequestMapReq popCallbackReq(RequestMapType_Callback, RequestType_Access, deviceType, NetworkErrCode_Fail);
                popCallbackReq.doPopRequest();
                NCNetworkPopRequestMapReq popInitReq(RequestMapType_Init, RequestType_Access, deviceType, NetworkErrCode_Fail);
                popInitReq.doPopRequest();
                reqConfirm.confirm();
            }
            else {
                if (WIFI_STA == deviceType && RequestType_RequestIp == lastRequest) {
                    notify.notifyRequestIpResult(deviceType, NetworkErrCode_Fail);
                    reqConfirm.confirm();
                }
                else {
                    NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Failed);
                    popAllReq.doPopAllRequest();
                    reqConfirm.confirm();
                }
            }
#endif
#if defined NCNETWORK_TARGET_PRODUCT_MEU
            myStatus->setDialState(deviceType, DialState_Init, DialType_None);
            if (!isDevExist) {
                myAvcUnit->setFlag(NC_FALSE);
                NCNetworkPopRequestMapReq popCallbackReq(RequestMapType_Callback, RequestType_Access, deviceType, NetworkErrCode_Fail);
                popCallbackReq.doPopRequest();
                NCNetworkPopRequestMapReq popInitReq(RequestMapType_Init, RequestType_Access, deviceType, NetworkErrCode_Fail);
                popInitReq.doPopRequest();
            }
            else {
                NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Failed);
                popAllReq.doPopAllRequest();
                reqConfirm.confirm();
            }
#endif
        }

        myWorkThread->unlockReqState(RequestType_SetDefaultNetwork);
    }


    VOID
    NCNetworkEventHandler::doRmDefaultRoute(const NCString& deviceName, const UINT32 result)
    {
        NCLOGD("NCNetworkEventHandler::doRmDefaultRoute - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkEventHandler::doRmDefaultRoute - Start deviceName [%s], result [%d]", deviceName.getString(), result);
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkAvcUnit* myAvcUnit = NCNetworkManager::Instance()->getNetworkAvcUnit();
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        if (NULL == myStatus || NULL == myAvcUnit || NULL == myWorkThread || NULL == myDeviceType) {
            NCLOGD("NCNetworkEventHandler::doRmDefaultRoute, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (!myWorkThread->lockReqState(RequestType_ClearDefaultNetwork)) {
            NCLOGD("NCNetworkEventHandler::doRmDefaultRoute, Timeout");
            return;
        }

        NCString deviceType;
        myDeviceType->getType(deviceName, deviceType);

        if (DCM != deviceType && BT_DUN != deviceType && WIFI_STA != deviceType) {
            NCLOGD("NCNetworkEventHandler::doRmDefaultRoute, Get deviceType failed");
            myWorkThread->unlockReqState(RequestType_ClearDefaultNetwork);
            return;
        }

        NCNetworkReqConfirm reqConfirm;
        NCString clearType = reqConfirm.getDeviceType(RequestType_ClearDefaultNetwork);
        UINT32 lastRequest = reqConfirm.getLastRequest(RequestType_ClearDefaultNetwork);
        NCLOGD("NCNetworkEventHandler::doRmDefaultRoute, Get deviceType [%s], clearType [%s], lastRequest [%d]", 
            deviceType.getString(), clearType.getString(), lastRequest);

        NCNetworkNotify notify;
        NCNetworkHandler handler;

        // Dcu only
        if (handler.checkIndividualApparatus()) {
            myStatus->setCurrentNetwork("");
            myStatus->setDialState(deviceType, DialState_Disconnected, DialType_None);


            NCNetworkPopRequestMapReq popCallbackReq(RequestMapType_Callback, RequestType_Disconnect, deviceType, NetworkErrCode_Success);
            popCallbackReq.doPopRequest();

            if (DCM == deviceType) {
                NCNetworkManager::Instance()->startWifiApTimer();
            }

            reqConfirm.confirm();
            myWorkThread->unlockReqState(RequestType_ClearDefaultNetwork);
            return;
        }

        // DCU-MEU
#if defined NCNETWORK_TARGET_PRODUCT_DCU
        myStatus->setCurrentNetwork("");
        if (DCM == deviceType) {
            NCLOGD("NCNetworkEventHandler::doRmDefaultRoute, Clear dcm route success");
            myStatus->setDialState(deviceType, DialState_Disconnected, DialType_None);
            NCNetworkPopRequestMapReq popCallbackReq(RequestMapType_Callback, RequestType_Disconnect, deviceType, NetworkErrCode_Success);
            popCallbackReq.doPopRequest();
            NCNetworkManager::Instance()->startWifiApTimer();
            reqConfirm.confirm();
        }
        else {
            NCLOGD("NCNetworkEventHandler::doRmDefaultRoute, wait meu clear route [%s]", deviceType.getString());
        }
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
        myStatus->setCurrentNetwork("");
        if (BT_DUN == deviceType) {
            myStatus->setDialState(deviceType, DialState_Disconnected, DialType_None);
            NCNetworkPopRequestMapReq popCallbackReq(RequestMapType_Callback, RequestType_Disconnect, deviceType, NetworkErrCode_Success);
            popCallbackReq.doPopRequest();
            reqConfirm.confirm();
        }
        else if (WIFI_STA == deviceType) {
            myStatus->setDialState(deviceType, DialState_Disconnected, DialType_None);
            reqConfirm.confirm();
        }
        else if (DCM == deviceType) {
            NCLOGD("NCNetworkEventHandler::doRmDefaultRoute, wait dcu clear route [%s]", deviceType.getString());
        }
#endif

        myWorkThread->unlockReqState(RequestType_ClearDefaultNetwork);
    }

    VOID
    NCNetworkEventHandler::doStartDhcpServer(const NCString& deviceName, const UINT32 result)
    {
        NCLOGD("NCNetworkEventHandler::doStartDhcpServer, Start, deviceName [%s], result [%d]", deviceName.getString(), result);
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        if (NULL == myWorkThread || NULL == myDeviceType || NULL == myStatus) {
            NCLOGD("NCNetworkEventHandler::doStartDhcpServer, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (!myWorkThread->lockReqState(RequestType_StartDhcpServer)) {
            NCLOGD("NCNetworkEventHandler::doStartDhcpServer, Timeout");
            return; 
        }

        NCString deviceType;
        myDeviceType->getType(deviceName, deviceType);
        NCLOGD("NCNetworkEventHandler::doStartDhcpServer, Device type [%s]", deviceType.getString());

        NCNetworkNotify notify;
        if (NC_NOERROR == result) {
            myStatus->setStatus(deviceType, NC_TRUE);
            myStatus->setCurrentDhcpServer(deviceType);
            notify.notifyStartDhcpServerResult(deviceType, NetworkErrCode_Success);
        }
        else {
            NCNetworkHandler handler;
            NCErrorLog(NetworkErrCode_StartDhcpServerFailed, handler.deviceTypeToNumber(deviceType));
            myStatus->setCurrentDhcpServer("");
            notify.notifyStartDhcpServerResult(deviceType, NetworkErrCode_Fail);
        }

        NCNetworkReqConfirm reqConfirm;
        reqConfirm.confirm();
        myWorkThread->unlockReqState(RequestType_StartDhcpServer);
        return;
    }

    VOID
    NCNetworkEventHandler::doStopDhcpServer(const NCString& deviceName, const UINT32 result)
    {
        NCLOGD("NCNetworkEventHandler::doStopDhcpServer, Start, deviceName [%s], result [%d]", deviceName.getString(), result);
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        NCNetworkDeviceType* myDeviceType = NCNetworkManager::Instance()->getNetworkDeviceType();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        if (NULL == myWorkThread || NULL == myDeviceType || NULL == myStatus) {
            NCLOGD("NCNetworkEventHandler::doStopDhcpServer, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (!myWorkThread->lockReqState(RequestType_StopDhcpServer)) {
            NCLOGD("NCNetworkEventHandler::doStopDhcpServer, Timeout");
            return; 
        }

        NCString deviceType;
        myDeviceType->getType(deviceName, deviceType);
        NCLOGD("NCNetworkEventHandler::doStopDhcpServer, Device type [%s]", deviceType.getString());

        NCNetworkNotify notify;
        if (NC_NOERROR == result) {
            myStatus->setStatus(deviceType, NC_FALSE);
            myStatus->setCurrentDhcpServer("");
            notify.notifyStopDhcpServerResult(deviceType, NetworkErrCode_Success);
        }
        else {
            NCNetworkHandler handler;
            NCErrorLog(NetworkErrCode_StopDhcpServerFailed, handler.deviceTypeToNumber(deviceType));
            notify.notifyStopDhcpServerResult(deviceType, NetworkErrCode_Fail);
        }

        NCNetworkReqConfirm reqConfirm;
        reqConfirm.confirm();
        myWorkThread->unlockReqState(RequestType_StopDhcpServer);
    }

    VOID
    NCNetworkEventHandler::doRestartDhcpServer(const NCString& deviceName, const UINT32 result)
    {
        NCLOGD("NCNetworkEventHandler::doRestartDhcpServer, Start, deviceName [%s], result [%d]", deviceName.getString(), result);
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        if (NULL == myWorkThread) {
            NCLOGD("NCNetworkEventHandler::doRestartDhcpServer, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (!myWorkThread->lockReqState(RequestType_RestartDhcpServer)) {
            NCLOGD("NCNetworkEventHandler::doRestartDhcpServer, Timeout");
            return; 
        }

        NCNetworkReqConfirm reqConfirm;
        reqConfirm.confirm();
        myWorkThread->unlockReqState(RequestType_RestartDhcpServer);
    }

    VOID
    NCNetworkEventHandler::doAddRoute(const NCString& deviceName, const UINT32 result)
    {
        NCLOGD("NCNetworkEventHandler::doAddRoute, Start, deviceName [%s], result [%d]", deviceName.getString(), result);
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        if (NULL == myWorkThread || NULL == myStatus) {
            NCLOGD("NCNetworkEventHandler::doAddRoute, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (!myWorkThread->lockReqState(RequestType_AddRouteVRServer)) {
            NCLOGD("NCNetworkEventHandler::doRestartDhcpServer, Timeout");
            return; 
        }

        NCLOGD("NCNetworkEventHandler::doAddRoute, Add VRServer route success");
        myStatus->setStatus(VR_SERVER, NC_TRUE);
        NCNetworkReqConfirm reqConfirm;
        reqConfirm.confirm();
        myWorkThread->unlockReqState(RequestType_AddRouteVRServer);
    }

} /* namespace nutshell */
/* EOF */
