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
#include <system/PfServiceDefine.h>
#include <evip/NEIpcServiceHelper.h>
#include <NDNetworkManager.pb.h>
#include "NCNetworkManagerDef.h"
#include "networkmanager/NCNetworkAccessReplier.h"
#include "networkmanager/NCNetworkAccess.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{
    NCNetworkAccess::NCNetworkAccess()
    : NCBinderServiceProxy(PFSERVICE_NETWORKMANAGER_NAME)
    , NEServiceProxy(NULL)
    {

    }

    NCNetworkAccess::~NCNetworkAccess()
    {

    }

    INT32
    NCNetworkAccess::request(const NCString& resourceId, const NCString& deviceType, const AccessMode& mode)
    {
        NCLOGD("NCNetworkAccess::request - Start, resourceId[%s], deviceType[%s], dialType[%d], communicateType[%d]",
               resourceId.getString(), deviceType.getString(), mode.type, mode.priority);
        NCLOGD("NCNetworkAccess::request, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCParcel data;
        data.writeCString(resourceId.getString());
        data.writeCString(deviceType.getString());
        data.writeInt32(mode.type);
        data.writeInt32(mode.priority);
        INT32 ret = sendCommand(FunctionId_Request, data, NULL);
        return ret;
    }

    INT32
    NCNetworkAccess::remoteRequest(const NCString& resourceId, const NCString& deviceType, const AccessMode& mode)
    {
        NCLOGD("NCNetworkAccess::remoteRequest - Start, resourceId[%s], deviceType[%s], dialType[%d], communicateType[%d]",
               resourceId.getString(), deviceType.getString(), mode.type, mode.priority);
        NCLOGD("NCNetworkAccess::remoteRequest, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCParcel data;
        data.writeCString(resourceId.getString());
        data.writeCString(deviceType.getString());
        data.writeInt32(mode.type);
        data.writeInt32(mode.priority);
        INT32 ret = sendCommand(FunctionId_RemoteRequest, data, NULL);
        return ret;
    }

    INT32
    NCNetworkAccess::requestWithSequenceId(const NCString& deviceType, const AccessMode& mode, UINT32& seqId)
    {
        NCLOGD("NCNetworkAccess::requestWithSequenceId, deviceType [%s], resourceId [%s], dialType [%d], commPriority [%d]", 
            deviceType.getString(), mode.resourceId.getString(), mode.type, mode.priority);
        NCParcel data;
        data.writeCString(deviceType.getString());
        data.writeCString(mode.resourceId.getString());
        data.writeInt32(mode.type);
        data.writeInt32(mode.priority);

        NCParcel reply;
        INT32 ret = sendCommand(FunctionId_RequestWithSequenceId, data, &reply);
        seqId = reply.readInt32();
        NCLOGD("NCNetworkAccess::requestWithSequenceId, ret [%d], seqId [%d]", ret, seqId);
        return ret;
    }


    INT32
    NCNetworkAccess::release(const NCString& resourceId)
    {
        NCLOGD("NCNetworkAccess::release - Start, resourceId = [%s]", resourceId.getString());
        NCParcel data;
        data.writeCString(resourceId.getString());
        INT32 ret = sendCommand(FunctionId_Release, data, NULL);
        return ret;
    }

    INT32
    NCNetworkAccess::releaseWithSequenceId(const NCString& resourceId, UINT32& seqId)
    {
        NCLOGD("NCNetworkAccess::releaseWithSequenceId - Start, resourceId = [%s]", resourceId.getString());
        NCParcel data;
        data.writeCString(resourceId.getString());

        NCParcel reply;
        INT32 ret = sendCommand(FunctionId_ReleaseWithSequenceId, data, &reply);
        seqId = reply.readInt32();
        NCLOGD("NCNetworkAccess::releaseWithSequenceId, ret [%d], seqId [%d]", ret, seqId);
        return ret;
    }

    INT32
    NCNetworkAccess::disconnect()
    {
        NCLOGD("NCNetworkAccess::disconnect - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCParcel data;
        INT32 ret = sendCommand(FunctionId_Disconnect, data, NULL);
        return ret;
    }

    INT32
    NCNetworkAccess::disconnect(const NCString& deviceType)
    {
        NCLOGD("NCNetworkAccess::disconnect - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkAccess::disconnect, deviceType [%s]", deviceType.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());
        INT32 ret = sendCommand(FunctionId_DisconnectNetwork, data, NULL);
        return ret;
    }

    INT32
    NCNetworkAccess::disconnectWithSequenceId(const NCString& deviceType, UINT32& seqId)
    {
        NCLOGD("NCNetworkAccess::disconnectWithSequenceId, deviceType [%s]", deviceType.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());

        NCParcel reply;
        INT32 ret = sendCommand(FunctionId_DisconnectWithSequenceId, data, &reply);
        seqId = reply.readInt32();
        NCLOGD("NCNetworkAccess::disconnectWithSequenceId, ret [%d], seqId [%d]", ret, seqId);
        return ret;
    }

    INT32
    NCNetworkAccess::remoteDisconnect(const NCString& deviceType)
    {
        NCLOGD("NCNetworkAccess::remoteDisconnect - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkAccess::remoteDisconnect, deviceType [%s]", deviceType.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());
        INT32 ret = sendCommand(FunctionId_RemoteDisconnect, data, NULL);
        return ret;
    }

    INT32
    NCNetworkAccess::connect()
    {
        return 0;
    }

    INT32
    NCNetworkAccess::setSlipDevice(const struct SlipDeviceInfo& slipInfo)
    {
        NCLOGD("NCNetworkAccess::setSlipDevice - Start, deviceNmae[%s], deviceType[%s], ip[%s], mask[%d], mtu[%d]",
            slipInfo.deviceName.getString(), slipInfo.deviceType.getString(), slipInfo.ip.getString(), slipInfo.mask, slipInfo.mtu);
        NCParcel data;
        data.writeCString(slipInfo.deviceName.getString());
        data.writeCString(slipInfo.deviceType.getString());
        data.writeCString(slipInfo.ip.getString());
        data.writeInt32(slipInfo.mask);
        data.writeInt32(slipInfo.mtu);
        INT32 ret = sendCommand(FunctionId_SetSlipDevice, data, NULL);
        return ret;
    }

    INT32
    NCNetworkAccess::notifySlipDisconnect(const NCString& deviceName)
    {
        NCLOGD("NCNetworkAccess::notifySlipDisconnect - Start, deviceName= [%s]", deviceName.getString());
        NCParcel data;
        data.writeCString(deviceName.getString());
        INT32 ret = sendCommand(FunctionId_NotifySlipDisconnect, data, NULL);
        return ret;
    }

    INT32
    NCNetworkAccess::notifyRequestAccessResult(const struct RequestAccessResult& accessInfo)
    {
        return 0;
    }

    INT32
    NCNetworkAccess::notifyDisconnectResult(const NCString& deviceType, const UINT32 result)
    {
        NCLOGD("NCNetworkAccess::notifyDisconnectResult This interface will be deleted later");
        NCParcel data;
        data.writeCString(deviceType.getString());
        INT32 ret = sendCommand(FunctionId_NotifyDisconnectResult, data, NULL);
        return ret;
    }

    INT32
    NCNetworkAccess::notifyDisconnectResult(const NCString& deviceType)
    {
        NCLOGD("NCNetworkAccess::notifyDisconnectResult Start, deviceType [%s]", deviceType.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());
        INT32 ret = sendCommand(FunctionId_NotifyDisconnectResult, data, NULL);
        return ret;
    }

    INT32
    NCNetworkAccess::updateGbookDns(const NCString& dns1, const NCString& dns2)
    {
        return 0;
    }

    INT32
    NCNetworkAccess::updateGbookWifiDns(const NCString& dns1, const NCString& dns2)
    {
        return 0;
    }

    INT32
    NCNetworkAccess::updateGbookServerDns(const GbookServerDnsInfo& info)
    {
        NCLOGD("NCNetworkAccess::updateGbookServerDns, Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkAccess::updateGbookServerDns, dcmDns1 [%s], dcmDns2 [%s], wifiDns1 [%s], wifiDns2 [%s]", info.dcmDns1.getString(), 
            info.dcmDns2.getString(), info.wifiDns1.getString(), info.wifiDns2.getString());
        NCParcel data;
        data.writeCString(info.dcmDns1.getString());
        data.writeCString(info.dcmDns2.getString());
        data.writeCString(info.wifiDns1.getString());
        data.writeCString(info.wifiDns2.getString());
        INT32 ret = sendCommand(FunctionId_UpdateGbookServerDns, data, NULL);
        return ret;
    }

    NC_BOOL
    NCNetworkAccess::bindService()
    {
        NCLOGD("NCNetworkAccess::bindService - Start");
        ncsp<NEIpcServiceProxy>::sp service =
        NEIpcServiceHelper::findService(NCString(PFSERVICE_NETWORKMANAGER_NAME), 0);

        NC_BOOL ret = NEServiceProxy::bindService(service);
        if (!ret) {
            return ret;
        }
        registerNotify(MethodId_Request);
        registerNotify(MethodId_Release);
        registerNotify(MethodId_Disconnect);
        return NC_TRUE;
    }

    VOID
    NCNetworkAccess::unbindService()
    {
        NCLOGD("NCNetworkAccess::unbindService - Start");
        NEServiceProxy::unbindService();
        unregisterNotify(MethodId_Request);
        unregisterNotify(MethodId_Release);
        unregisterNotify(MethodId_Disconnect);
    }

    VOID
    NCNetworkAccess::onReceiveMessage(NEMessage& message)
    {
        NCLOGD("NCNetworkAccess::onReceiveMessage - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        switch (message.methodId()) {
        case MethodId_Request:
        {
            std::string resourceId;
            UINT32 result;
            UINT32 seqId = 0;
            nutshell::networkmanager::NDNetworkAccessResult in;
            if (!in.ParseFromString(message.param())) {
                NCLOGD("NCNetworkAccess::onReceiveMessage - MethodId_Request, Parse from string error");
            }
            else {
                if (in.has_resourceid()) {
                    resourceId = in.resourceid();
                }
                if (in.has_result()) {
                    result = in.result();
                }
                if (in.has_seqid()) {
                    seqId = in.seqid();
                }
             }

            NCLOGD("NCNetworkAccess::onReceiveMessage - MethodId_Request, resourceId [%s], result [%d], seqId [%d]", 
                resourceId.c_str(), result, seqId);
            NCString id(resourceId.c_str());
            NCNetworkAccessReplier* replier = dynamic_cast<NCNetworkAccessReplier*>(m_replyReceiver.get());
            if (replier) {
                if (0 == seqId) {
                    replier->replyRequest(id, result);
                }
                else {
                    replier->replyRequestWithSequenceId(seqId, id, result);
                }
            }
        }
        break;
        case MethodId_Release:
        {
            std::string resourceId;
            UINT32 result;
            UINT32 seqId = 0;
            nutshell::networkmanager::NDNetworkAccessResult in;
            if (!in.ParseFromString(message.param())) {
                NCLOGD("NCNetworkAccess::onReceiveMessage - MethodId_Release, Parse from string error");
            }
            else {
                if (in.has_resourceid()) {
                    resourceId = in.resourceid();
                }
                if (in.has_result()) {
                    result = in.result();
                }
                if (in.has_seqid()) {
                    seqId = in.seqid();
                }
            }

            NCLOGD("NCNetworkAccess::onReceiveMessage - MethodId_Release, resourceId [%s], result [%d], seqId [%d]", 
                resourceId.c_str(), result, seqId);
            NCString id(resourceId.c_str());
            NCNetworkAccessReplier* replier = dynamic_cast<NCNetworkAccessReplier*>(m_replyReceiver.get());
            if (replier) {
                if (0 == seqId) {
                    replier->replyRelease(id, result);
                }
                else {
                    replier->replyReleaseWithSequenceId(seqId, id, result);
                }
            }
        }
        break;
        case MethodId_Disconnect:
        {
            std::string device;
            UINT32 result;
            UINT32 seqId;
            nutshell::networkmanager::NDNetworkDisconnectResult in;
            if (!in.ParseFromString(message.param())) {
                NCLOGD("NCNetworkAccess::onReceiveMessage - MethodId_Disconnect, Parse from string error");
            }
            else {
                device = in.device();
                result = in.result();
                if (in.has_seqid()) {
                    seqId = in.seqid();
                }
            }

            NCLOGD("NCNetworkAccess::onReceiveMessage - MethodId_Disconnect, device [%s], result [%d], seqId [%d]", 
                device.c_str(), result, seqId);
            NCString deviceType(device.c_str());
            NCNetworkAccessReplier* replier = dynamic_cast<NCNetworkAccessReplier*>(m_replyReceiver.get());
            if (replier) {
                if (0 == seqId) {
                    replier->replyDisconnect(deviceType, result);
                }
                else {
                    replier->replyDisconnectWithSeqId(seqId, deviceType, result);
                }
                
            }
            else {
                NCLOGD("NCNetworkAccess::onReceiveMessage - MethodId_Disconnect, Get reply receiver failed");
            }
        }
        break;
        default:
            break;
        }
     }

} /* namespace nutshell */
/* EOF */
