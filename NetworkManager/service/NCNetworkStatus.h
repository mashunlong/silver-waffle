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
/**
 * @file NCNetworkStatus.h
 * @brief Declaration file of class NCNetworkStatus.
 *
 * This file includes the declaration of class NCNetworkStatus, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKSTATUS_H
#define NCNETWORKSTATUS_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <set>
#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCSyncObj.h>

namespace nutshell
{

    /**
     * @brief Class of NCNetworkStatus.
     *
     * This class is used to store the connection status of network devices.
     */
    class NCNetworkStatus
    {
    public:
        NCNetworkStatus();
        virtual ~NCNetworkStatus();

        VOID setStatus(const NCString& deviceType, NC_BOOL status);
        NC_BOOL status(const NCString& deviceType);

        VOID setCurrentNetwork(const NCString& deviceType);
        NCString currentNetwork();

        VOID setDcmDnsInfo(const NCString& dns1, const NCString& dns2);
        VOID dcmDnsInfo(NCString& dns1, NCString& dns2);

        VOID setGbookServerDns(const NCString& dcmDns1, const NCString& dcmDns2, const NCString& wifiDns1, const NCString& wifiDns2);
        VOID getGbookDcmDns(NCString& dns1, NCString& dns2);
        VOID getGbookWifiDns(NCString& dns1, NCString& dns2);

        NC_BOOL checkWifiApTrafficChange();
        NC_BOOL storeWifiApTraffic();

        NCString currentDhcpServer();
        VOID setCurrentDhcpServer(const NCString& deviceType);

        VOID setLinkState(const NCString& deviceType, const UINT32 dialState, const UINT32 dialType);
        UINT32 getLinkState(const NCString& deviceType);

        VOID setDialState(const NCString& deviceType, const UINT32 dialState, const UINT32 dialType);
        UINT32 getDialState(const NCString& deviceType);

        VOID setMirrorLinkStatus(const NC_BOOL status);
        NC_BOOL getMirrorLinkStatus();

        VOID setDunCarrierInfo(const NCString& info);
        VOID getDunCarrierInfo(NCString& info);

        VOID setWifiApConnectedStatus(const NC_BOOL status);
        NC_BOOL getWifiApConnectedStatus();

    private:
        NC_BOOL readWifiApTrafficFile(UINT32& rxBytes, UINT32& txBytes);

        std::set<NCString> m_status;
        NCString m_currNetwork;
        NCString m_dcmDns1;
        NCString m_dcmDns2;

        NCString m_gbookDcmDns1;
        NCString m_gbookDcmDns2;
        NCString m_gbookWifiDns1;
        NCString m_gbookWifiDns2;

        UINT32 m_rxBytes;
        UINT32 m_txBytes;
        NCSyncObj m_syncWifiApTraffic;

        NCString m_currDhcpServer;
        NCSyncObj m_syncCurrDhcpServer;

        NCSyncObj m_syncStatus;
        NCSyncObj m_syncCurrNetwork;
        NCSyncObj m_syncDcmDns;

        NCSyncObj m_syncGbookServerDns;

        UINT32 m_dcmLinkState;
        UINT32 m_dunLinkState;
        NCSyncObj m_syncLinkState;

        UINT32 m_wifiDialState;
        UINT32 m_dcmDialState;
        UINT32 m_dunDialState;
        NCSyncObj m_syncDialState;

        NC_BOOL m_mirrorLinkStatus;
        NCSyncObj m_syncMirrorLinkStatus;

        NCString m_dunCarrierInfo;
        NCSyncObj m_syncDunCarrierInfo;

        NC_BOOL m_wifiApConnectedStatus;

        NC_DISABLE_COPY(NCNetworkStatus);
    };

} /* namespace nutshell */

#endif /* NCNETWORKSTATUS_H */
/* EOF */
