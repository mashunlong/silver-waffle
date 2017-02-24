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
 * @file NCNetworkDef.h
 * @brief Declaration file of class NCNetworkDef.
 *
 * This file includes the declaration of class NCNetworkDef, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKDEF_H
#define NCNETWORKDEF_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <system/PfErrorCodeDef.h>

namespace nutshell
{
// notify the network device state is connected
#define NCNETWORK_EVENT_CONNECT "network_connect"

// notify the network device state is disconnected
#define NCNETWORK_EVENT_DISCONNECT "network_disconnect"

// notify the network route state changed
#define NCNETWORK_EVENT_ROUTECHANGED "network_route_changed"

// notify the start and end for network connect or disconnect
#define NCNETWORK_EVENT_DIAL_STATE "network_dial_state"

// notify the start and end for dcm/dun link connect and disconnect
// this event only broadcast in dcu/meu that has dcm/dun network device
#define NCNETWORK_EVENT_LINK_STATE "network_link_state"

// notify firewall configure failed
#define NCNETWORK_EVENT_FIREWALL_FAILED "network_firewall_failed"

// notify the NCM netowrk between dcu and meu is available
#define NCNETWORK_EVENT_NCM_IP_FINISHED "network_ncm_ip_finished"

// notify the network devcie has sinnal or has no signal
#define NCNETWORK_EVENT_HAVESIGNAL "network_have_signal"
#define NCNETWORK_EVENT_NOSIGNAL "network_no_signal"

/// Define network error code
#define  NCNETWORK_ERRSTART NCERRORCODEP(NC_ERRORCODE_PLATFORM_SUB_MODULE_NETWORKMANAGER, 0)
#define  NCNETWORK_PLG_DCM_ERRSTART  ((NCNETWORK_ERRSTART) + 0x0100)
#define  NCNETWORK_PLG_DUN_ERRSTART  ((NCNETWORK_ERRSTART) + 0x0140)
    enum NetworkErrCode
    {
        NetworkErrCode_Success = NC_NOERROR,           ///< Result indicates success
        NetworkErrCode_Fail = NCNETWORK_ERRSTART,      ///< Result indicates fail
        NetworkErrCode_Timeout,                        ///< Result indicates timeout
        NetworkErrCode_HttpClientMgr_TaskNumFull,      ///< Socket num reach max of task
        NetworkErrCode_HttpClientMgr_TotalNumFull,     ///< Socket num reach max of total
        NetworkErrCode_HttpClientMgr_TaskUnknow,       ///< Task is not found in access list
        NetworkErrCode_AvcLanIpConfig_OpenFailed,      ///< Open /etc/networkmanager/avclanip_config failed
        NetworkErrCode_DhcpServer_Exist,               ///< DHCP server is existed and started failed
        NetworkErrCode_HttpClientMgr_FileFlgError, ///< lighttpd log flag get/set error
        NetworkErrCode_Firewall_StartFailed,           ///< Start Firewall failed
        NetworkErrCode_DcmDunPlugin_pppdFailed,        ///< dcm/dun plugin dial failed
        NetworkErrCode_DcmDunPlugin_DisconnectFailed,  ///< dcm/dun disconnect link failed
        NetworkErrCode_VerifyPermissionFailed,         ///< verify permission failed
        NetworkErrCode_OpenFileFailed,                 ///< open config file failed
        NetworkErrCode_ReadFileFailed,                 ///< Read file failed
        NetworkErrCode_RequestIpFailed,                ///< Request IP failed
        NetworkErrCode_ReleaseIpFaile,                 ///< Release IP failed
        NetworkErrCode_StartDhcpServerFailed,          ///< Start dhcp server failed
        NetworkErrCode_StopDhcpServerFailed,           ///< Stop dhcp server failed
        NetworkErrCode_PointerIsNULL,                  ///< Pointer is null
        NetworkErrCode_InterruptByDisconnect,          ///< Interrupt by disconnect

