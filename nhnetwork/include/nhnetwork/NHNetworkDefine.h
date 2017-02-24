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
/**
 * @file NHNetworkDefine.h
 * @brief Declaration file of types of NHNetwork
*/

#ifndef NHNETWORKDEFINE_H
#define NHNETWORKDEFINE_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <ncore/NCString.h>

namespace nutshell
{

#define VAR_DHCLIENT_PID "/var/run/dhclient.pid"
#define VAR_DHCLIENT_PID_TEMP "/tmp/dhclient.pid~"

#define VAR_DHCPD_PID "/var/run/dhcpd.pid"
#define DHCPD_SERVER_LEASE "/var/db/dhcpd.leases"
#define FIREWALL_SET_SCRIPT "/etc/firewall-script"
#define VAR_DHCPD_CONFIG_FILE "/tmp/dhcpd.conf"
#define DHCLIENT_FLAG_FILE "/tmp/dhclientFlag"

/// Convert IP address from string format to array
#define IPADDR_STRING_TO_ARRAY(arrIp, strIp)    \
    {    \
        INT32 ip[4] = { 0 };    \
        sscanf(strIp.getString(), "%d.%d.%d.%d", ip, ip+1, ip+2, ip+3);    \
        memset(arrIp, 0, 4);    \
        memcpy(arrIp,ip,1);    \
        memcpy(arrIp+1,ip+1,1);    \
        memcpy(arrIp+2,ip+2,1);    \
        memcpy(arrIp+3,ip+3,1);    \
    }    \

/// Convert IP address from array format to string
#define IPADDR_ARRAY_TO_STRING(strIp, arrIp)    \
    {    \
        NCCHAR ip[16];    \
        snprintf(ip, sizeof(ip), "%d.%d.%d.%d", arrIp[0], arrIp[1], arrIp[2], arrIp[3]);    \
        strIp.set(ip);    \
    }    \

    /// Store the network device information
    struct NHNetworkDeviceInfo
    {
        NCString device;          ///< The name of network device
        NCString ip;              ///< The IP address
        NCString mac;             ///< The hardware address
        UINT32 prefixLength;      ///< The prefix length of network mask
        NCString gateway;         ///< The IP address of gateway
        NCString dns1;            ///< The ip of domain name server
        NCString dns2;            ///< The ip of domain name server
        NCString serverId;        ///< The server identifier
    };

    /// Store the connected client information
    struct NHNetworkConnectedClientInfo
    {
        NCString mac;
        NCString ip;
        NCString hostName;
    };

    /// Store the route information
    struct NHNetworkRouteInfo
    {
        NCString destination;     ///< The destination IP address
        UINT32 prefixLength;      ///< The prefix length of network mask
        NCString gateway;         ///< The IP address of gateway
    };

    /// Store the dhclient information
    struct DhclientInfo
    {
        NCString status;           ///< The dhclient status, which includes running, stopped, failed
        NCString ip;               ///< The ip of network device
        NCString gateway;          ///< The gateway of network device
        NCString dns1;             ///< The domain name server
        NCString dns2;             ///< The domain name server
        NCString serverId;         ///< The server identifier
    };

    /// networkhandler timer type
    enum TimerType
    {
        TimerType_Dhclient = 1,     ///< The time type is for dhclient config
    };

} /* namespace nutshell */

#endif /* NHNETWORKDEFINE_H */
/* EOF */
