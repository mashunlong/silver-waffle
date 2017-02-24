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
#include "NCNetworkManagerDef.h"
#include <NDNetworkManager.pb.h>
#include "networkmanager/NCNetworkDhcpReplier.h"
#include "networkmanager/NCNetworkDhcp.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{
    NCNetworkDhcp::NCNetworkDhcp()
    : NCBinderServiceProxy(PFSERVICE_NETWORKMANAGER_NAME)
    , NEServiceProxy(NULL)
    {

    }

    NCNetworkDhcp::~NCNetworkDhcp()
    {

    }

    INT32
    NCNetworkDhcp::requestIp(const NCString& deviceType, const NCString& deviceName)
    {
        NCLOGD("NCNetworkDhcp::requestIp - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkDhcp::requestIp - Start, deviceType [%s], deviceName [%s]", deviceType.getString(), deviceName.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        NC_ERROR ret = sendCommand(FunctionId_RequestIp, data, NULL);
        return ret;
    }

    INT32
    NCNetworkDhcp::requestIp(const NCString& resourceId, const NCString& deviceType, const NCString& deviceName)
    {
        NCLOGD("NCNetworkDhcp::requestIp - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkDhcp::requestIp - Start, resourceId [%s], deviceType [%s], deviceName [%s]", resourceId.getString(), deviceType.getString(), deviceName.getString());
        NCParcel data;
        data.writeCString(resourceId.getString());
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        NC_ERROR ret = sendCommand(FunctionId_RequestIpResourceId, data, NULL);
        return ret;
    }

    INT32
    NCNetworkDhcp::stopRequestIp(const NCString& deviceType, const NCString& deviceName)
    {
        NCLOGD("NCNetworkDhcp::stopRequestIp - Start, deviceType = [%s], deviceName = [%s]", deviceType.getString(), deviceName.getString());
        return 0;
    }

    INT32
    NCNetworkDhcp::releaseIp(const NCString& deviceType, const NCString& deviceName)
    {
        NCLOGD("NCNetworkDhcp::releaseIp - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        NCLOGD("NCNetworkDhcp::releaseIp - Start, deviceType = [%s], deviceName = [%s]", deviceType.getString(), deviceName.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        NC_ERROR ret = sendCommand(FunctionId_ReleaseIp, data, NULL);
        return ret;
    }

    INT32
    NCNetworkDhcp::startDhcpServer(const NCString& deviceType, const NCString& deviceName)
    {
        NCLOGD("NCNetworkDhcp::startDhcpServer - Start, deviceType = [%s], deviceName = [%s]", deviceType.getString(), deviceName.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        NC_ERROR ret = sendCommand(FunctionId_StartDhcpServer, data, NULL);
        return ret;
    }

    INT32
    NCNetworkDhcp::stopDhcpServer(const NCString& deviceType, const NCString& deviceName)
    {
        NCLOGD("NCNetworkDhcp::stopDhcpServer - Start, deviceType = [%s], deviceName = [%s]", deviceType.getString(), deviceName.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        NC_ERROR ret = sendCommand(FunctionId_StopDhcpServer, data, NULL);
        return ret;
    }

    NC_BOOL
    NCNetworkDhcp::checkDhcpClientStatus(const NCString& deviceName)
    {
        NCLOGD("NCNetworkDhcp::checkDhcpClientStatus - Start, deviceName = [%s]", deviceName.getString());
        return 0;
    }

    NC_BOOL
    NCNetworkDhcp::checkDhcpServerStatus()
    {
        NCLOGD("NCNetworkDhcp::checkDhcpServerStatus - Start");
        NCParcel data;
        NCParcel reply;
        INT32 ret = sendCommand(FunctionId_CheckDhcpServerStatus, data, &reply);
        NC_BOOL status = reply.readInt32();
        return status;
    }

    INT32
    NCNetworkDhcp::startMonitorDataComm(const NCString& deviceType, const NCString& deviceName)
    {
        NCLOGD("NCNetworkDhcp::startMonitorDataComm - Start, deviceType [%s], deviceName [%s]", 
            deviceType.getString(), deviceName.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        NC_ERROR ret = sendCommand(FunctionId_StartMonitorDataComm, data, NULL);
        return ret;
    }


    INT32
    NCNetworkDhcp::stopMonitorDataComm(const NCString& deviceType, const NCString& deviceName)
    {
        NCLOGD("NCNetworkDhcp::stopMonitorDataComm - Start, deviceType [%s], deviceName [%s]", 
            deviceType.getString(), deviceName.getString());
        NCParcel data;
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        NC_ERROR ret = sendCommand(FunctionId_StopMonitorDataComm, data, NULL);
        return ret;
    }

    NC_BOOL
    NCNetworkDhcp::bindService()
    {
        NCLOGD("NCNetworkDhcp::bindService - Start");
        ncsp<NEIpcServiceProxy>::sp service =
        NEIpcServiceHelper::findService(NCString(PFSERVICE_NETWORKMANAGER_NAME), 0);

        NC_BOOL ret = NEServiceProxy::bindService(service);
        if (!ret) {
            return ret;
        }
        registerNotify(MethodId_RequestIp);
        registerNotify(MethodId_ReleaseIp);
        registerNotify(MethodId_StartDhcpServer);
        registerNotify(MethodId_StopDhcpServer);
        return NC_TRUE;
    }

    VOID
    NCNetworkDhcp::unbindService()
    {
        NCLOGD("NCNetworkDhcp::unbindService - Start");
        NEServiceProxy::unbindService();

        unregisterNotify(MethodId_RequestIp);
        unregisterNotify(MethodId_ReleaseIp);
        unregisterNotify(MethodId_StartDhcpServer);
        unregisterNotify(MethodId_StopDhcpServer);
    }

    VOID NCNetworkDhcp::onReceiveMessage(NEMessage& message)
    {
        NCLOGD("NCNetworkDhcp::onReceiveMessage - Start, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        std::string device;
        UINT32 result;
        nutshell::networkmanager::NDNetworkDhcpResult in;
        if (!in.ParseFromString(message.param())) {
            NCLOGD("NCNetworkDhcp::onReceiveMessage - Parse from string error");
        }
        else {
            if (in.has_device()) {
                device = in.device();
            }
            if (in.has_result()) {
                result = in.result();
            }
        }

        switch (message.methodId()) {
        case MethodId_RequestIp:
        {
            NCLOGD("NCNetworkDhcp::onReceiveMessage - MethodId_RequestIp");
            NCString name(device.c_str());
            NCNetworkDhcpReplier* replier = dynamic_cast<NCNetworkDhcpReplier*>(m_replyReceiver.get());
            if (NULL != replier) {
                replier->replyRequestIp(name, result);
            }
        }
        break;
        case MethodId_ReleaseIp:
        {
            NCLOGD("NCNetworkDhcp::onReceiveMessage - MethodId_ReleaseIp");
            NCString name(device.c_str());
            NCNetworkDhcpReplier* replier = dynamic_cast<NCNetworkDhcpReplier*>(m_replyReceiver.get());
            if (NULL != replier) {
                replier->replyReleaseIp(name, result);
            }
        }
        break;
        case MethodId_StartDhcpServer:
        {
            NCLOGD("NCNetworkDhcp::onReceiveMessage - MethodId_StartDhcpServer");
            NCString name(device.c_str());
            NCNetworkDhcpReplier* replier = dynamic_cast<NCNetworkDhcpReplier*>(m_replyReceiver.get());
            if (NULL != replier) {
                replier->replyStartDhcpServer(name, result);
            }
        }
        break;
        case MethodId_StopDhcpServer:
        {
            NCLOGD("NCNetworkDhcp::onReceiveMessage - MethodId_StopDhcpServer");
            NCString name(device.c_str());
            NCNetworkDhcpReplier* replier = dynamic_cast<NCNetworkDhcpReplier*>(m_replyReceiver.get());
            if (NULL != replier) {
                replier->replyStopDhcpServer(name, result);
            }
        }
        break;
        default:
            break;
        }
    }

} /* namespace nutshell */
/* EOF */
