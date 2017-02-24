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
 * @file NCNetworkManagerDef.h
 * @brief Declaration file of class NCNetworkManagerDef.
 *
 */

#ifndef NCNETWORKMANAGERDEF_H
#define NCNETWORKMANAGERDEF_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <evip/NEMessage.h>

namespace nutshell
{

/// Define the work thread name
#define NCNETWORK_REQ_THREAD "network.work"
#define NCNETWORK_NCM_THREAD "network.ncm"
#define NCNETWORK_TIMER_THREAD "network.timer"

/// Defien the section of avclan config file
#define NCNETWORK_AVCLANIP_CONFIG_SECTION "staticIp"
#define NCNETWORK_WIFIAP_CONFIG_SECTION "wifiAp"
#define NCNEWORK_AVCLAN_IP_DCU_SECTION "dcuAvclanIp"
#define NCNEWORK_AVCLAN_IP_MEU_SECTION "meuAvclanIp"
#define NCNETWORK_DUN_WIFISTA_DNS "dunWifiDns"
#define NCNETWORK_DCM_DNS "dcmDns"
#define NCNETWORK_ADB_DEBUG_SECTION "adbDebug"
#define NCNETWORK_VRSERVER_ROUTE_SECTION "VRServerRoute"

/// Define the network device type
#define DCM "dcm"
#define BT_DUN "btDun"
#define BT_PAN "btPan"
#define BT_PHONE "btPhone"
#define WIFI_STA "wifiSta"
#define WIFI_P2P "wifiP2p"
#define WIFI_AP "wifiAp"
#define DSRC "dsrc"
#define USB_AVCLAN "ncm"
#define VR_SERVER "VRServer"

/// Define the path of dhclient pid file and dhcpd pid file
#define DHCPCLIENT_PID_FILE "/var/run/dhclient.pid"
#define DHCPD_PID_FILE "/var/run/dhcpd.pid"
#define NCNETWORK_RESOLV_CONF_ORGIN "/etc/resolv.conf.orgin"
#define NCNETWORK_RESOLV_CONF "/tmp/resolv.conf"

/// Define the path of wifi hotspot traffic file
#define WIFI_AP_TRAFFIC_RX_FILE "/sys/class/net/wlan0/statistics/rx_bytes"
#define WIFI_AP_TRAFFIC_TX_FILE "/sys/class/net/wlan0/statistics/tx_bytes"

/// Define uri for network device signal strength
#define SYSTEM_INFO_URI     "content://system/info"
#define WIFI_STRENGTH_KEY   "wifi/strength"
#define DCM_STRTHGTH_KEY    "dcm/strength"
#define BT_STRTHGTH_KEY     "bt/linkquality"
#define DCM_DEV_Key         "dcm/devstat"
#define BTDUN_DEV_KEY       "bt/hfp-main"
#define INTERNET_STATUS_KEY "internet/status"
#define INTERNET_TYPE_KEY   "internet/type"
#define DCM_STATUS_KEY    "dcm/status"

#define NCNETWORK_AVCLANIP_CONFIG_FILE "/etc/networkmanager/avclanip_config"

#define NCNETWORK_NAMED_DNS_OPTIONS "options"
#define NCNETWORK_GBOOK_DNS_ZONE "g-book.com.cn"

#define NCNETWORK_DHCLIENT_CANCEL_FLAG "dhclient.cancel.flag"

#define WIFI_AP_RESOURCE_ID "wireless#operater#wifitethering"

// Define error type for diag log
#define NCNETWORK_DIAG_REQUEST_IP_FAILED          0x01

// Define WifiService event space and event ID
#define NCNETWORK_WIFI_EVENT_SPACENAME NCString(NCTEXT("NCWIFI"))
#define NCNETWORK_WIFI_EVENT_P2P_DEVICE_DISCONNECTED NCString(NCTEXT("MIRRORLINK_DISCONNECTED"))
#define NCNETWORK_WIFI_EVENT_P2P_DEVICE_CONNECTED NCString(NCTEXT("MIRRORLINK_CONNECTED"))

#define NCNETWORK_BTDUN_CARRIER_INFO_LENGTH 16

