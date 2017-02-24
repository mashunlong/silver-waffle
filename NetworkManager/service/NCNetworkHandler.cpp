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
#include <ncore/NCError.h>
#include <dlfcn.h>
#include <ncore/NCFile.h>
#include <ncore/NCIniFile.h>
#include <ncore/NCIniFileParser.h>
#include <netutils/ifc.h>
#include <nhinterface/NIAvcLanHandler.h>
#include <nhinterface/NINetworkHandler.h>
#include <nhinterface/NIPnpHandler.h>
#include <connectmanager/NCConnect.h>
#include <contentresolver/NCContentResolver.h>
#include <system/NCSystem.h>
#include <system/NCEnvironment.h>
#include "NCNetworkManagerDef.h"
#include "NCNetworkDeviceType.h"
#include "NCNetworkStatus.h"
#include "NCNetworkManager.h"
#include "NCNetworkHandler.h"

#define NC_LOG_TAG "NCNetworkManager"
#define NCNETWORK_DEFAULT_DNS "114.114.114.114"
#define NCNETWORK_DUMMY_DCM_KEY "content://settings/developer?dummy_dcm"

namespace nutshell
{

    NCNetworkHandler::NCNetworkHandler()
    {

    }

    NCNetworkHandler::~NCNetworkHandler()
    {

    }

