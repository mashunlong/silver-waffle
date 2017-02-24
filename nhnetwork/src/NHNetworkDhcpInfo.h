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
 * @file NHNetworkDhcpInfo.h
 * @brief Declaration file of class NHNetworkDhcpInfo.
 *
 * This file includes the declaration of class NHNetworkDhcpInfo, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NHNETWORKDHCPINFO_H
#define NHNETWORKDHCPINFO_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <map>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCString.h>
#include <ncore/NCSyncObj.h>
#include "nhnetwork/NHNetworkDefine.h"

namespace nutshell
{
    /**
     * @brief NHNetworkDhcpInfo
     *
     * This class This class is used to store DHCP information.
     */
    class NHNetworkDhcpInfo
    {

    public:
        NHNetworkDhcpInfo();
        virtual ~NHNetworkDhcpInfo();

        VOID add(const NCString& deviceName, const DhclientInfo& info);
        NC_BOOL get(const NCString& deviceName, DhclientInfo& info);
        VOID remove(const NCString& deviceName);

        VOID updateDns(const NCString& deviceName, const NCString& dns1, const NCString& dns2);
        VOID updateGateway(const NCString& deviceName, const NCString& gateway);

        NC_BOOL getDhclientTimerStatus();
        VOID setDhclientTimerStatus(const NC_BOOL status);

        NC_BOOL checkDhclientRunningStatus();

    private:
        std::map<NCString, DhclientInfo> m_infoMap;
        NCSyncObj m_syncMap;

        NC_BOOL m_dhclientTimerStatus;
        NCSyncObj m_syncDhclientTimerStatus;

        NC_DISABLE_COPY(NHNetworkDhcpInfo);
    };

} /* namespace nutshell */

#endif /* NHNETWORKDHCPINFO_H */
/* EOF */