    /// Define method ID for notify message
    enum MethodId
    {
        MethodId_Request = NESERVICE_METHODID_INNER,      ///< Method Id for notify request network access result
        MethodId_Release,                                 ///< Method Id for notify release network access result
        MethodId_Disconnect,                              ///< Method Id for notify force disconnect network result
        MethodId_RequestIp,                               ///< Method Id for notify request IP result
        MethodId_ReleaseIp,                               ///< Method Id for notify release IP result
        MethodId_StartDhcpServer,                         ///< Method Id for notify start DHCP server result
        MethodId_StopDhcpServer,                          ///< Method Id for notify stop DHCP server result
        MethodId_DeviceInfo,                              ///< Method Id for notify network device info
        MethodId_ConnectedClientInfo,                     ///< Method Id for notify connected client info
    };


    /// NetworkManager Function ID
    enum FunctionId
    {
        FunctionId_Request = 0,                   ///< Request network access
        FunctionId_RemoteRequest,                 ///< DCM/DUN plugin request network in DCU-MEU mode
        FunctionId_Release,                       ///< Release network access
        FunctionId_Disconnect,                    ///< Force disconnect current network
        FunctionId_DisconnectNetwork,             ///< Disconnect the specified network
        FunctionId_RemoteDisconnect,              ///< DCM/DUN plugin disconnct network in DCU-MEU mode
        FunctionId_DisconnectWithSequenceId,      ///< Disconnect specified network with sequence ID
        FunctionId_SetSlipDevice,                 ///< Set IP, route and mtu for slip device
        FunctionId_NotifySlipDisconnect,          ///< Notify slip device is disconnected
        FunctionId_NotifyDisconnectResult,        ///< Notify dcm/dun disconnect
        FunctionId_RequestIp,                     ///< Request IP address
        FunctionId_RequestIpResourceId,           ///< Request IP address, need resource id
        FunctionId_ReleaseIp,                     ///< Release IP address
        FunctionId_StartDhcpServer,               ///< Start DHCP server
        FunctionId_StopDhcpServer,                ///< Stop DHCP server
        FunctionId_CheckDhcpServerStatus,         ///< Check dhcp server status
        FunctionId_DeviceInfo,                    ///< Get network device info
        FunctionId_ConnectedClientInfo,           ///< Get connected client info
        FunctionId_CurrentUseNetwork,             ///< Get current use network
        FunctionId_DeviceStatus,                  ///< Get current device status
        FunctionId_NetworkSignal,                 ///< Get network signal status
        FunctionId_UpdateGbookServerDns,          ///< Update gbook server dns info
        FunctionId_RequestWithSequenceId,         ///< Request network access with sequence id
        FunctionId_ReleaseWithSequenceId,         ///< Release netowrk access with sequence id
        FunctionId_StartMonitorDataComm,          ///< Start monitor data communication on specified network node
        FunctionId_StopMonitorDataComm,           ///< Stop monitor data communication on specified network node
        FunctionId_HttpClienBegin = 100,          ///< The begin functionId of HttpClient
        FunctionId_RequestHttpClientAccess,       ///< Request HttpClient access Permission
        FunctionId_ReleaseHttpClientAccess,       ///< Release HttpClient access Permission
        FunctionId_SetHttpLogFlag,       ///< Set Http log flag
        FunctionId_GetHttpLogFlag,       ///< Get Http log flag
        FunctionId_HttpClienEnd,                  ///< The end functionId of HttpClient
    };

    /// Store parameters during request network access
    struct AccessParameter
    {
        UINT32 reqId;
        UINT32 result;
        UINT32 reqType;
        NCString resourceId;
        NCString deviceType;
        UINT32 dealType;
        UINT32 commPriority;
        NC_BOOL wifiApFlag;
        NC_BOOL remoteFlag;
        NC_BOOL reqIdFlag;
        DWORD tc;

        AccessParameter()
        : reqId(0)
        , result(NC_NOERROR)
        , reqType(0)
        , resourceId("")
        , deviceType("")
        , dealType(0)
        , commPriority(0)
        , wifiApFlag(NC_FALSE)
        , remoteFlag(NC_FALSE)
        , reqIdFlag(NC_FALSE)
        , tc(0)
        {
            
        }
    };

