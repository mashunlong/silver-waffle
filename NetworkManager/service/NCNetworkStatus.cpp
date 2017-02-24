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
#include <NDNetworkManager.pb.h>
#include "networkmanager/NCNetworkDef.h"
#include "NCNetworkManager.h"
#include "NCNetworkAvcUnit.h"
#include "NCNetworkSignal.h"
#include "NCNetworkHandler.h"
#include "NCNetworkNotify.h"
#include "NCNetworkStatus.h"

namespace nutshell
{
#define NC_LOG_TAG "NCNetworkManager"

    NCNetworkStatus::NCNetworkStatus()
    : m_currNetwork("")
    , m_dcmDns1("")
    , m_dcmDns2("")
    , m_gbookDcmDns1("")
    , m_gbookDcmDns2("")
    , m_gbookWifiDns1("")
    , m_gbookWifiDns2("")
    , m_currDhcpServer("")
    , m_dcmLinkState(DialState_Init)
    , m_dunLinkState(DialState_Init)
    , m_wifiDialState(DialState_Init)
    , m_dcmDialState(DialState_Init)
    , m_dunDialState(DialState_Init)
    , m_mirrorLinkStatus(NC_FALSE)
    , m_wifiApConnectedStatus(NC_FALSE)
    {

    }

    NCNetworkStatus::~NCNetworkStatus()
    {

    }

    VOID
    NCNetworkStatus::setStatus(const NCString& deviceType, NC_BOOL status)
    {
        m_syncStatus.syncStart();
        std::set<NCString>::iterator iter = m_status.find(deviceType);
        NC_BOOL syncFlag = NC_FALSE;
        if (status) {
            if (iter != m_status.end()) {
                m_syncStatus.syncEnd();
                return;
            }

            m_status.insert(deviceType);
            m_syncStatus.syncEnd();
            NCLOGD("NCNetworkStatus::setStatus - deviceType = [%s], status = [%d]", deviceType.getString(), status);

            // notify the network device status is changed to be connection
            NCNetworkNotify notify;
            notify.broadcastNetworkStatus(NCNETWORK_EVENT_CONNECT, deviceType);
            syncFlag = NC_TRUE;
        }
        else {
            if (iter == m_status.end()) {
                m_syncStatus.syncEnd();
                return;
            }
            
            m_status.erase(deviceType);
            m_syncStatus.syncEnd();
            NCLOGD("NCNetworkStatus::setStatus - deviceType = [%s], status = [%d]", deviceType.getString(), status);

            if (BT_DUN == deviceType) {
                setDunCarrierInfo(NCString(""));
            }

            // notify the network device status is changed to be disconnection
            NCNetworkNotify notify;
            notify.broadcastNetworkStatus(NCNETWORK_EVENT_DISCONNECT, deviceType);
            syncFlag = NC_TRUE;
        }


        NCNetworkHandler handler;
        if (!handler.checkIndividualApparatus() && syncFlag) {
#if defined NCNETWORK_TARGET_PRODUCT_DCU
            if (BT_DUN == deviceType || WIFI_STA == deviceType || WIFI_AP == deviceType || WIFI_P2P == deviceType) {
                NCNetworkManager::Instance()->getNetworkAvcUnit()->notifyCommStatusChange();
            }
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
            if (DCM == deviceType) {
                NCNetworkManager::Instance()->getNetworkAvcUnit()->notifyCommStatusChange();
            }
#endif
        }

    }

    NC_BOOL
    NCNetworkStatus::status(const NCString& device)
    {
        m_syncStatus.syncStart();
        std::set<NCString>::iterator iter = m_status.find(device);
        if (iter != m_status.end()) {
            m_syncStatus.syncEnd();
            return NC_TRUE;
        }
        else {
            m_syncStatus.syncEnd();
            return NC_FALSE;
        }
    }

