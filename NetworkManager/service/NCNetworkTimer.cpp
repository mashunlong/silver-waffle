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
#include <ncore/NCFile.h>
#include <connectmanager/NCConnect.h>
#include "NCNetworkManager.h"
#include "NCNetworkThread.h"
#include "NCNetworkManagerDef.h"
#include "NCNetworkAvcUnit.h"
#include "NCNetworkReq.h"
#include "NCNetworkStatus.h"
#include "NCNetworkEventReq.h"
#include "NCNetworkTimer.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{

    NCNetworkTimer::NCNetworkTimer(const UINT32 type, LONG msec, NC_BOOL iter)
    : NCTimer(msec, iter)
    , m_type(type)
    {

    }

    NCNetworkTimer::~NCNetworkTimer()
    {

    }

    VOID
    NCNetworkTimer::OnTimer()
    {
        switch (m_type) {
        case TimerType_DcuMeuSync:
        {
            NCLOGD("NCNetworkTimer::OnTimer, Sync status between dcu and meu");
            NCNetworkManager::Instance()->getNetworkAvcUnit()->notifyCommStatusChange();
            break;
        }
        case TimerType_AvcLanIp:
        {
            NCLOGD("NCNetworkTimer::OnTimer, Set AvcLan IP");
            NCNetworkManager::Instance()->postRequest(new NCNetworkSetAvcLanIpReq());
            break;
        }
        case TimerType_WifiAp:
        {
            NCLOGD("NCNetworkTimer::OnTimer, Check Wifi Hotspot");
            NCString currNetwork = NCNetworkManager::Instance()->getNetworkStatus()->currentNetwork();
            if (DCM == currNetwork) {
                NCLOGD("NCNetworkTimer::startWifiApTimer, Current network is dcm");
                NCNetworkManager::Instance()->stopTimer(TimerType_WifiAp);
                break;
            }

            if (!NCNetworkManager::Instance()->getNetworkStatus()->status(WIFI_AP)) {
                NCLOGD("NCNetworkTimer::startWifiApTimer, WIFI AP is not running");
                NCNetworkManager::Instance()->stopTimer(TimerType_WifiAp);
                break;
            }

            if (!NCNetworkManager::Instance()->getNetworkStatus()->getWifiApConnectedStatus()) {
                NCLOGD("NCNetworkTimer::startWifiApTimer, No device connect to WIFI AP");
                return;
            }

            if (!NCFile::exists(WIFI_AP_TRAFFIC_RX_FILE) || !NCFile::exists(WIFI_AP_TRAFFIC_TX_FILE)) {
                NCLOGD("NCNetworkTimer::startWifiApTimer, WLAN0 is not exists");
                NCNetworkManager::Instance()->stopTimer(TimerType_WifiAp);
                break;
            }

            NCConnect connectProxy;
            if (!connectProxy.verifyPermission(WIFI_AP_RESOURCE_ID)) {
                NCLOGD("NCNetworkTimer::startWifiApTimer, WIFI AP has not permission");
                NCNetworkManager::Instance()->stopTimer(TimerType_WifiAp);
                break;
            }

            if (!NCNetworkManager::Instance()->getNetworkStatus()->checkWifiApTrafficChange()) {
                NCLOGD("NCNetworkTimer::startWifiApTimer, WIFI AP traffic is not change");
                break;
            }
            else {
                NCNetworkManager::Instance()->getNetworkStatus()->storeWifiApTraffic();
            }

            if (NCNetworkManager::Instance()->getWifiApFlag(WifiApTimerFlag_OnTimer)) {
                NCLOGD("NCNetworkTimer::startWifiApTimer, Connecting dcm ...");
                break;
            }

            NCNetworkManager::Instance()->setWifiApFlag(WifiApTimerFlag_OnTimer, NC_TRUE);
            NCLOGD("NCNetworkTimer::startWifiApTimer, Post reqeust to connect dcm");

            AccessParameter param;
            param.reqId = NCNetworkManager::Instance()->getRequestSequenceId();
            param.reqType = RequestType_Access;
            param.resourceId = WIFI_AP_RESOURCE_ID;
            param.deviceType = DCM;
            param.dealType = DialType_Auto;
            param.commPriority = CommunicatePriority_AutoHigh;
            param.wifiApFlag = NC_TRUE;
            NCNetworkManager::Instance()->getEventThreadPointer()->looper().postRunnable(new NCNetworkCheckRequstReq(param));
            break;
        }
        case TimerType_RequestMap:
        {
            NCLOGD("NCNetworkTimer::OnTimer, Check request map");
            NCNetworkManager::Instance()->getEventThreadPointer()->looper().postRunnable(new NCNetworkCheckTimeoutRequestMapReq(RequestMapType_Init));
            break;
        }
        default:
            break;
        }
    }

} /* namespace nutshell */
/* EOF */
