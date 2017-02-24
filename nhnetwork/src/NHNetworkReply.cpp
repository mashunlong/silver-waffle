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
#include <nceventsys/NCEvent.h>
#include <nceventsys/NCEvSysContext.h>
#include <nhinterface/NINetworkHandler.h>
#include <nhnetwork/NHNetworkHandler.h>
#include "NDNetworkEvent_generated.h"
#include "NHNetworkReply.h"

#define NC_LOG_TAG "NHNetworkHandler"

namespace nutshell
{
    NHNetworkReply::NHNetworkReply()
    {

    }

    NHNetworkReply::~NHNetworkReply()
    {

    }

    VOID
    NHNetworkReply::notifyDoDhcpResult(const NCString& device, const DhclientInfo& info, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyDoDhcpResult - device [%s], gateway [%s], dns1 [%s], dns2 [%s], result = [%d]", device.getString(), 
            info.gateway.getString(), info.dns1.getString(), info.dns2.getString(), result);
        NCEvSysContext evSysContext; 
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_DoDhcp;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyDoDhcpResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkDoDhcpBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        auto autoGateway = fbb.CreateString((char*)info.gateway.getString());
        auto autoDns1 = fbb.CreateString((char*)info.dns1.getString());
        auto autoDns2 = fbb.CreateString((char*)info.dns2.getString());
        mb.add_name(name);
        mb.add_gateway(autoGateway);
        mb.add_dns1(autoDns1);
        mb.add_dns2(autoDns2);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifyStopDhcpResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyStopDhcpResult - device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_StopDhcp;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyStopDhcpResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkStopDhcpBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifyReleaseDhcpLeaseResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyReleaseDhcpLeaseResult - device = [%s], reuslt = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_ReleaseDhcpLease;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyReleaseDhcpLeaseResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkReleaseDhcpLeaseBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifySetStaticIpResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifySetStaticIpResult  - Start, device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_SetStaticIp;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifySetStaticIpResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkSetStaticIpBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifySetMaskResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifySetMaskResult  - Start, device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_SetMask;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifySetMaskResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkSetMaskBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifySetMtuResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifySetMtuResult - Start, device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_SetMtu;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifySetMtuResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkSetMtuBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifySetStaticIpMaskMtuResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifySetStaticIpMaskMtuResult  - Start, device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_SetStaticIpMaskMtu;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifySetStaticIpMaskMtuResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkSetStaticIpMaskMtuBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifyAddDefaultRouteResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyAddDefaultRouteResult  - Start, device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_AddDefaultRoute;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyAddDefaultRouteResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkAddDefaultRouteBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifyRemoveDefaultRouteResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyRemoveDefaultRouteResult  - Start, device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_RemoveDefaultRoute;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyRemoveDefaultRouteResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkRemoveDefaultRouteBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifyAddRouteResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyAddRouteResult  - Start, device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_AddRoute;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyAddRouteResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkAddRouteBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifyRemoveRouteResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyRemoveRouteResult - Start");
    }

    VOID
    NHNetworkReply::notifySetFilterTableDefaultRuleResult(UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifySetFilterTableDefaultRuleResult - Start");
    }

    VOID
    NHNetworkReply::notifyAddFilterTableRuleResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyAddFilterTableRuleResult - Start");
    }

    VOID
    NHNetworkReply::notifyRemoveFilterTableRuleResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyRemoveFilterTableRuleResult - Start");
    }

    VOID
    NHNetworkReply::notifySetNatTableRuleResult(UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifySetNatTableRuleResult - Start");
    }

    VOID
    NHNetworkReply::notifyNetworkDeviceInfoResult(const NHNetworkDeviceInfo& info, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyNetworkDeviceInfoResult - result[%d], device[%s], ip[%s], mac[%s], prefixLength[%d],", 
            result, info.device.getString(), info.ip.getString(), info.mac.getString(), info.prefixLength);
        NCLOGD("NHNetworkReply::notifyNetworkDeviceInfoResult - gateway[%s], dns1 [%s], dns2 [%s], serverId[%s]", 
            info.gateway.getString(), info.dns1.getString(), info.dns2.getString(), info.serverId.getString());

        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_NetworkDeviceInfo;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyNetworkDeviceInfoResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkDeviceBuilder mb(fbb);
        mb.add_result(result);
        mb.add_prefixLength(info.prefixLength);
        auto name = fbb.CreateString((char*)info.device.getString());
        auto ip = fbb.CreateString((char*)info.ip.getString());
        auto mac = fbb.CreateString((char*)info.mac.getString());
        auto gateway = fbb.CreateString((char*)info.gateway.getString());
        auto dns1 = fbb.CreateString((char*)info.dns1.getString());
        auto dns2 = fbb.CreateString((char*)info.dns2.getString());
        auto serverId = fbb.CreateString((char*)info.serverId.getString());
        mb.add_name(name);
        mb.add_ip(ip);
        mb.add_mac(mac);
        mb.add_gateway(gateway);
        mb.add_dns1(dns1);
        mb.add_dns2(dns2);
        mb.add_serverId(serverId);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifyConnectedClientInfoResult(const NHNetworkConnectedClientInfo& info, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyConnectedClientInfoResult - result[%d], mac[%s], ip[%s], hostName[%s]",
               result, info.mac.getString(), info.ip.getString(), info.hostName.getString());

        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_ConnectedClientInfo;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyConnectedClientInfoResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkClientBuilder mb(fbb);
        mb.add_result(result);
        auto mac = fbb.CreateString((char*)info.mac.getString());
        auto ip = fbb.CreateString((char*)info.ip.getString());
        auto hostName = fbb.CreateString((char*)info.hostName.getString());
        mb.add_mac(mac);
        mb.add_ip(ip);
        mb.add_hostName(hostName);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }


    VOID
    NHNetworkReply::notifyStartDhcpServerResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyStartDhcpServerResult - device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_StartDhcpServer;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyStartDhcpServerResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkStartDhcpServerBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifyStopDhcpServerResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyStopDhcpServerResult - device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_StopDhcpServer;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyStopDhcpServerResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkStopDhcpServerBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

    VOID
    NHNetworkReply::notifyRestartDhcpServerResult(const NCString& device, UINT32 result)
    {
        NCLOGD("NHNetworkReply::notifyRestartDhcpServerResult - device = [%s], result = [%d]", device.getString(), result);
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_RestartDhcpServer;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);
        if (NULL == event) {
            NCLOGD("NHNetworkReply::notifyRestartDhcpServerResult - Obtain event failed!");
            return;
        }

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkStartDhcpServerBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());

        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());
        NHNetworkHandler::getNetworkHandler()->sendEvent(event);
        evSysContext.releaseEvent(event);
    }

} /* namespace nutshell */
/* EOF */