    VOID
    NCNetworkStatus::setCurrentNetwork(const NCString& deviceType)
    {
        m_syncCurrNetwork.syncStart();
        NCLOGD("NCNetworkStatus::setCurrentNetwork, deviceType [%s]", deviceType.getString());
        if (m_currNetwork == deviceType) {
            m_syncCurrNetwork.syncEnd();
            return;
        }

        NCString tempNetwork = m_currNetwork;
        m_currNetwork = deviceType;
        m_syncCurrNetwork.syncEnd();

        // update environment info
        NCNetworkSignal mySignal;
        NCNetworkHandler handler;
        if (NCString("") != deviceType) {
            mySignal.updateNetworkStatus(deviceType, "connected");
        }
        else {
            mySignal.updateNetworkStatus(tempNetwork, "disconnected");
            handler.clearNatTable();
        }

        // notify the current use network device
        NCNetworkNotify notify;
        notify.broadcastNetworkStatus(NCNETWORK_EVENT_ROUTECHANGED, deviceType);

        if (!handler.checkIndividualApparatus()) {
            NCNetworkManager::Instance()->getNetworkAvcUnit()->notifyCommStatusChange();
        }
    }

    NCString
    NCNetworkStatus::currentNetwork()
    {
        NCAutoSync lock(m_syncCurrNetwork);
        return m_currNetwork;
    }

    VOID
    NCNetworkStatus::setDcmDnsInfo(const NCString& dns1, const NCString& dns2)
    {
        NCLOGD("NCNetworkStatus::setDcmDnsInfo, dns1 [%s], dns2 [%s]", dns1.getString(), dns2.getString());
        m_syncDcmDns.syncStart();
        m_dcmDns1 = dns1;
        m_dcmDns2 = dns2;
        m_syncDcmDns.syncEnd();
    }
    
    VOID
    NCNetworkStatus::dcmDnsInfo(NCString& dns1, NCString& dns2)
    {
        NCLOGD("NCNetworkStatus::dcmDnsInfo, Start, m_dcmDns1 [%s], m_dcmDns2 [%s]", m_dcmDns1.getString(), m_dcmDns2.getString());
        m_syncDcmDns.syncStart();
        dns1 = m_dcmDns1;
        dns2 = m_dcmDns2;
        m_syncDcmDns.syncEnd();
    }

    VOID
    NCNetworkStatus::setGbookServerDns(const NCString& dcmDns1, const NCString& dcmDns2, const NCString& wifiDns1, const NCString& wifiDns2)
    {
        NCLOGD("NCNetworkStatus::setGbookDcmDns, Start, dcmDns1 [%s], dcmDns2 [%s], wifiDns1 [%s], wifiDns2 [%s]", dcmDns1.getString(), dcmDns2.getString(), 
            wifiDns1.getString(), wifiDns2.getString());
        NCAutoSync lock(m_syncGbookServerDns);
        m_gbookDcmDns1 = dcmDns1;
        m_gbookDcmDns2 = dcmDns2;
        m_gbookWifiDns1 = wifiDns1;
        m_gbookWifiDns2 = wifiDns2;
    }

    VOID
    NCNetworkStatus::getGbookDcmDns(NCString& dns1, NCString& dns2)
    {
        NCLOGD("NCNetworkStatus::getGbookDcmDns, Start");
        NCAutoSync lock(m_syncGbookServerDns);
        dns1 = m_gbookDcmDns1;
        dns2 = m_gbookDcmDns2;
    }

    VOID
    NCNetworkStatus::getGbookWifiDns(NCString& dns1, NCString& dns2)
    {
        NCLOGD("NCNetworkStatus::getGbookWifiDns, Start");
        NCAutoSync lock(m_syncGbookServerDns);
        dns1 = m_gbookWifiDns1;
        dns2 = m_gbookWifiDns2;
    }


    NC_BOOL
    NCNetworkStatus::checkWifiApTrafficChange()
    {
        NCLOGD("NCNetworkStatus::checkWifiApTrafficChange, Start");
        NCAutoSync lock(m_syncWifiApTraffic);
        UINT32 rxData;
        UINT32 txData;
        NC_BOOL ret = readWifiApTrafficFile(rxData, txData);
        if (ret) {
            if (rxData != m_rxBytes || txData != m_txBytes) {
                return NC_TRUE;
            }
            else {
                return NC_FALSE;
            }
        }
        else {
            return NC_FALSE;
        }
    }