    VOID
    NCNetworkHandler::requestIp(const NCString& deviceName)
    {
        NCLOGD("NCNetworkHandler::requestIp - Start, deviceName [%s]", deviceName.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->doDhcpRequest(deviceName);
        }
        else {
            NCLOGD("NCNetworkHandler::requestIp - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::stopRequestIp(const NCString& deviceName)
    {
        NCLOGD("NCNetworkHandler::stopRequestIp - Start, deviceName [%s]", deviceName.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->stopDhcpRequest(deviceName);
        }
        else {
            NCLOGD("NCNetworkHandler::stopRequestIp - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::releaseIp(const NCString& deviceName)
    {
        NCLOGD("NCNetworkHandler::releaseIp - Start, deviceName [%s]", deviceName.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->releaseDhcpLease(deviceName);
        }
        else {
            NCLOGD("NCNetworkHandler::releaseIp - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::startDhcpServer(const NCString& deviceName, const NCString& ip, const UINT32 prefixLength)
    {
        NCLOGD("NCNetworkHandler::startDhcpServer - Start, deviceName [%s], ip [%s], prefixLength [%d]", deviceName.getString(), ip.getString(), prefixLength);
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->startDhcpServer(deviceName, ip, prefixLength);
        }
        else {
            NCLOGD("NCNetworkHandler::startDhcpServer - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::stopDhcpServer(const NCString& deviceName)
    {
        NCLOGD("NCNetworkHandler::stopDhcpServer - Start, deviceName [%s]", deviceName.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->stopDhcpServer(deviceName);
        }
        else {
            NCLOGD("NCNetworkHandler::stopDhcpServer - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::networkDeviceInfo(const NCString& deviceName)
    {
        NCLOGD("NCNetworkHandler::networkDeviceInfo - Start, deviceName [%s]", deviceName.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->networkDeviceInfo(deviceName);
        }
        else {
            NCLOGD("NCNetworkHandler::networkDeviceInfo - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::connectedClientInfo(const NCString& mac)
    {
        NCLOGD("NCNetworkHandler::connectedClientInfo - Start, mac = [%s]", mac.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->connectedClientInfo(mac);
        }
        else {
            NCLOGD("NCNetworkHandler::connectedClientInfo - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::startDnsCacheServer()
    {
        NCLOGD("NCNetworkHandler::startDnsCacheServer - Start");
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->startDnsCacheServer();
        }
        else {
            NCLOGD("NCNetworkHandler::startDnsCacheServer - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::stopDnsCacheServer()
    {
        NCLOGD("NCNetworkHandler::stopDnsCacheServer - Start");
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->stopDnsCacheServer();
        }
        else {
            NCLOGD("NCNetworkHandler::stopDnsCacheServer - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::restartDhcpServer(const NCString& deviceName)
    {
        NCLOGD("NCNetworkHandler::restartDhcpServer - Start, deviceName [%s]", deviceName.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->restartDhcpServer(deviceName);
        }
        else {
            NCLOGD("NCNetworkHandler::restartDhcpServer - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::setStaticIp(const NCString& deviceName, const NCString& ip)
    {
        NCLOGD("NCNetworkHandler::setStaticIp - Start, deviceName [%s], ip [%s]", deviceName.getString(), ip.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->setStaticIp(deviceName, ip);
        }
        else {
            NCLOGD("NCNetworkHandler::setStaticIp - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::setMask(const NCString& deviceName, const UINT32 prefixLength)
    {
        NCLOGD("NCNetworkHandler::setMask - Start, deviceName [%s], prefixLength [%d]", deviceName.getString(), prefixLength);
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->setMask(deviceName, prefixLength);
        }
        else {
            NCLOGD("NCNetworkHandler::setMask - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::setMtu(const NCString& deviceName, const UINT32 mtu)
    {
        NCLOGD("NCNetworkHandler::setMtu - Start, deviceName [%s], mtu [%d]", deviceName.getString(), mtu);
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->setMtu(deviceName, mtu);
        }
        else {
            NCLOGD("NCNetworkHandler::setMtu - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::setStaticIpMaskMtu(const NCString& deviceName, const NCString& ip, const UINT32 prefixLength, const UINT32& mtu)
    {
        NCLOGD("NCNetworkHandler::setStaticIpMaskMtu, Start, deviceName [%s], ip [%s], prefixLength [%d], mtu [%d]", deviceName.getString(), ip.getString(), prefixLength, mtu);
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->setStaticIpMaskMtu(deviceName, ip, prefixLength, mtu);
        }
        else {
            NCLOGD("NCNetworkHandler::setStaticIpMaskMtu, Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::addRoute(const RouteInfo& info)
    {
        NCLOGD("NCNetworkHandler::addRoute - Start, deviceName [%s], destination IP [%s], prefixLength [%d], gateway [%s]", 
            info.deviceName.getString(), info.ip.getString(), info.prefixLength, info.gateway.getString());
        NINetworkHandler::RouteInfo tempInfo;
        tempInfo.prefixLength = info.prefixLength;

        NCCHAR dstIp[4] = { 0 };
        NCCHAR dstGateway[4] = { 0 };
        convertIpStringToArray(info.ip, dstIp);
        convertIpStringToArray(info.gateway, dstGateway);
        memcpy(tempInfo.destination, dstIp, 4);
        memcpy(tempInfo.gateway, dstGateway, 4);

        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->addRoute(info.deviceName, tempInfo);
        }
        else {
            NCLOGD("NCNetworkHandler::addRoute - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::addDefaultRoute(const NCString& deviceName, const NCString& gateway)
    {
        NCLOGD("NCNetworkHandler::addDefaultRoute - Start, deviceName [%s], gateway [%s]", deviceName.getString(), gateway.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->addDefaultRoute(deviceName, gateway);
        }
        else {
            NCLOGD("NCNetworkHandler::addDefaultRoute - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::removeDefaultRoute(const NCString& deviceName)
    {
        NCLOGD("NCNetworkHandler::removeDefaultRoute - Start, deviceName [%s]", deviceName.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->removeDefaultRoute(deviceName);
        }
        else {
            NCLOGD("NCNetworkHandler::removeDefaultRoute - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::updateNamedConf(const NCString& option, const NCString& dns1, const NCString& dns2)
    {
        NCLOGD("NCNetworkHandler::updateNamedConf, Start, option [%s], dns1 [%s], dns2 [%s]", option.getString(), dns1.getString(), dns2.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->updateNamedConf(option, dns1, dns2);
        }
        else {
            NCLOGD("NCNetworkHandler::updateNamedConf - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::setNatTable(const NCString& deviceType)
    {
        NCLOGD("NCNetworkHandler::setNatTable - Start, deviceType [%s]", deviceType.getString());
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL == netInterface) {
            NCLOGD("NCNetworkHandler::setNatTable - Get interface failed");
        }

        NCString deviceName;
        NCNetworkManager::Instance()->getNetworkDeviceType()->getName(deviceType, deviceName);
        NCLOGD("NCNetworkHandler::setNatTable, Device name [%s]", deviceName.getString());

        if (DCM == deviceType) {
            netInterface->setNatTable(NINetworkHandler::NatSetMode_DcuAp, deviceName);
        }
        else if (WIFI_STA == deviceType || BT_DUN == deviceType) {
#if defined NCNETWORK_TARGET_PRODUCT_DCU
            netInterface->setNatTable(NINetworkHandler::NatSetMode_DcuWifi, deviceName);
#endif
        } 
        else {
            NCLOGD("NCNetworkHandler::setNatTable - Device type is not defined");
        }
    }

    VOID
    NCNetworkHandler::clearNatTable()
    {
        NCLOGD("NCNetworkHandler::clearNatTable - Start");
        NCCHAR cmd[128] = { 0 };
        snprintf(cmd, sizeof(cmd)-1, "iptables -t nat -F");
        system(cmd);
    }

    NC_BOOL
    NCNetworkHandler::readAvcLanIp(const NCString& section, DeviceIpInfo& info)
    {
        NCLOGD("NCNetworkHandler::readAvcLanIp - Start");
        NCString filePath = NCNETWORK_AVCLANIP_CONFIG_FILE;

        std::map<NCString, NCString> infoMap;
        if (!readConfig(filePath, section, infoMap)) {
            NCLOGD("NCNetworkHandler::readAvcLanIp, Read config file failed");
            return NC_FALSE;
        }

        std::map<NCString, NCString>::iterator iter;
#if defined NCNETWORK_TARGET_PRODUCT_MEU
        iter = infoMap.find(NCString("device"));
        if (iter != infoMap.end()) {
            info.device = iter->second;
        }
        else {
            NCLOGD("NCNetworkHandler::readAvcLanIp - Find device name failed");
            return NC_FALSE;
        }
#endif

        iter = infoMap.find(NCString("ip"));
        if (iter != infoMap.end()) {
            info.ip = iter->second;
        }
        else {
            NCLOGD("NCNetworkHandler::readAvcLanIp - Find ip failed");
            return NC_FALSE;
        }

        iter = infoMap.find(NCString("mask"));
        if (iter != infoMap.end()) {
            NCString mask = iter->second;
            UINT32 prefixLength;
            if (maskToPrefixlength(mask, prefixLength)) {
                info.prefixLength = prefixLength;
            }
            else {
                NCLOGD("NCNetworkHandler::readAvcLanIp - Convert mask to prefixLength failed");
                return NC_FALSE;
            }
        }
        else {
            NCLOGD("NCNetworkHandler::readAvcLanIp - Find mask failed");
            return NC_FALSE;
        }

        return NC_TRUE;
    }

    VOID
    NCNetworkHandler::readAvcLanDefaultIp(DeviceIpInfo& info)
    {
#if defined NCNETWORK_TARGET_PRODUCT_EMULATOR
        NCLOGD("NCNetworkHandler::readAvcLanDefaultIp, Emulator");
        return;
#endif

#if defined NCNETWORK_TARGET_PRODUCT_DCU
        NCLOGD("NCNetworkHandler::readAvcLanDefaultIp, Dcu");
        info.ip = "192.168.128.129";
        info.prefixLength = 30;
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
        NCLOGD("NCNetworkHandler::readAvcLanDefaultIp, Meu");
        info.device = "gadncm0";
        info.ip = "192.168.128.130";
        info.prefixLength = 30;
#endif
    }

    VOID
    NCNetworkHandler::readRemoteAvcLanDefaultIp(DeviceIpInfo& info)
    {
#if defined NCNETWORK_TARGET_PRODUCT_EMULATOR
        NCLOGD("NCNetworkHandler::readRemoteAvcLanDefaultIp, Emulator");
        return;
#endif

#if defined NCNETWORK_TARGET_PRODUCT_DCU
        NCLOGD("NCNetworkHandler::readRemoteAvcLanDefaultIp, DCU");
        info.device = "gadncm0";
        info.ip = "192.168.128.130";
        info.prefixLength = 30;
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
        NCLOGD("NCNetworkHandler::readRemoteAvcLanDefaultIp, MEU");
        info.device = "usb0";
        info.ip = "192.168.128.129";
        info.prefixLength = 30;
#endif
    }

    NCString
    NCNetworkHandler::getAvcLanUsbName()
    {
        NCLOGD("NCNetworkHandler::getAvcLanUsbName, Start");
        NCString usbName;
        NIPnpHandler* netInterface = GET_NIINTERFACE(NIPnpHandler);
        if (NULL != netInterface) {
            NIPnpHandler::UsbDevInfo info;
            netInterface->getUsbDeviceInfo(NIPnpHandler::UsbDevId_Meu, &info);
            usbName = info.meu.ncm;
        }
        else {
            NCLOGD("NCNetworkHandler::getAvcLanUsbName - Get NIPnpHandler pointer failed");
            usbName = "";
        }

        NCLOGD("NCNetworkHandler::getAvcLanUsbName. usbName [%s]", usbName.getString());
        return usbName;
    }


    NC_BOOL
    NCNetworkHandler::readWifiApStaticIp(NCString& ip, UINT32& prefixLength)
    {
        NCLOGD("NCNetworkHandler::readWifiApStaticIp - Start");
        NCString filePath = NCNETWORK_AVCLANIP_CONFIG_FILE;
        NCString section = NCNETWORK_WIFIAP_CONFIG_SECTION;
        std::map<NCString, NCString> infoMap;
        if (!readConfig(filePath, section, infoMap)) {
            NCLOGD("NCNetworkHandler::readWifiApStaticIp - Read config failed");
            return NC_FALSE;
        }


        std::map<NCString, NCString>::iterator iter = infoMap.find(NCString("ip"));
        if (iter != infoMap.end()) {
            ip = iter->second;
        }
        else {
            NCLOGD("NCNetworkHandler::readWifiApStaticIp - Find ip failed");
            return NC_FALSE;
        }

        iter = infoMap.find(NCString("mask"));
        if (iter != infoMap.end()) {
            NCString mask = iter->second;
            if (!maskToPrefixlength(mask, prefixLength)) {
                NCLOGD("NCNetworkHandler::readWifiApStaticIp - Mask to prefixLength failed");
                return NC_FALSE;
            }
        }
        else {
            NCLOGD("NCNetworkHandler::readWifiApStaticIp - Find mask failed");
            return NC_FALSE;
        }

        return NC_TRUE;
    }

    NC_BOOL
    NCNetworkHandler::readAdbDebugIp(DeviceIpInfo& info)
    {
        NCLOGD("NCNetworkHandler::readAdbDebugIp, Start");
        std::map<NCString, NCString> infoMap;
        if (!readConfig(NCNETWORK_AVCLANIP_CONFIG_FILE, NCNETWORK_ADB_DEBUG_SECTION, infoMap)) {
            NCLOGD("NCNetworkHandler::readAdbDebugIp - ReadConfig return failed");
            return NC_FALSE;
        }

        std::map<NCString, NCString>::iterator deviceIter = infoMap.find(NCString("device"));
        if (deviceIter != infoMap.end()) {
            info.device = deviceIter->second;
        }
        else {
            NCLOGD("NCNetworkHandler::readAdbDebugIp - Find device failed");
            return NC_FALSE;
        }

        std::map<NCString, NCString>::iterator ipIter = infoMap.find(NCString("ip"));
        if (ipIter != infoMap.end()) {
            info.ip = ipIter->second;
        }
        else {
            NCLOGD("NCNetworkHandler::readAdbDebugIp - Find ip failed");
            return NC_FALSE;
        }

        std::map<NCString, NCString>::iterator maskIter = infoMap.find(NCString("mask"));
        if (maskIter != infoMap.end()) {
            NCString mask = maskIter->second;
            if (!maskToPrefixlength(mask, info.prefixLength)) {
                NCLOGD("NCNetworkHandler::readWifiApStaticIp - MaskToPrefixlength return failed");
                return NC_FALSE;
            }
        }
        else {
            NCLOGD("NCNetworkHandler::readWifiApStaticIp - Find mask failed");
            return NC_FALSE;
        }

        return NC_TRUE;
    }

    NC_BOOL
    NCNetworkHandler::checkAvcLanStatus()
    {
        NCLOGD("NCNetworkHandler::checkAvcLanStatus - Start");
        NIAvcLanHandler* avcHandler = GET_NIINTERFACE(NIAvcLanHandler);
        if (NULL == avcHandler) {
            NCLOGD("NCNetworkHandler::checkAvcLanStatus - Get interface failed");
            return NC_FALSE;
        }

        NIAvcLanHandler::ConnectStatus status;
        avcHandler->queryConnectStatus(&status);
        if ((status == NIAvcLanHandler::ConnectStatus_AuthenticationSuccessful) || (status == NIAvcLanHandler::ConnectStatus_CommandEnabled)) {
            NCLOGD("NCNetworkHandler::checkAvcLanStatus - Status is success");
            return NC_TRUE;
        }
        else {
            NCLOGD("NCNetworkManager::queryConnectStatus - Status is failed");
            return NC_FALSE;
        }
    }

    NC_BOOL
    NCNetworkHandler::checkIndividualApparatus()
    {
        NCLOGD("NCNetworkHandler::checkIndividualApparatus, Start");
        NIAvcLanHandler* avcHandler = GET_NIINTERFACE(NIAvcLanHandler);
        if (NULL == avcHandler) {
            NCLOGD("NCNetworkHandler::checkIndividualApparatus - Get interface failed");
            return NC_FALSE;
        }

        NC_BOOL status;
        avcHandler->queryIndividualApparatus(&status);
        if (status) {
            NCLOGD("NCNetworkHandler::checkIndividualApparatus - DCU is Individual apparatus");
            return NC_TRUE;
        }
        else {
            NCLOGD("NCNetworkManager::checkIndividualApparatus - DCU is not Individual apparatus");
            return NC_FALSE;
        }
    }

    NC_BOOL
    NCNetworkHandler::readConfig(const NCString& filePath, const NCString& section, std::map<NCString, NCString>& info)
    {
        NCLOGD("NCNetworkManager::readConfig - Start");
        NCIniFile iniFileOperator;
        NC_BOOL ret = iniFileOperator.open(filePath.getString(), NCINI_OM_R);
        if (!ret) {
            NCLOGE("NCNetworkHandler::readConfig - Open file failed!");
            return NC_FALSE;
        }

        NCIniIterator iter = iniFileOperator.iterator(section);
        for (; !iter.end(); ++iter) {
            NCIniKey key;
            if (iter.current(key)) {
                NCString keyName = key.name();
                NCString keyValue = key.value();
                info.insert(std::map<NCString, NCString>::value_type(keyName, keyValue));
            }
        }

        iniFileOperator.close();
        return NC_TRUE;
    }


    NC_BOOL
    NCNetworkHandler::maskToPrefixlength(const NCString& mask, UINT32& prefixLength)
    {
        NCLOGD("NCNetworkHandler::maskToPrefixlength - Start, mask = [%s]", mask.getString());
        struct in_addr ip_addr;
        UINT32 netMask = 0;
        if (inet_aton(mask.getString(), &ip_addr)) {
            netMask = ntohl(ip_addr.s_addr);
        }
        else {
            NCLOGD("NCNetworkHandler::maskToPrefixlength - inet aton failed");
            return NC_FALSE;
        }

        UINT32 count = 0;
        while (0 == (netMask & 0x01) && count < 32) {
            count++;
            netMask = netMask >> 1;
        }

        prefixLength = 32 - count;
        NCLOGD("NCNetworkHandler::maskToPrefixlength - Success, prefixLength = [%d]", prefixLength);
        return NC_TRUE;
    }


    NC_BOOL
    NCNetworkHandler::checkSystemReturn(const INT status)
    {
        NCLOGD("NCNetworkHandler::checkSystemReturn, Start, status [%d]", status);
        if (-1 == status) {
            NCLOGD("NCNetworkHandler::checkSystemReturn, System return error");
            return NC_FALSE;
        }
        else {
            if (WIFEXITED(status)) {
                if (0 == WEXITSTATUS(status)) {
                    NCLOGD("NCNetworkHandler::checkSystemReturn, System return success");
                    return NC_TRUE;
                }
                else {
                    NCLOGD("NCNetworkHandler::checkSystemReturn, System reutrn error1: [%d]", WEXITSTATUS(status));
                    return NC_FALSE;
                }
            }
            else {
                NCLOGD("NCNetworkHandler::checkSystemReturn, System reutrn error2: [%d]", WIFEXITED(status));
                return NC_FALSE;
            }
        }
    }

    VOID
    NCNetworkHandler::notifyFirewallFailed()
    {
        NCLOGD("NCNetworkHandler::notifyFirewallFailed, Start");
        NINetworkHandler* netInterface = GET_NIINTERFACE(NINetworkHandler);
        if (NULL != netInterface) {
            netInterface->notifyFirewallFailed();
        }
        else {
            NCLOGD("NCNetworkHandler::notifyFirewallFailed - Get interface failed");
        }
    }

    VOID
    NCNetworkHandler::convertIpStringToArray(const NCString& ip, NCCHAR* array)
    {
        NCLOGD("NCNetworkHandler::convertIpStringToArray, Start ip [%s]", ip.getString());
        INT32 tempArray[4] = { 0 };
        sscanf(ip.getString(), "%5d.%5d.%5d.%5d", tempArray, tempArray+1, tempArray+2, tempArray+3);
        memcpy(array, tempArray, 1);
        memcpy(array+1, tempArray+1, 1);
        memcpy(array+2, tempArray+2, 1);
        memcpy(array+3, tempArray+3, 1);
        NCLOGD("NCNetworkHandler::convertIpStringToArray, End array [%x] [%x] [%x] [%x]", array[0], array[1], array[2], array[3]);
    }

    VOID
    NCNetworkHandler::convertIpArrayToString(const NCCHAR* array, NCString& ip)
    {
        NCLOGD("NCNetworkHandler::convertIpArrayToString, Start, array [%x] [%x] [%x] [%x]", array[0], array[1], array[2], array[3]);
        NCCHAR tempIp[16];
        snprintf(tempIp, sizeof(tempIp), "%d.%d.%d.%d", array[0], array[1], array[2], array[3]);
        ip.set(tempIp);
        NCLOGD("NCNetworkHandler::convertIpArrayToString, End, ip [%s]", ip.getString());
    }


    VOID
    NCNetworkHandler::getUsbRouteDnsInfo(const NCString& deviceType, DeviceRouteDnsInfo& info)
    {
        NCString tempSection;
        if (DCM == deviceType) {
            tempSection = NCNEWORK_AVCLAN_IP_MEU_SECTION;
        }
        else if (WIFI_STA == deviceType || BT_DUN == deviceType) {
            tempSection = NCNEWORK_AVCLAN_IP_DCU_SECTION;
        }
        else {
            NCLOGD("NCNetworkHandler::getUsbRouteDnsInfo, Bug: Parameters error");
            return;
        }

        DeviceIpInfo remoteInfo;
        NCNetworkHandler handler;
        handler.readAvcLanIp(tempSection, remoteInfo);
        DeviceIpInfo localInfo = NCNetworkManager::Instance()->getAvcLanIpInfo();

        info.deviceType = deviceType;
        info.deviceName = localInfo.device;
        info.gateway = remoteInfo.ip;
        info.dns1 = remoteInfo.ip;
        info.dns2 = "";
    }

    NC_BOOL
    NCNetworkHandler::readVRServerRouteInfo(RouteInfo& info)
    {
        NCLOGD("NCNetworkHandler::readVRServerRouteInfo, Start");
        std::map<NCString, NCString> infoMap;
        if (!readConfig(NCNETWORK_AVCLANIP_CONFIG_FILE, NCNETWORK_VRSERVER_ROUTE_SECTION, infoMap)) {
            NCLOGD("NCNetworkHandler::readVRServerRouteInfo, Read config file failed");
            return NC_FALSE;
        }

        std::map<NCString, NCString>::iterator iter;
        iter = infoMap.find(NCString("device"));
        if (iter != infoMap.end()) {
            info.deviceName = iter->second;
        }
        else {
            NCLOGD("NCNetworkHandler::readVRServerRouteInfo - Find device name failed");
            return NC_FALSE;
        }

        iter = infoMap.find(NCString("ip"));
        if (iter != infoMap.end()) {
            info.ip = iter->second;
        }
        else {
            NCLOGD("NCNetworkHandler::readVRServerRouteInfo - Find ip failed");
            return NC_FALSE;
        }

        iter = infoMap.find(NCString("mask"));
        if (iter != infoMap.end()) {
            NCString mask = iter->second;
            UINT32 prefixLength;
            if (maskToPrefixlength(mask, prefixLength)) {
                info.prefixLength = prefixLength;
            }
            else {
                NCLOGD("NCNetworkHandler::readVRServerRouteInfo - Convert mask to prefixLength failed");
                return NC_FALSE;
            }
        }
        else {
            NCLOGD("NCNetworkHandler::readVRServerRouteInfo - Find mask failed");
            return NC_FALSE;
        }

        iter = infoMap.find(NCString("gateway"));
        if (iter != infoMap.end()) {
            info.gateway = iter->second;
        }
        else {
            NCLOGD("NCNetworkHandler::readVRServerRouteInfo - Find gateway failed");
            return NC_FALSE;
        }

        return NC_TRUE;
    }

    UINT32
    NCNetworkHandler::deviceTypeToNumber(const NCString& deviceType)
    {
        NCLOGD("NCNetworkHandler::deviceTypeToNumber, Start");
        if (DCM == deviceType) {
            return 1;
        }
        else if (BT_DUN == deviceType) {
            return 2;
        }
        else if (WIFI_STA == deviceType) {
            return 3;
        }
        else if (WIFI_P2P == deviceType) {
            return 4;
        }
        else if (WIFI_AP == deviceType) {
            return 5;
        }
        else if (USB_AVCLAN == deviceType) {
            return 6;
        }
        else {
            return 0;
        }
    }

    VOID
    NCNetworkHandler::setFirewallForDcm(const NCString& deviceType, const NCString& deviceName)
    {
        NCLOGD("NCNetworkHandler::setFirewallForDcm, Start, deviceType [%s], deviceName [%s]", deviceType.getString(), deviceName.getString());
        NC_BOOL flag = NCNetworkManager::Instance()->getFirewallFlag(FirewallType_Dcm);
        NCLOGD("NCNetworkHandler::setFirewallForDcm, Dcm firewall flag [%d]", flag);
        if (DCM == deviceType && isCarryNetworkDevice(deviceType) && (!flag)) {
            NCEnvironment env;
            NCString country;
            env.getProductCountry(country);
            NCLOGD("NCNetworkHandler::setFirewallForDcm, country [%s]", country.getString());
            if (NCString("America") == country) {
                NCLOGD("NCNetworkHandler::setFirewallForDcm, set firewall for dcm");
                NCCHAR cmd[128] = { 0 };
                snprintf(cmd, sizeof(cmd)-1, "iptables -I FORWARD 1 -o %s -j DROP ; iptables -I OUTPUT 1 -o %s -j DROP", 
                    deviceName.getString(), deviceName.getString());
                INT ret = system(cmd);
                if (!checkSystemReturn(ret)) {
                    NCLOGD("NCNetworkHandler::setFirewallForDcm, set firewall Failed");
                    NCErrorLog(NetworkErrCode_Firewall_StartFailed, 0);

                    NCSystem systemProxy;
                    systemProxy.firewallReset();
                }
                else {
                    NCLOGD("NCNetworkHandler::setFirewallForDcm, set firewall success");
                    NCNetworkManager::Instance()->setFirewallFlag(FirewallType_Dcm, NC_TRUE);
                }
            }
        }
    }

    VOID
    NCNetworkHandler::clearFirewallForDcm(const NCString& deviceType)
    {
        NCLOGD("NCNetworkHandler::clearFirewallForDcm, Start, deviceType [%s]", deviceType.getString());
        NC_BOOL flag = NCNetworkManager::Instance()->getFirewallFlag(FirewallType_Dcm);
        NCLOGD("NCNetworkHandler::clearFirewallForDcm, Dcm firewall flag [%d]", flag);
        if (DCM == deviceType && isCarryNetworkDevice(deviceType) && flag) {
            NCEnvironment env;
            NCString country;
            env.getProductCountry(country);
            NCLOGD("NCNetworkHandler::clearFirewallForDcm, country [%s]", country.getString());
            if (NCString("America") == country) {
                NCLOGD("NCNetworkHandler::clearFirewallForDcm, set firewall for dcm");
                NCCHAR cmd[128] = { 0 };
                snprintf(cmd, sizeof(cmd)-1, "iptables -D FORWARD 1 ; iptables -D OUTPUT 1");
                INT ret = system(cmd);
                if (!checkSystemReturn(ret)) {
                    NCLOGD("NCNetworkHandler::clearFirewallForDcm, set firewall Failed");
                    NCErrorLog(NetworkErrCode_Firewall_StartFailed, 0);

                    NCSystem systemProxy;
                    systemProxy.firewallReset();
                }
                else {
                    NCLOGD("NCNetworkHandler::clearFirewallForDcm, set firewall for dcm success");
                    NCNetworkManager::Instance()->setFirewallFlag(FirewallType_Dcm, NC_FALSE);
                }
            }
        }
    }

    NC_BOOL
    NCNetworkHandler::isCarryNetworkDevice(const NCString& deviceType)
    {
        NCLOGD("NCNetworkHandler::isCarryNetworkDevice, deviceType [%s]", deviceType.getString());
        if (BT_DUN != deviceType && WIFI_STA != deviceType && DCM != deviceType) {
            NCLOGD("NCNetworkHandler::isCarryNetworkDevice, Device type error");
            return NC_FALSE;
        }

#if defined NCNETWORK_TARGET_PRODUCT_DCU
        if (checkIndividualApparatus()) {
            return NC_TRUE;
        }
        else {
            if (BT_DUN == deviceType || WIFI_STA == deviceType) {
                return NC_TRUE;
            }
            else {
                return NC_FALSE;
            }
        }
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
        if (DCM == deviceType) {
            return NC_TRUE;
        }
        else {
            return NC_FALSE;
        }
#endif

        NCLOGD("NCNetworkHandler::isCarryNetworkDevice, Not DCU and MEU");
        return NC_FALSE;
    }

    NC_BOOL
    NCNetworkHandler::isDummyDcmOn()
    {
        NCLOGD("NCNetworkHandler::isDummyDcmOn, Start");
        NCUri uri(NCNETWORK_DUMMY_DCM_KEY);
        NCContentResolver resolver;
        NCCursor* cursor = resolver.query(uri, NCObjList<NCString>(), NCString(), NCObjList<NCString>(), NCString());
        NCString value = "off";
        if (NULL != cursor) {
            if ((1 <= cursor->getCount()) && (cursor->moveToFirst())) {
                value = cursor->getString(0);
            }
            
            delete cursor;
            cursor = NULL;
        }
        else {
            NCLOGD("NCNetworkHandler::isDummyDcmOn, cursor is NULL");
            return NC_FALSE;
        }

        if (NCString("on") == value) {
            NCLOGD("NCNetworkHandler::isDummyDcmOn, dummy dcm is on");
            return NC_TRUE;
        }
        else {
            NCLOGD("NCNetworkHandler::isDummyDcmOn, dummy dcm is off");
            return NC_FALSE;
        }
    }

    NC_BOOL
    NCNetworkHandler::checkConnectPermission(const NCString& resourceId)
    {
        NCLOGD("NCNetworkHandler::checkConnectPermission, Start, resourceId [%s]", resourceId.getString());
        NCConnect connectProxy;
        if (connectProxy.verifyPermission(resourceId)) {
            NCLOGD("NCNetworkHandler::checkConnectPermission, Check permission success");
            return NC_TRUE;
        }
        else {
            NCLOGD("NCNetworkHandler::checkConnectPermission, Check permission failed");
            return NC_FALSE;
        }

    }

} /* namespace nutshell */
/* EOF */
