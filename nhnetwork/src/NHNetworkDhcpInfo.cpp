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
#include <ncore/NCAutoSync.h>
#include "NHNetworkDhcpInfo.h"

#define NC_LOG_TAG "NHNetworkHandler"

namespace nutshell
{
    NHNetworkDhcpInfo::NHNetworkDhcpInfo()
    : m_dhclientTimerStatus(NC_FALSE)
    {

    }

    NHNetworkDhcpInfo::~NHNetworkDhcpInfo()
    {

    }

    VOID
    NHNetworkDhcpInfo::add(const NCString& deviceName, const DhclientInfo& info)
    {
        NCAutoSync lock(m_syncMap);
        m_infoMap.erase(deviceName);
        m_infoMap[deviceName] = info;
        NCLOGD("NHNetworkDhcpInfo::add, deviceName [%s], status [%s], gateway [%s], dns1 [%s], dns2 [%s]", 
            deviceName.getString(), m_infoMap[deviceName].status.getString(), m_infoMap[deviceName].gateway.getString(), m_infoMap[deviceName].dns1.getString(), m_infoMap[deviceName].dns2.getString());
    }

    NC_BOOL
    NHNetworkDhcpInfo::get(const NCString& deviceName, DhclientInfo& info)
    {
        NCLOGD("NHNetworkDhcpInfo::get - deviceName[%s]", deviceName.getString());
        NCAutoSync lock(m_syncMap);
        if (m_infoMap.count(deviceName) > 0) {
            info = m_infoMap[deviceName];
            return NC_TRUE;
        }
        return NC_FALSE;
    }

    VOID
    NHNetworkDhcpInfo::remove(const NCString& deviceName)
    {
        NCLOGD("NHNetworkDhcpInfo::remove - deviceName[%s]", deviceName.getString());
        NCAutoSync lock(m_syncMap);
        m_infoMap.erase(deviceName);
    }

    VOID
    NHNetworkDhcpInfo::updateDns(const NCString& deviceName, const NCString& dns1, const NCString& dns2)
    {
        NCLOGD("NHNetworkDhcpInfo::updateDns - deviceName[%s], dns1 [%s], dns2 [%s]", deviceName.getString(), dns1.getString(), 
            dns2.getString());
        NCAutoSync lock(m_syncMap);
        std::map<NCString, DhclientInfo>::iterator iter = m_infoMap.find(deviceName);
        if (iter != m_infoMap.end()) {
            DhclientInfo info = iter->second;
            info.dns1 = dns1;
            info.dns2 = dns2;
            iter->second = info;
        }
        else {
            DhclientInfo info;
            info.dns1 = dns1;
            info.dns2 = dns2;
            m_infoMap[deviceName] = info;
        }

    }

    VOID
    NHNetworkDhcpInfo::updateGateway(const NCString& deviceName, const NCString& gateway)
    {
        NCLOGD("NHNetworkDhcpInfo::updateGateway, deviceName [%s], gateway [%s]", deviceName.getString(), gateway.getString());
        NCAutoSync lock(m_syncMap);
        std::map<NCString, DhclientInfo>::iterator iter = m_infoMap.find(deviceName);
        if (iter != m_infoMap.end()) {
            DhclientInfo info = iter->second;
            info.gateway = gateway;
            iter->second = info;
        }
        else {
            DhclientInfo info;
            info.gateway = gateway;
            m_infoMap[deviceName] = info;
        }
    }

    NC_BOOL
    NHNetworkDhcpInfo::getDhclientTimerStatus()
    {
        NCLOGD("NHNetworkDhcpInfo::getDhclientTimerStatus, m_status [%d]", m_dhclientTimerStatus);
        NCAutoSync lock(m_syncDhclientTimerStatus);
        return m_dhclientTimerStatus;
    }

    VOID
    NHNetworkDhcpInfo::setDhclientTimerStatus(const NC_BOOL status)
    {
        NCLOGD("NHNetworkDhcpInfo::setDhclientTimerStatus, status [%d]", status);
        NCAutoSync lock(m_syncDhclientTimerStatus);
        m_dhclientTimerStatus = status;
    }

    NC_BOOL
    NHNetworkDhcpInfo::checkDhclientRunningStatus()
    {
        NCLOGD("NHNetworkDhcpInfo::checkDhclientRunning, Start");
        NCAutoSync lock(m_syncMap);
        std::map<NCString, DhclientInfo>::iterator iter = m_infoMap.begin();
        while (iter != m_infoMap.end()) {
            if (NCString("running") == iter->second.status) {
                return NC_TRUE;
            }
            ++iter;
        }

        return NC_FALSE;
    }
    
} /* namespace nutshell */
/* EOF */