    NC_BOOL
    NCNetworkStatus::storeWifiApTraffic()
    {
        NCLOGD("NCNetworkStatus::storeWifiApTraffic, Start");
        NCAutoSync lock(m_syncWifiApTraffic);
        UINT32 rxData;
        UINT32 txData;
        NC_BOOL ret = readWifiApTrafficFile(rxData, txData);
        if (ret) {
            m_rxBytes = rxData;
            m_txBytes = txData;
        }

        return ret;
    }

    NC_BOOL
    NCNetworkStatus::readWifiApTrafficFile(UINT32& rxBytes, UINT32& txBytes)
    {
        NCFile rxFile(WIFI_AP_TRAFFIC_RX_FILE);
        if (!rxFile.open(NC_FOM_ReadOnly)) {
            NCLOGD("NCNetworkStatus::readWifiApTrafficFile,  Open [%s] failed", WIFI_AP_TRAFFIC_RX_FILE);
            return NC_FALSE;
        }

        NCCHAR rxBuf[16] = { 0 };
        rxFile.read(rxBuf, sizeof(rxBuf)-1);
        UINT32 rxData;
        sscanf(rxBuf, "%10d", &rxData);

        NCFile txFile(WIFI_AP_TRAFFIC_TX_FILE);
        if (!txFile.open(NC_FOM_ReadOnly)) {
            NCLOGD("NCNetworkStatus::readWifiApTrafficFile,  Open [%s] failed", WIFI_AP_TRAFFIC_TX_FILE);
            return NC_FALSE;
        }

        NCCHAR txBuf[16] = { 0 };
        txFile.read(txBuf, sizeof(txBuf)-1);
        UINT32 txData;
        sscanf(txBuf, "%10d", &txData);

        NCLOGD("NCNetworkStatus::readWifiApTrafficFile, RX [%d], TX [%d]", rxData, txData);
        rxBytes = rxData;
        txBytes = txData;
        return NC_TRUE;
    }

    NCString
    NCNetworkStatus::currentDhcpServer()
    {
        NCLOGD("NCNetworkStatus::currentDhcpServer, Start");
        NCAutoSync lock(m_syncCurrDhcpServer);
        return m_currDhcpServer;
    }
    
    VOID
    NCNetworkStatus::setCurrentDhcpServer(const NCString& deviceType)
    {
        NCLOGD("NCNetworkStatus::setCurrentDhcpServer, Start, deviceType [%s]", deviceType.getString());
        NCAutoSync lock(m_syncCurrDhcpServer);
        m_currDhcpServer = deviceType;
    }

    VOID
    NCNetworkStatus::setLinkState(const NCString& deviceType, const UINT32 dialState, const UINT32 dialType)
    {
        NCLOGD("NCNetworkStatus::setLinkState, Start, deviceType [%s], dialState [%d], dialType [%d]", 
            deviceType.getString(), dialState, dialType);
        NCAutoSync lock(m_syncLinkState);
        NCNetworkNotify notify;
        if (DCM == deviceType) {
            if (dialState != m_dcmLinkState) {
                m_dcmLinkState = dialState;
                notify.notifyLinkState(deviceType, dialState, dialType);
            }
        }
        else if (BT_DUN == deviceType) {
            if (dialState != m_dunLinkState) {
                m_dunLinkState = dialState;
                notify.notifyLinkState(deviceType, dialState, dialType);
            }
        }
        else {
            NCLOGD("NCNetworkStatus::setLinkState, parameters error");
        }
    }

    UINT32
    NCNetworkStatus::getLinkState(const NCString& deviceType)
    {
        NCLOGD("NCNetworkStatus::getLinkState, Start, deviceType [%s]", deviceType.getString());
        NCAutoSync lock(m_syncLinkState);
        if (DCM == deviceType) {
            return m_dcmLinkState;
        }
        else if (BT_DUN == deviceType) {
            return m_dunLinkState;
        }
        else {
            NCLOGD("NCNetworkStatus::setLinkState, parameters error");
            return 0;
        }
    }

