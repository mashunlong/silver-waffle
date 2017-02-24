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
#include "networkmanager/NCNetworkDataReplier.h"
#include "networkmanager/NCNetworkData.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{

    NCNetworkData::NCNetworkData()
    : NCBinderServiceProxy(PFSERVICE_NETWORKMANAGER_NAME)
    , NEServiceProxy(NULL)
    {

    }

    NCNetworkData::~NCNetworkData()
    {

    }

    INT32
    NCNetworkData::deviceInfo(const NCString& deviceType)
    {
        NCLOGD("NCNetworkData::deviceInfo - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkData::deviceInfo - Start, device = [%s]", deviceType.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());
        INT32 ret = sendCommand(FunctionId_DeviceInfo, data, NULL);
        return ret;
    }

    INT32
    NCNetworkData::connectedClientInfo(const NCString& mac)
    {
        NCLOGD("NCNetworkData::connectedClientInfo - Start, mac = [%s]", mac.getString());
        NCParcel data;
        data.writeCString(mac.getString());
        INT32 ret = sendCommand(FunctionId_ConnectedClientInfo, data, NULL);
        return ret;
    }

    NCString
    NCNetworkData::currentUseNetwork()
    {
        NCLOGD("NCNetworkData::currentUseNetwork - Start");
        NCParcel data;
        NCParcel reply;
        INT32 ret = sendCommand(FunctionId_CurrentUseNetwork, data, &reply);
        NCString network;
        network.set(reply.readCString());
        return network;
    }

    NC_BOOL
    NCNetworkData::getDeviceStatus(const NCString& deviceType)
    {
        NCLOGD("NCNetworkData::getDeviceStatus - Start");
        NCParcel data;
        NCParcel reply;
        data.writeCString(deviceType.getString());
        INT32 ret = sendCommand(FunctionId_DeviceStatus, data, &reply);
        NC_BOOL status = reply.readInt32();
        return status;
    }

    NC_BOOL
    NCNetworkData::networkSignal(const NCString& deviceType)
    {
        NCLOGD("NCNetworkData::networkSignal - Start");
        NCParcel data;
        NCParcel reply;
        data.writeCString(deviceType.getString());
        INT32 ret = sendCommand(FunctionId_NetworkSignal, data, &reply);
        NC_BOOL status;
        status = reply.readInt32();
        return status;
    }

    NC_BOOL
    NCNetworkData::bindService()
    {
        NCLOGD("NCNetworkData::bindService - Start");
        ncsp<NEIpcServiceProxy>::sp service =
        NEIpcServiceHelper::findService(NCString(PFSERVICE_NETWORKMANAGER_NAME), 0);

        NC_BOOL ret = NEServiceProxy::bindService(service);
        if (!ret) {
            return ret;
        }
        registerNotify(MethodId_DeviceInfo);
        registerNotify(MethodId_ConnectedClientInfo);
        return NC_TRUE;
    }

    VOID
    NCNetworkData::unbindService()
    {
        NCLOGD("NCNetworkData::unbindService - Start");
        NEServiceProxy::unbindService();
        unregisterNotify(MethodId_DeviceInfo);
        unregisterNotify(MethodId_ConnectedClientInfo);
    }

    VOID
    NCNetworkData::onReceiveMessage(NEMessage& message)
    {
        NCLOGD("NCNetworkData::onReceiveMessage - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        switch (message.methodId()) {
        case MethodId_DeviceInfo:
        {
            NCLOGD("NCNetworkData::onReceiveMessage - MethodId_DeviceInfo");
            nutshell::networkmanager::NDNetworkDevice in;
            if (!in.ParseFromString(message.param())) {
                NCLOGD("NCNetworkData::onReceiveMessage - Parse from string error");
                break;
            }

            NCString device;
            NCNetworkDataReplier::NetworkDeviceInfo deviceInfo;
            if (in.has_name()) {
                std ::string name = in.name();
                device = name.c_str();
            }
            if (in.has_ip()) {
                std::string ip = in.ip();
                deviceInfo.ip = ip.c_str();
            }
            if (in.has_mac()) {
                std::string mac = in.mac();
                deviceInfo.mac = mac.c_str();
            }
            if (in.has_prefixlength()) {
                deviceInfo.prefixLength = in.prefixlength();
            }
            if (in.has_gateway()) {
                std::string gateway = in.gateway();
                deviceInfo.gateway = gateway.c_str();
            }
            if (in.has_serverid()) {
                std::string serverId = in.serverid();
                deviceInfo.serverId = serverId.c_str();
            }

            NCNetworkDataReplier* replier = dynamic_cast<NCNetworkDataReplier*>(m_replyReceiver.get());
            if (NULL != replier) {
                replier->replyDeviceInfo(device, deviceInfo);
            }
        }
        break;
        case MethodId_ConnectedClientInfo:
        {
            NCLOGD("NCNetworkData::onReceiveMessage - MethodId_ConnectedClientInfo");
            nutshell::networkmanager::NDNetworkClient in;
            if (!in.ParseFromString(message.param())) {
                NCLOGD("NCNetworkData::onReceiveMessage - Parse from string error");
                break;
            }

            NCString macAddr;
            NCNetworkDataReplier::ConnectedClientInfo clientInfo;
            if (in.has_mac()) {
                std::string mac = in.mac();
                macAddr = mac.c_str();
            }
            if (in.has_ip()) {
                std::string ip = in.ip();
                clientInfo.ip = ip.c_str();
            }
            if (in.has_name()) {
                std::string name = in.name();
                clientInfo.name = name.c_str();
            }

            NCNetworkDataReplier* replier = dynamic_cast<NCNetworkDataReplier*>(m_replyReceiver.get());
            if (NULL != replier) {
                replier->replyConnectedClientInfo(macAddr, clientInfo);
            }
        }
        break;
        default:
            break;
        }
     }


} /* namespace nutshell */
/* EOF */
