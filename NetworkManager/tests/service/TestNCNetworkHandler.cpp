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

#include <gtest/gtest.h>
#include "NCNetworkHandler.h"

using namespace nutshell;

NCNetworkHandler* testHandler = NULL;

TEST(TestNCNetworkHandler, init)
{
    testHandler = new NCNetworkHandler();
}

TEST(TestNCNetworkHandler, requestIp)
{
    NCString deviceName = "wlan0";
    if (NULL != testHandler) {
        testHandler->requestIp(deviceName);
    }
}

TEST(TestNCNetworkHandler, stopRequestIp)
{
    NCString deviceName = "wlan0";
    if (NULL != testHandler) {
        testHandler->stopRequestIp(deviceName);
    }
}

TEST(TestNCNetworkHandler, releaseIp)
{
    NCString deviceName = "wlan0";
    if (NULL != testHandler) {
        testHandler->releaseIp(deviceName);
    }
}

TEST(TestNCNetworkHandler, startDhcpServer)
{
    NCString deviceName = "wlan0";
    NCString ip = "192.168.45.1";
    UINT32 prefixLength = 24;

    if (NULL != testHandler) {
        testHandler->startDhcpServer(deviceName, ip, prefixLength);
    }
}

TEST(TestNCNetworkHandler, stopDhcpServer)
{
    NCString deviceName = "wlan0";
    if (NULL != testHandler) {
        testHandler->stopDhcpServer(deviceName);
    }
}

TEST(TestNCNetworkHandler, networkDeviceInfo)
{
    NCString deviceName = "wlan0";
    if (NULL != testHandler) {
        testHandler->networkDeviceInfo(deviceName);
    }
}

TEST(TestNCNetworkHandler, connectedClientInfo)
{
    NCString mac = "123456";
    if (NULL != testHandler) {
        testHandler->connectedClientInfo(mac);
    }
}

TEST(TestNCNetworkHandler, startDnsCacheServer)
{
    if (NULL != testHandler) {
        testHandler->startDnsCacheServer();
    }
}

TEST(TestNCNetworkHandler, setStaticIp)
{
    if (NULL != testHandler) {
        NCString deviceName = "wlan0";
        NCString ip = "192.168.45.1";
        testHandler->setStaticIp(deviceName, ip);
    }
}

TEST(TestNCNetworkHandler, setMask)
{
    if (NULL != testHandler) {
        NCString deviceName = "wlan0";
        UINT32 prefixLength = 24;
        testHandler->setMask(deviceName, prefixLength);
    }
}

TEST(TestNCNetworkHandler, setMtu)
{
    if (NULL != testHandler) {
        NCString deviceName = "wlan0";
        UINT32 mtu = 1024;
        testHandler->setMtu(deviceName, mtu);
    }
}

TEST(TestNCNetworkHandler, setStaticIpMaskMtu)
{
    if (NULL != testHandler) {
        NCString deviceName = "wlan0";
        NCString ip = "192.168.45.1";
        UINT32 prefixLength = 24;
        UINT32 mtu = 1024;
        testHandler->setStaticIpMaskMtu(deviceName, ip, prefixLength, mtu);
    }
}

TEST(TestNCNetworkHandler, addDefaultRoute)
{
    if (NULL != testHandler) {
        NCString deviceName = "wlan0";
        NCString gateway = "192.168.45.1";
        testHandler->addDefaultRoute(deviceName, gateway);
    }
}

TEST(TestNCNetworkHandler, removeDefaultRoute)
{
    if (NULL != testHandler) {
        NCString deviceName = "wlan0";
        testHandler->removeDefaultRoute(deviceName);
    }
}

TEST(TestNCNetworkHandler, setNatTable)
{
    if (NULL != testHandler) {
        // NCString deviceType = "dcm";
        // testHandler->setNatTable(deviceType);

        // deviceType = "wifiSta";
        // testHandler->setNatTable(deviceType);

        // deviceType = "test";
        // testHandler->setNatTable(deviceType);
    }
}

TEST(TestNCNetworkHandler, clearNatTable)
{
    if (NULL != testHandler) {
        testHandler->clearNatTable();
    }
}

TEST(TestNCNetworkHandler, readAvcLanIp)
{
    if (NULL != testHandler) {
        DeviceIpInfo info;
        testHandler->readAvcLanIp(NCNETWORK_AVCLANIP_CONFIG_SECTION, info);

        NCString file = "/etc/networkmanager/avclanip_config";
        FILE* pid = fopen(file, "at");
        if (NULL != pid) {
            fprintf(pid, "%s\n", "[staticIp]");
            fprintf(pid, "%s\n", "device=usb0");
            fprintf(pid, "%s\n", "ip=192.168.128.129");
            fprintf(pid, "%s\n", "mask=255.255.255.252");
            fclose(pid);
            testHandler->readAvcLanIp(NCNETWORK_AVCLANIP_CONFIG_SECTION, info);
        }
    }
}

TEST(TestNCNetworkHandler, readAvcLanDefaultIp)
{
    if (NULL != testHandler) {
        DeviceIpInfo info;
        testHandler->readAvcLanDefaultIp(info);
    }
}

TEST(TestNCNetworkHandler, readWifiApStaticIp)
{
    if (NULL != testHandler) {
        NCString ip;
        UINT32 prefixLength;
        testHandler->readWifiApStaticIp(ip, prefixLength);

        NCString file = "/etc/networkmanager/avclanip_config";
        FILE* pid = fopen(file, "at");
        if (NULL != pid) {
            fprintf(pid, "%s\n", "[wifiAp]");
            fprintf(pid, "%s\n", "ip=192.168.45.1");
            fprintf(pid, "%s\n", "mask=255.255.255.0");
            fclose(pid);
            testHandler->readWifiApStaticIp(ip, prefixLength);
        }
    }
}

TEST(TestNCNetworkHandler, checkAvcLanStatus)
{
    if (NULL != testHandler) {
        testHandler->checkAvcLanStatus();
    }
}

TEST(TestNCNetworkHandler, deinit)
{
    if (NULL != testHandler) {
        delete testHandler;
        testHandler = NULL;
    }
}

/* EOF */