    VOID
    NCNetworkStatus::setDialState(const NCString& deviceType, const UINT32 dialState, const UINT32 dialType)
    {
        NCLOGD("NCNetworkStatus::setDialState, Start, deviceType [%s], dialState [%d], dialType [%d]", 
            deviceType.getString(), dialState, dialType);
        NCAutoSync lock(m_syncDialState);
        NCNetworkNotify notify;
        if (DCM == deviceType) {
            if (dialState != m_dcmDialState) {
                m_dcmDialState = dialState;
                notify.notifyDialState(deviceType, dialState, dialType);
            }
        }
        else if (BT_DUN == deviceType) {
            if (dialState != m_dunDialState) {
                m_dunDialState = dialState;
                notify.notifyDialState(deviceType, dialState, dialType);
            }
        }
        else if (WIFI_STA == deviceType) {
            if (dialState != m_wifiDialState) {
                m_wifiDialState = dialState;
                notify.notifyDialState(deviceType, dialState, dialType);
            }
        }
        else {
            NCLOGD("NCNetworkStatus::setLinkState, parameters error");
        }
    }

    UINT32
    NCNetworkStatus::getDialState(const NCString& deviceType)
    {
        NCLOGD("NCNetworkStatus::setDialState, Start, deviceType [%s]", deviceType.getString());
        NCAutoSync lock(m_syncDialState);
        NCNetworkNotify notify;
        if (DCM == deviceType) {
            return m_dcmDialState;
        }
        else if (BT_DUN == deviceType) {
            return m_dunDialState;
        }
        else if (WIFI_STA == deviceType) {
            return m_wifiDialState;
        }
        else {
            NCLOGD("NCNetworkStatus::setLinkState, parameters error");
            return 0;
        }
    }

    VOID
    NCNetworkStatus::setMirrorLinkStatus(const NC_BOOL status)
    {
        NCLOGD("NCNetworkStatus::setMirrorLinkStatus, Start, status [%d]", status);
        NCAutoSync lock(m_syncMirrorLinkStatus);
        if (status != m_mirrorLinkStatus) {
            NCLOGD("NCNetworkStatus::setMirrorLinkStatus, MirrorLink status changed, local status [%d]", m_mirrorLinkStatus);
            m_mirrorLinkStatus = status;
            NCNetworkManager::Instance()->getNetworkAvcUnit()->notifyCommStatusChange();
        }
    }
    
    NC_BOOL
    NCNetworkStatus::getMirrorLinkStatus()
    {
        NCLOGD("NCNetworkStatus::getMirrorLinkStatus, Start, status [%d]", m_mirrorLinkStatus);
        NCAutoSync lock(m_syncMirrorLinkStatus);
        return m_mirrorLinkStatus;
    }

    VOID
    NCNetworkStatus::setDunCarrierInfo(const NCString& info)
    {
        NCLOGD("NCNetworkStatus::setDunCarrierInfo, info = [%s]", info.getString());
        NCAutoSync lock(m_syncDunCarrierInfo);
        if (strncmp(info.getString(), m_dunCarrierInfo.getString(), 16)) {
            NCLOGD("NCNetworkStatus::setDunCarrierInfo, dun carrier info changed, local info [%s]", m_dunCarrierInfo.getString());
            m_dunCarrierInfo = info;
            NCNetworkManager::Instance()->getNetworkAvcUnit()->notifyCommStatusChange();
        }
    }

    VOID
    NCNetworkStatus::getDunCarrierInfo(NCString& info)
    {
        NCAutoSync lock(m_syncDunCarrierInfo);
        info = m_dunCarrierInfo;
    }

    VOID
    NCNetworkStatus::setWifiApConnectedStatus(const NC_BOOL status)
    {
        NCLOGD("NCNetworkStatus::setWifiApConnectedStatus, status [%d]", status);
        m_wifiApConnectedStatus = status;
    }

    NC_BOOL
    NCNetworkStatus::getWifiApConnectedStatus()
    {
        NCLOGD("NCNetworkStatus::getWifiApConnectedStatus, status [%d]", m_wifiApConnectedStatus);
        return m_wifiApConnectedStatus;
    }

} /* namespace nutshell */
/* EOF */