        NetworkErrCode_PlgDcm_UserCancel = NCNETWORK_PLG_DCM_ERRSTART,  ///< connect canceled by disconnect or user
        NetworkErrCode_PlgDcm_CarStsInvalid,           ///< car is driving can not do connection
        NetworkErrCode_PlgDcm_DcmNotPlugined,          ///< DCM device is not plugged
        NetworkErrCode_PlgDcm_DcmRegulation,           ///< DCM is regulation can not do connection
        NetworkErrCode_PlgDcm_DcmPhoneCalling,         ///< DCM phone is calling can not do connection
        NetworkErrCode_PlgDcm_DcmSecurityCalling,      ///< DCM security calling can not do connection
        NetworkErrCode_PlgDcm_DcmOutOfSerice,          ///< DCM is out of circle
        NetworkErrCode_PlgDcm_DcmComInvalid,           ///< DCM data communication is invalid
        NetworkErrCode_PlgDcm_DcmAtError,              ///< DCM send AT command fail
        NetworkErrCode_PlgDcm_DcmUsbError,             ///< DCM send USB command fail
        NetworkErrCode_PlgDcm_PppdAuthError,           ///< pppd authenticate failed
        NetworkErrCode_PlgDcm_PppdError,               ///< pppd other errors
        NetworkErrCode_PlgDcm_TimeOut,                 ///< connection time out
        NetworkErrCode_PlgDcm_OtherError,              ///< DCM connection other error

        NetworkErrCode_PlgDun_UserCancel  = NCNETWORK_PLG_DUN_ERRSTART, ///< connect canceled by disconnect or user
        NetworkErrCode_PlgDun_CarStsInvalid,           ///< car is driving can not do connection
        NetworkErrCode_PlgDun_OutOfService,            ///< DUN phone is out of circle
        NetworkErrCode_PlgDun_BtDialing,               ///< BT phone is dialing
        NetworkErrCode_PlgDun_BtPhoneDisconnected,     ///< BT phone is disconnected
        NetworkErrCode_PlgDun_BtPbUpdate,              ///< BT phonebook is updating
        NetworkErrCode_PlgDun_PppdAuthError,           ///< pppd authenticate failed
        NetworkErrCode_PlgDun_PppdError,               ///< pppd other errors
        NetworkErrCode_PlgDun_TimeOut,                 ///< connection time out
        NetworkErrCode_PlgDun_OtherError,              ///< DUN connection other error

    };

    /// Define the dial type when access network by DCM/BTDUN
    enum DialType
    {
        DialType_Auto = 1,        ///< The dial type is auto
        DialType_Manual = 2,      ///< The dial type is manual
        DialType_HalfAuto = 3,    ///< The dial type is half auto
        DialType_ConfirmAuto = 4, ///< The dial type is confirm auto
        DialType_None = 5,        ///< The dial type is none
    };

    /// Define the priority when access network by DCM/BTDUN
    enum CommunicatePriority
    {
        CommunicatePriority_Oss = 1,      ///< OSS
        CommunicatePriority_Manual = 2,   ///< Manual communication
        CommunicatePriority_AutoHigh = 3, ///< High auto communication
        CommunicatePriority_AutoLow = 5,  ///< Low auto communication
        CommunicatePriority_None = 4,     ///< None communication
    };

    /// Define the access network mode that includes dial type and communicate priority
    struct AccessMode
    {
        DialType type;                    ///< The dial type
        CommunicatePriority priority;     ///< The communication priority
        NCString resourceId;              ///< The resource identify
    };

    struct RequestPluginResult
    {
        UINT32 reqId;
        UINT32 result;
        NCString deviceType;
        NCString deviceName;
        NCString gateway;
        NCString dns1;
        NCString dns2;
    };

    /// Define the dial state of dcm/dun
    enum DialState
    {
        DialState_Init = 1,       ///< The init state
        DialState_Connecting,     ///< Start dial for dcm/dun
        DialState_Connected,      ///< Dcm/dun is connected
        DialState_Disconnecting,  ///< Start disconnect for dcm/dun
        DialState_Disconnected,   ///< Dcm/dun is disconnected
    };

} /* namespace nutshell */

#endif /* NCNETWORKDEF_H */
/* EOF */