    /// Store the device IP info  
    struct DeviceIpInfo
    {
        NCString device;              ///< The device name
        NCString ip;                  ///< The IP adderss
        UINT32 prefixLength;          ///< Prefix length of mask
    };

    /// NetworkManager request type
    enum RequestType
    {
        RequestType_Init = 0,
        RequestType_Access = 1,
        RequestType_Disconnect,
        RequestType_Release,
        RequestType_SetDefaultNetwork,
        RequestType_ClearDefaultNetwork,
        RequestType_RequestIp,
        RequestType_StopRequestIp,
        RequestType_ReleaseIp,
        RequestType_StartDhcpServer,
        RequestType_StopDhcpServer,
        RequestType_DeviceInfo,
        RequestType_SetRouteDcuMeu,
        RequestType_SetRouteDhcpWifi,
        RequestType_SetAdbDebugIp,
        RequestType_SetAvcLanIp,
        RequestType_SetSlipDevice,
        RequestType_RestartDhcpServer,
        RequestType_AddRouteVRServer,
        RequestType_NotifyDisconnectResult,
        RequestType_PluginReleaseAccess,
        RequestType_StartMonitorDataComm,
        RequestType_StopMonitorDataComm
    };

    /// NetworkManager timer type
    enum TimerType
    {
        TimerType_DcuMeuSync = 1,
        TimerType_AvcLanIp,
        TimerType_WifiAp,
        TimerType_RequestMap,
    };
    
    /// The type when run firewall script
    enum FirewallType
    {
        FirewallType_Default = 1,
        FirewallType_Ncm,
        FirewallType_DcuMeu,
        FirewallType_DcuOnly,
        FirewallType_DefaultDcuMeu,
        FirewallType_DefaultDcuOnly,
        FirewallType_ClearDefault,
        FirewallType_ClearNcm,
        FirewallType_InitFirewall,
        FirewallType_Dcm,
    };

    struct DeviceRouteDnsInfo
    {
        NCString deviceType;
        NCString deviceName;
        NCString gateway;
        NCString dns1;
        NCString dns2;
    };

    /// WiFi AP timer flag type
    enum WifiApTimerFlag
    {
        WifiApTimerFlag_OnTimer = 1,
    };

    /// Configure route info
    struct RouteInfo
    {
        NCString deviceName;
        NCString ip;
        UINT32 prefixLength;
        NCString gateway;
    };

    /// Define NetworkManager status
    enum NetworkManagerStatus
    {
        NetworkManagerStatus_Unknown = 1,
        NetworkManagerStatus_OnInitStart,
        NetworkManagerStatus_OnInitEnd,
        NetworkManagerStatus_OnHibernateStart,
        NetworkManagerStatus_OnHibernateEnd,
        NetworkManagerStatus_onAwakeStart,
        NetworkManagerStatus_onAwakeEnd,
        NetworkManagerStatus_onStopStart,
        NetworkManagerStatus_onStopEnd,
        NetworkManagerStatus_onDeinitStart,
        NetworkManagerStatus_onDeinitEnd,
    };

    // Define wait object type
    enum WaitObjType
    {
        WaitObjType_PluginCallback = 1,
        WaitObjType_WifiRoute,
    };

    // Define current requests map type
    enum RequestMapType 
    {
        RequestMapType_Init = 1,
        RequestMapType_Callback,
    };

    enum PopResultFlag
    {
        PopResultFlag_Init = 0,
        PopResultFlag_Interrupt = 1,
        PopResultFlag_Timeout = 2,
        PopResultFlag_Failed = 3,
    };

    // Define request state
    enum
    {
        REQ_INIT = 0, 
        REQ_TIMEOUT = 1, 
        REQ_CONFIRMED = 2,
        REQ_DIALSTART = 4,
        REQ_DIALEND = 8,
    };

} /* namespace nutshell */

#endif /* NCNETWORKMANAGERDEF_H */
/* EOF */
