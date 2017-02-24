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
 * @file NCNetworkHandler.h
 * @brief Declaration file of class NCNetworkHandler.
 *
 * This file includes the declaration of class NCNetworkHandler, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKHANDLER_H
#define NCNETWORKHANDLER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <map>
#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include "NCNetworkManagerDef.h"

namespace nutshell
{

    /**
     * @brief Class of NCNetworkHandler.
     *
     * This class is used to call the function in NINetworkHandler and read config file.
     */
    class NCNetworkHandler
    {
    public:
        NCNetworkHandler();
        virtual ~NCNetworkHandler();
        VOID requestIp(const NCString& deviceName);
        VOID stopRequestIp(const NCString& deviceName);
        VOID releaseIp(const NCString& deviceName);
        VOID startDhcpServer(const NCString& deviceName, const NCString& ip, const UINT32 prefixLength);
        VOID stopDhcpServer(const NCString& deviceName);
        VOID restartDhcpServer(const NCString& deviceName);
        VOID networkDeviceInfo(const NCString& deviceName);
        VOID connectedClientInfo(const NCString& mac);
        VOID startDnsCacheServer();
        VOID stopDnsCacheServer();

        VOID setStaticIp(const NCString& deviceName, const NCString& ip);
        VOID setMask(const NCString& deviceName, const UINT32 prefixLength);
        VOID setMtu(const NCString& deviceName, const UINT32 mtu);
        VOID setStaticIpMaskMtu(const NCString& deviceName, const NCString& ip, const UINT32 prefixLength, const UINT32& mtu);

        VOID addRoute(const RouteInfo& info);
        VOID addDefaultRoute(const NCString& deviceName, const NCString& gateway);
        VOID removeDefaultRoute(const NCString& deviceName);
        VOID updateNamedConf(const NCString& option, const NCString& dns1, const NCString& dns2);
        VOID setNatTable(const NCString& deviceType);
        VOID clearNatTable();

        NC_BOOL readAvcLanIp(const NCString& section, DeviceIpInfo& info);
        VOID readAvcLanDefaultIp(DeviceIpInfo& info);
        VOID readRemoteAvcLanDefaultIp(DeviceIpInfo& info);
        NCString getAvcLanUsbName();
        NC_BOOL readWifiApStaticIp(NCString& ip, UINT32& prefixLength);
        NC_BOOL readAdbDebugIp(DeviceIpInfo& info);
        NC_BOOL checkAvcLanStatus();
        NC_BOOL checkIndividualApparatus();

        // Check the return value of system()
        NC_BOOL checkSystemReturn(const INT status);
        VOID notifyFirewallFailed();

        VOID convertIpStringToArray(const NCString& ip, NCCHAR* array);
        VOID convertIpArrayToString(const NCCHAR* array, NCString& ip);

        VOID getUsbRouteDnsInfo(const NCString& deviceType, DeviceRouteDnsInfo& info);
        NC_BOOL readVRServerRouteInfo(RouteInfo& info);

        UINT32 deviceTypeToNumber(const NCString& deviceType);

        VOID setFirewallForDcm(const NCString& deviceType, const NCString& deviceName);
        VOID clearFirewallForDcm(const NCString& deviceType);

        NC_BOOL isCarryNetworkDevice(const NCString& deviceType);
        NC_BOOL isDummyDcmOn();
        NC_BOOL checkConnectPermission(const NCString& resourceId);

    private:

        NC_BOOL readConfig(const NCString& filePath, const NCString& section, std::map<NCString, NCString>& info);
        NC_BOOL maskToPrefixlength(const NCString& mask, UINT32& prefixLength);

        NCNetworkHandler(const NCNetworkHandler&);
        NCNetworkHandler& operator=(const NCNetworkHandler&);

    };

} /* namespace nutshell */

#endif /* NCNETWORKHANDLER_H */
/* EOF */
