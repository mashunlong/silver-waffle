/**
 * Copyright @ 2013 - 2015 Suntec Software(Shanghai) Co., Ltd.
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
#include <cutils/properties.h>
#include <ncore/NCFile.h>
#include <ncore/NCError.h>
#include "nhnetwork/NHNetworkDefine.h"
#include "nhnetwork/NHNetworkHandler.h"
#include "NHNetworkDhcpInfo.h"
#include "NHNetworkTimer.h"

#define NC_LOG_TAG "NHNetworkHandler"

#define PROPERTY_DHCLIENT_INTERFACE "dhclient.interface"
#define PROPERTY_DHCLIENT_STATUS "dhclient.status"
#define PROPERTY_DHCLIENT_GATWAY "dhclient.gateway"
#define PROPERTY_DHCLIENT_IPADDR "dhclient.ipaddr"
#define PROPERTY_DHCLIENT_DNS1 "dhclient.dns1"
#define PROPERTY_DHCLIENT_DNS2 "dhclient.dns2"
#define PROPERTY_DHCLIENT_SERVER_ID "dhclient.server.identifier"

namespace nutshell
{

    NHNetworkTimer::NHNetworkTimer(const LONG msec, const NC_BOOL iter, const UINT32 type)
    : NCTimer(msec, iter)
    , m_type(type)
    {

    }

    NHNetworkTimer::~NHNetworkTimer()
    {

    }

    VOID
    NHNetworkTimer::OnTimer()
    {
        switch (m_type) {
        case TimerType_Dhclient:
        {
            NCFile flagFile(DHCLIENT_FLAG_FILE);
            if (!flagFile.open(NC_FOM_ReadWrite)) {
                NCLOGD("NHNetworkTimer::OnTimer, TimerType_Dhclient, Open file failed");
                NCErrorLog(NI_ERROR_HAL_NETWORK_OPENFILE_FAILED, 0);
                break;
            }

            NCCHAR buf[16] = { 0 };
            NCCHAR end[] = "end";
            flagFile.read(buf, sizeof(buf)-1);
            NCLOGD("NHNetworkTimer::OnTimer, TimerType_Dhclient, buf = [%s]", buf);

            if (strncmp(end, buf, strlen(end))) {
                break;
            }

            NCString deviceName;
            DhclientInfo newInfo;
            readDhclientProperty(deviceName, newInfo);

            NCCHAR init[] = "init";
            flagFile.seek(0, NC_FSM_Begin);
            flagFile.write(init, sizeof(init));
            flagFile.close();

            if (NCString("") == deviceName) {
                NCLOGD("NHNetworkTimer::OnTimer, TimerType_Dhclient, Read device name null");
                break;
            }

            NCLOGD("NHNetworkTimer::OnTimer, TimerType_Dhclient, deviceName [%s], status [%s]", deviceName.getString(), 
                newInfo.status.getString());
            DhclientInfo oldInfo;
            NC_BOOL find = NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->get(deviceName, oldInfo);
            if (!find) {
                NCLOGD("NHNetworkTimer::OnTimer, TimerType_Dhclient, Find info failed");
                NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->add(deviceName, newInfo);
                break;
            }

            if (NCString("wlan0") != deviceName) {
                NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->add(deviceName, newInfo);
                break;
            }

            if (NCString("running") != newInfo.status) {
                NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->add(deviceName, newInfo);
                return;
            }

            if (oldInfo.ip != newInfo.ip || oldInfo.gateway != newInfo.gateway) {
                NCLOGD("NHNetworkTimer::OnTimer, TimerType_Dhclient, old ip [%s], new ip [%s]", oldInfo.ip.getString(), newInfo.ip.getString());
                NCLOGD("NHNetworkTimer::OnTimer, TimerType_Dhclient, old gateway [%s], new gateway [%s]", oldInfo.gateway.getString(), newInfo.gateway.getString());
                NHNetworkHandler::getNetworkHandler()->postRequest(new NHNetworkAddDefauteRouteReq(deviceName, newInfo.gateway, NC_TRUE));
            }

            if (oldInfo.dns1 != newInfo.dns1 || oldInfo.dns2 != newInfo.dns2) {
                NCLOGD("NHNetworkTimer::OnTimer, TimerType_Dhclient, old dns1 [%s], new dns1 [%s]", oldInfo.dns1.getString(), newInfo.dns1.getString());
                NCLOGD("NHNetworkTimer::OnTimer, TimerType_Dhclient, old dns2 [%s], new dns2 [%s]", oldInfo.dns2.getString(), newInfo.dns2.getString());
                NHNetworkHandler::getNetworkHandler()->postRequest(new NHNetworkUpdateRemoteDnsReq(newInfo.dns1, newInfo.dns2));
            }

            NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->add(deviceName, newInfo);
            break;
        }
        default:
            break;
        }
    }

    VOID
    NHNetworkTimer::readDhclientProperty(NCString& deviceName, DhclientInfo& info)
    {
        NCCHAR value[PROPERTY_VALUE_MAX] = { 0 };
        property_get(PROPERTY_DHCLIENT_INTERFACE, value, NULL);
        deviceName = value;

        value[0] = '\0';
        property_get(PROPERTY_DHCLIENT_STATUS, value, NULL);
        info.status = value;

        value[0] = '\0';
        property_get(PROPERTY_DHCLIENT_IPADDR, value, NULL);
        info.ip = value;

        value[0] = '\0';
        property_get(PROPERTY_DHCLIENT_GATWAY, value, NULL);
        info.gateway = value;

        value[0] = '\0';
        property_get(PROPERTY_DHCLIENT_DNS1, value, NULL);
        info.dns1 = value;

        value[0] = '\0';
        property_get(PROPERTY_DHCLIENT_DNS2, value, NULL);
        info.dns2 = value;

        value[0] = '\0';
        property_get(PROPERTY_DHCLIENT_SERVER_ID, value, NULL);
        info.serverId = value;
    }

} /* namespace nutshell */
/* EOF */
