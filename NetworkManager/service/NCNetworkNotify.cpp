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
#include <NDNetworkManager.pb.h>
#include <system/PfServiceDefine.h>
#include <nceventsys/NCEvSysContext.h>
#include <nhinterface/NINetworkDefine.h>
#include "networkmanager/NCNetworkDef.h"
#include "NCNetworkManagerDef.h"
#include "NCNetworkManager.h"
#include "NCNetworkDeviceType.h"
#include "NCNetworkStatus.h"
#include "NCNetworkAvcUnit.h"
#include "NCNetworkHandler.h"
#include "NCNetworkReqConfirm.h"
#include "NCNetworkPluginManager.h"
#include "NCNetworkNotify.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{

    NCNetworkNotify::NCNetworkNotify()
    {

    }

    NCNetworkNotify::~NCNetworkNotify()
    {

    }

    VOID
    NCNetworkNotify::notifyRequestIpResult(const NCString& deviceType, UINT32 result)
    {
        NCLOGD("NCNetworkNotify::notifyRequestIpResult - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkNotify::notifyRequestIpResult - Start, deviceType [%s], result [%d]", deviceType.getString(), result);
        nutshell::networkmanager::NDNetworkDhcpResult message;
        message.set_device(std::string(deviceType.getString()));
        message.set_result(result);
        std::string param("");
        message.SerializeToString(&param);
        notifyResult(param, NEMessage::TYPE_NOTIFY, MethodId_RequestIp);
    }

    VOID
    NCNetworkNotify::notifyReleaseIpResult(const NCString& deviceType, UINT32 result)
    {
        NCLOGD("NCNetworkNotify::notifyReleaseIpResult - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkNotify::notifyReleaseIpResult - Start, deviceType [%s], reuslt [%d]", deviceType.getString(), result);
        nutshell::networkmanager::NDNetworkDhcpResult message;
        message.set_device(std::string(deviceType.getString()));
        message.set_result(result);
        std::string param("");
        message.SerializeToString(&param);
        notifyResult(param, NEMessage::TYPE_NOTIFY, MethodId_ReleaseIp);
    }

    VOID
    NCNetworkNotify::notifyStartDhcpServerResult(const NCString& deviceType, UINT32 result)
    {
        NCLOGD("NCNetworkNotify::notifyStartDhcpServerResult - Start, deviceType [%s], result [%d]", deviceType.getString(), result);
        nutshell::networkmanager::NDNetworkDhcpResult message;
        message.set_device(std::string(deviceType.getString()));
        message.set_result(result);
        std::string param("");
        message.SerializeToString(&param);
        notifyResult(param, NEMessage::TYPE_NOTIFY, MethodId_StartDhcpServer);
    }

    VOID
    NCNetworkNotify::notifyStopDhcpServerResult(const NCString& deviceType, UINT32 result)
    {
        NCLOGD("NCNetworkNotify::notifyStopDhcpServerResult - Start, deviceType [%s], result [%d]", deviceType.getString(), result);
        result = parseResult(result);
        nutshell::networkmanager::NDNetworkDhcpResult message;
        message.set_device(std::string(deviceType.getString()));
        message.set_result(result);
        std::string param("");
        message.SerializeToString(&param);
        notifyResult(param, NEMessage::TYPE_NOTIFY, MethodId_StopDhcpServer);
    }

    VOID
    NCNetworkNotify::notifyRequestAccessResult(const NCString& resourceId, const UINT32 result, const NC_BOOL wifiApFlag, const UINT32 seqId)
    {
        NCLOGD("NCNetworkNotify::notifyRequestAccessResult - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkNotify::notifyRequestAccessResult, resourceId [%s], result [%d], wifiApFlag [%d], seqId [%d]", resourceId.getString(), 
            result, wifiApFlag, seqId);
        if (wifiApFlag) {
            NCLOGD("NCNetworkNotify::notifyRequestAccessResult, WIFI AP Flag is true");
            NCNetworkManager::Instance()->setWifiApFlag(WifiApTimerFlag_OnTimer, NC_FALSE);
            return;
        }

        nutshell::networkmanager::NDNetworkAccessResult message;
        message.set_resourceid(std::string(resourceId.getString()));
        message.set_result(result);
        message.set_seqid(seqId);
        std::string param("");
        message.SerializeToString(&param);
        notifyResult(param, NEMessage::TYPE_NOTIFY, MethodId_Request);
    }

    VOID
    NCNetworkNotify::notifyReleaseAccessResult(const NCString& resourceId, const UINT32 result, const UINT32 seqId)
    {
        NCLOGD("NCNetworkNotify::notifyReleaseAccessResult - Start");
        NCLOGD("NCNetworkNotify::notifyReleaseAccessResult, resourceId [%s], result [%d], seqId [%d]", resourceId.getString(), result, seqId);
        nutshell::networkmanager::NDNetworkAccessResult message;
        message.set_resourceid(std::string(resourceId.getString()));
        message.set_result(result);
        message.set_seqid(seqId);

        std::string param("");
        message.SerializeToString(&param);
        notifyResult(param, NEMessage::TYPE_NOTIFY, MethodId_Release);
    }

    VOID
    NCNetworkNotify::notifyDisconnectResult(const NCString& deviceType, const UINT32 result, const UINT32 seqId)
    {
        NCLOGD("NCNetworkNotify::notifyDisconnectResult, Start, deviceType [%s], result [%d], seqId [%d]", deviceType.getString(), result, seqId);
        nutshell::networkmanager::NDNetworkDisconnectResult message;
        message.set_device(std::string(deviceType.getString()));
        message.set_result(result);
        message.set_seqid(seqId);

        std::string param("");
        message.SerializeToString(&param);
        notifyResult(param, NEMessage::TYPE_NOTIFY, MethodId_Disconnect);
    }

    VOID
    NCNetworkNotify::notifyDeviceInfo(const NCString& deviceType, const NCNetworkDataReplier::NetworkDeviceInfo& info)
    {
        NCLOGD("NCNetworkNotify::notifyDeviceInfo - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkNotify::notifyDeviceInfo, deviceType [%s], ip [%s], mask [%d], gateway [%s], serverId [%s]", 
            deviceType.getString(), info.ip.getString(), info.prefixLength, info.gateway.getString(), info.serverId.getString());
        nutshell::networkmanager::NDNetworkDevice message;
        message.set_name(std::string(deviceType.getString()));
        message.set_ip(std::string(info.ip.getString()));
        message.set_mac(std::string(info.mac.getString()));
        message.set_prefixlength(info.prefixLength);
        message.set_gateway(std::string(info.gateway.getString()));
        message.set_serverid(std::string(info.serverId.getString()));

        std::string param("");
        message.SerializeToString(&param);
        notifyResult(param, NEMessage::TYPE_NOTIFY, MethodId_DeviceInfo);
    }

    VOID
    NCNetworkNotify::notifyConnectedClientInfo(const NCString& mac, const NCNetworkDataReplier::ConnectedClientInfo& info)
    {
        NCLOGD("NCNetworkNotify::notifyConnectedClientInfo - Start");
        nutshell::networkmanager::NDNetworkClient message;
        message.set_mac(std::string(mac.getString()));
        message.set_ip(std::string(info.ip.getString()));
        message.set_name(std::string(info.name.getString()));

        std::string param("");
        message.SerializeToString(&param);
        notifyResult(param, NEMessage::TYPE_NOTIFY, MethodId_ConnectedClientInfo);
    }

    VOID
    NCNetworkNotify::notifyDialState(const NCString& deviceType, const UINT32 state, const UINT32 type)
    {
        NCLOGD("NCNetworkNotify::notifyDialState, Start, deviceType [%s], state [%d], type [%d]", deviceType.getString(), state, type);
        nutshell::networkmanager::NDNetworkDialState proto;
        proto.set_name(std::string(deviceType.getString()));
        proto.set_state(state);
        proto.set_type(type);

        std::string strExtraData;
        strExtraData = proto.SerializeAsString();

        NCEvSysContext context;
        NCEvent* event = context.obtainEvent(PFSERVICE_NETWORKMANAGER_NAME, NCNETWORK_EVENT_DIAL_STATE);
        if (NULL == event) {
            NCLOGD("NCNetworkNotify::notifyDialState, obtain event failed");
            return;
        }

        NCEvMessageIF* eventSender = context.queryEventSender();
        if (NULL == eventSender) {
            NCLOGD("NCNetworkManager::notifyDialState - Query event sender failed");
            context.releaseEvent(event);
            return;
        }

        event->setExtraData((void*)strExtraData.data(), strExtraData.size());
        if (!eventSender->sendEvent(event)) {
            NCLOGD("NCNetworkNotify::notifyDialState, send event failed");
        }

        context.releaseEvent(event);
    }

    VOID
    NCNetworkNotify::notifyLinkState(const NCString& deviceType, const UINT32 dialState, const UINT32 dialType)
    {
        NCLOGD("NCNetworkNotify::notifyLinkState, Start, deviceType [%s], dialState [%d], dialType [%d]", deviceType.getString(), dialState, dialType);
        nutshell::networkmanager::NDNetworkDialState proto;
        proto.set_name(std::string(deviceType.getString()));
        proto.set_state(dialState);
        proto.set_type(dialType);

        std::string strExtraData;
        strExtraData = proto.SerializeAsString();

        NCEvSysContext context;
        NCEvent* event = context.obtainEvent(PFSERVICE_NETWORKMANAGER_NAME, NCNETWORK_EVENT_LINK_STATE);
        if (NULL == event) {
            NCLOGD("NCNetworkNotify::notifyLinkState, Obtain event failed");
            return;
        }

        NCEvMessageIF* eventSender = context.queryEventSender();
        if (NULL == eventSender) {
            NCLOGD("NCNetworkManager::notifyLinkState - Query event sender failed");
            context.releaseEvent(event);
            return;
        }

        event->setExtraData((void*)strExtraData.data(), strExtraData.size());
        if (!eventSender->sendEvent(event)) {
            NCLOGD("NCNetworkNotify::notifyLinkState, Send event failed");
        }

        context.releaseEvent(event);
    }

    VOID
    NCNetworkNotify::notifyStartFirewallFailed()
    {
        NCLOGD("NCNetworkNotify::notifyStartFirewallFailed, Start");
        std::string strExtraData;

        NCEvSysContext context;
        NCEvent* event = context.obtainEvent(PFSERVICE_NETWORKMANAGER_NAME, NCNETWORK_EVENT_FIREWALL_FAILED);
        if (NULL == event) {
            NCLOGD("NCNetworkNotify::notifyStartFirewallFailed, obtain event failed");
            return;
        }

        NCEvMessageIF* eventSender = context.queryEventSender();
        if (NULL == eventSender) {
            NCLOGD("NCNetworkManager::notifyStartFirewallFailed - Query event sender failed");
            context.releaseEvent(event);
            return;
        }

        event->setExtraData((void*)strExtraData.data(), strExtraData.size());
        if (!eventSender->sendEvent(event)) {
            NCLOGD("NCNetworkNotify::notifyStartFirewallFailed, send event failed");
        }

        context.releaseEvent(event);
    }

    VOID
    NCNetworkNotify::broadcastNetworkStatus(const NCString& identity, const NCString& deviceType)
    {
        NCLOGD("NCNetworkStatus::broadcastNetworkStatus - Start, identity[%s], deviceType[%s]", identity.getString(), deviceType.getString());
        nutshell::networkmanager::NDNetworkStatus proto;
        proto.set_device(std::string(deviceType.getString()));

        std::string strExtraData;
        strExtraData = proto.SerializeAsString();

        NCEvSysContext context;
        NCEvent* event = context.obtainEvent(PFSERVICE_NETWORKMANAGER_NAME, identity);
        if (NULL == event) {
            NCLOGD("NCNetworkNotify::broadcastNetworkStatus, obtain event failed");
            return;
        }

        NCEvMessageIF* eventSender = context.queryEventSender();
        if (NULL == eventSender) {
            NCLOGD("NCNetworkManager::broadcastNetworkStatus - Query event sender failed");
            context.releaseEvent(event);
            return;
        }

        event->setExtraData((void*)strExtraData.data(), strExtraData.size());
        if (!eventSender->sendEvent(event)) {
            NCLOGD("NCNetworkNotify::broadcastNetworkStatus, send event failed");
        }

        context.releaseEvent(event);
    }

    VOID
    NCNetworkNotify::notifyNcmIpFinished()
    {
        NCLOGD("NCNetworkNotify::notifyNcmIpFinished, Start");
        std::string strExtraData;
        NCEvSysContext context;
        NCEvent* event = context.obtainEvent(PFSERVICE_NETWORKMANAGER_NAME, NCNETWORK_EVENT_NCM_IP_FINISHED);
        if (NULL == event) {
            NCLOGD("NCNetworkNotify::notifyNcmIpFinished, obtain event failed");
            return;
        }

        event->setSticky(NC_TRUE);
        
        NCEvMessageIF* eventSender = context.queryEventSender();
        if (NULL == eventSender) {
            NCLOGD("NCNetworkManager::notifyNcmIpFinished - Query event sender failed");
            context.releaseEvent(event);
            return;
        }

        event->setExtraData((void*)strExtraData.data(), strExtraData.size());
        if (!eventSender->sendEvent(event)) {
            NCLOGD("NCNetworkNotify::notifyNcmIpFinished, send event failed");
        }

        context.releaseEvent(event);
    }

    UINT32
    NCNetworkNotify::parseResult(const UINT32 result)
    {
        if (NI_ERROR_HAL_NETWORK_SUCCESS == result) {
            return NetworkErrCode_Success;
        }
        else if (NI_ERROR_HAL_NETWORK_DHCPSERVER_EXIST == result) {
            return NetworkErrCode_DhcpServer_Exist;
        }
        else {
            return NetworkErrCode_Fail;
        }
    }

    VOID
    NCNetworkNotify::notifyResult(std::string param, UINT32 type, UINT32 methodId)
    {
        NEMessage msg;
        msg.setType(type);
        msg.setMethodId(methodId);
        msg.setParam(param);

        if (NCNetworkManager::Instance() != NULL) {
            NCNetworkManager::Instance()->notifyChanged(msg);
        }
        else {
            NCLOGD("NCNetworkNotify::notifyRequestIpResult - NCNetworkManager::Instance is NULL");
        }
    }

} /* namespace nutshell */
/* EOF */
