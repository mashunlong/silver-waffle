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
#include <ncore/NCVariant.h>
#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCParcel.h>
#include <system/PfServiceDefine.h>
#include "networkmanager/NCNetworkDef.h"
#include "NCNetworkManagerDef.h"
#include "NCNetworkPluginManager.h"
#include "NCNetworkManager.h"

using namespace nutshell;

/**
 * TestNCNetworkManager.
 *
 * gtest for TestNCNetworkManager.
 */
class TestNCNetworkManager: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkManager set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkManager tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkManager::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkManager::SetUpTestCase, start service \n");
            m_service->onInit();
            m_service->onAwake();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkManager::TearDownTestCase, case\n");
        if (m_service != NULL) {
            printf("TestNCNetworkManager::TearDownTestCase, end service \n");
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};


ncsp<NCNetworkManager>::sp TestNCNetworkManager::m_service = NULL;

TEST_F(TestNCNetworkManager, HttpClienBegin)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCParcel data, reply;
        data.setDataPosition(0);
        m_service->onCommand(FunctionId_HttpClienBegin, data, &reply, 0);
    }
}

TEST_F(TestNCNetworkManager, Request)
{
    if (m_service != NULL) {
        NCString resourceId = "987654321";
        NCString deviceType = "dcm";
        AccessMode mode;

        NCParcel data1, reply1;
        data1.writeCString(resourceId.getString());
        data1.writeCString(deviceType.getString());
        data1.writeInt32(mode.type);
        data1.writeInt32(mode.priority);
        data1.setDataPosition(0);
        EXPECT_TRUE(m_service->onCommand(FunctionId_Request, data1, &reply1, 0));
    }
}

TEST_F(TestNCNetworkManager, release)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString resourceId = "987654321";
        NCParcel data, reply;
        data.writeCString(resourceId.getString());
        data.setDataPosition(0);
        NC_BOOL result = m_service->onCommand(FunctionId_Release, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, disconnect)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCParcel data, reply;
        data.setDataPosition(0);
        NC_BOOL result = m_service->onCommand(FunctionId_Disconnect, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, notifyDisconnectResult)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString deviceType = "btDun";
        UINT32 result = 0;
        NCParcel data, reply;
        data.writeCString(deviceType.getString());
        data.writeInt32(result);
        data.setDataPosition(0);
        m_service->onCommand(FunctionId_NotifyDisconnectResult, data, &reply, 0);
    }
}


TEST_F(TestNCNetworkManager, RequestIp)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString deviceType = "wifiSta";
        NCString deviceName = "wlan0";
        NCParcel data, reply;
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        data.setDataPosition(0);
        NC_BOOL result = m_service->onCommand(FunctionId_RequestIp, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, ReleaseIp)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString deviceType = "wifiSta";
        NCString deviceName = "wlan0";
        NCParcel data, reply;
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        data.setDataPosition(0);
        NC_BOOL result = m_service->onCommand(FunctionId_ReleaseIp, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, startDhcpServer)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString deviceType = "wifiSta";
        NCString deviceName = "wlan0";
        NCParcel data, reply;
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        data.setDataPosition(0);
        NC_BOOL result = m_service->onCommand(FunctionId_StartDhcpServer, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, stopDhcpServer)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString deviceType = "wifiSta";
        NCString deviceName = "wlan0";
        NCParcel data, reply;
        data.writeCString(deviceType.getString());
        data.writeCString(deviceName.getString());
        data.setDataPosition(0);
        NC_BOOL result = m_service->onCommand(FunctionId_StopDhcpServer, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, checkDhcpServerStatus)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCParcel data, reply;
        data.setDataPosition(0);
        NC_BOOL result = m_service->onCommand(FunctionId_CheckDhcpServerStatus, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, setSlipDevice)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString deviceName = "sl0";
        NCString deviceType = "slip0";
        NCString ip = "192.168.2.10";
        UINT32 mask = 24;
        UINT32 mtu = 4096;

        NCParcel data, reply;
        data.writeCString(deviceName.getString());
        data.writeCString(deviceType.getString());
        data.writeCString(ip.getString());
        data.writeInt32(mask);
        data.writeInt32(mtu);
        data.setDataPosition(0);

        NC_BOOL result = m_service->onCommand(FunctionId_SetSlipDevice, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, notifySlipDisconnect)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString deviceName = "sl0";
        NCParcel data, reply;
        data.writeCString(deviceName.getString());
        data.setDataPosition(0);

        NC_BOOL result = m_service->onCommand(FunctionId_NotifySlipDisconnect, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, deviceInfo)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString deviceType = "wifiSta";
        NCParcel data, reply;
        data.writeCString(deviceType.getString());
        data.setDataPosition(0);

        NC_BOOL result = m_service->onCommand(FunctionId_DeviceInfo, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, connectedClientInfo)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString mac = "98:34:2e:c4:5a:6d";
        NCParcel data, reply;
        data.writeCString(mac.getString());
        data.setDataPosition(0);

        NC_BOOL result = m_service->onCommand(FunctionId_ConnectedClientInfo, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, currentUseNetwork)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCParcel data, reply;
        data.setDataPosition(0);
        NC_BOOL result = m_service->onCommand(FunctionId_CurrentUseNetwork, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, getDeviceStatus)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString deviceType = "dcm";
        NCParcel data, reply;
        data.writeCString(deviceType.getString());
        data.setDataPosition(0);
        m_service->onCommand(FunctionId_DeviceStatus, data, &reply, 0);
    }
}

TEST_F(TestNCNetworkManager, networkSignal)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCString deviceType = "wifi";
        NCParcel data, reply;
        data.writeCString(deviceType.getString());
        data.setDataPosition(0);
        NC_BOOL result = m_service->onCommand(FunctionId_NetworkSignal, data, &reply, 0);
        EXPECT_TRUE(result);
    }
}

TEST_F(TestNCNetworkManager, testDefault)
{
    if (NCNetworkManager::Instance() != NULL) {
        NCParcel data, reply;
        data.setDataPosition(0);
        NC_BOOL result = m_service->onCommand(999, data, &reply, 0);
        EXPECT_FALSE(result);
    }
}

TEST_F(TestNCNetworkManager, getAvcLanIpInfo)
{
    if (NCNetworkManager::Instance() != NULL) {
        DeviceIpInfo info;
        info = m_service->getAvcLanIpInfo();
    }
}

TEST_F(TestNCNetworkManager, getNetworkDeviceType)
{
    if (NCNetworkManager::Instance() != NULL) {
        m_service->getNetworkDeviceType();
    }
}

TEST_F(TestNCNetworkManager, getAccessStack)
{
    if (NCNetworkManager::Instance() != NULL) {
        m_service->getAccessStack();
    }
}

TEST_F(TestNCNetworkManager, getNetworkStatus)
{
    if (NCNetworkManager::Instance() != NULL) {
        m_service->getNetworkStatus();
    }
}

TEST_F(TestNCNetworkManager, getNetworkAvcUnit)
{
    if (NCNetworkManager::Instance() != NULL) {
        m_service->getNetworkAvcUnit();
    }
}

TEST_F(TestNCNetworkManager, getNetworkPluginManager)
{
    if (NCNetworkManager::Instance() != NULL) {
        m_service->getNetworkPluginManager();
    }
}

TEST_F(TestNCNetworkManager, postRequest)
{
    if (NCNetworkManager::Instance() != NULL) {
        m_service->postRequest(NULL);
    }
}

TEST_F(TestNCNetworkManager, insertRequest)
{
    if (NCNetworkManager::Instance() != NULL) {
        m_service->insertRequest(NULL);
    }
}

/* EOF */
