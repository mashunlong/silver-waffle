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
#include "NCNetworkManagerDef.h"
#include "NCNetworkManager.h"
#include "NCNetworkThread.h"
#include "networkmanager/NCNetworkAccess.h"
#include "NCNetworkReq.h"

using namespace nutshell;

/**
 * TestNCNetworkReq.
 *
 * gtest for TestNCNetworkReq.
 */
class TestNCNetworkReq: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkReq set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkReq tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkReq::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkSignal::SetUpTestCase, start service \n");
            m_service->onInit();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkReq::TearDownTestCase, case\n");
        if (m_service != NULL) {
            printf("TestNCNetworkSignal::TearDownTestCase, end service \n");
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkReq::m_service = NULL;
NCNetworkThread* testReqThread = new NCNetworkThread(NCNETWORK_REQ_THREAD);

TEST_F(TestNCNetworkReq, NCNetworkSetAdbDebugIpReq)
{
    if (NULL != testReqThread) {
        NCNetworkSetAdbDebugIpReq testReq;
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkSetAvcLanIpReq)
{
    if (NULL != testReqThread) {
        NCNetworkSetAvcLanIpReq testReq;
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkReleaseReq)
{
    if (NULL != testReqThread) {
        AccessParameter param;
        param.resourceId = "123456";
        param.deviceType = "wifiSta";
        param.reqId = 1;
        NCNetworkReleaseReq testReq(param);
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkSetSlipDeviceReq)
{
    if (NULL != testReqThread) {
        NCNetworkAccess::SlipDeviceInfo info;
        info.deviceName = "wlan0";
        info.deviceType = "wifiSta";
        info.ip = "192.168.45.1";
        info.mask = 24;
        info.mtu = 1111;

        NCNetworkSetSlipDeviceReq testReq(info);
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkRequestIpReq)
{
    if (NULL != testReqThread) {
        NCNetworkRequestIpReq testReq("123456", "wifiSta", "wlan0");
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkReleaseIpReq)
{
    if (NULL != testReqThread) {
        NCNetworkReleaseIpReq testReq("wifiSta", "wlan0");
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkStartDhcpServerReq)
{
    if (NULL != testReqThread) {
        NCNetworkStartDhcpServerReq testReq("wifiSta", "wlan0");
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkStopDhcpServerReq)
{
    if (NULL != testReqThread) {
        NCNetworkStopDhcpServerReq testReq("wifiSta", "wlan0");
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkDeviceInfoReq)
{
    if (NULL != testReqThread) {
        NCNetworkDeviceInfoReq testReq("wifiSta");
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkConnectedClientInfo)
{
    if (NULL != testReqThread) {
        NCNetworkConnectedClientInfo testReq("123456");
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkInitResolvConfReq)
{
    if (NULL != testReqThread) {
        NCNetworkInitResolvConfReq testReq;
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkStartDnsCacheServerReq)
{
    if (NULL != testReqThread) {
        NCNetworkStartDnsCacheServerReq testReq;
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkStartFirewallReq)
{
    if (NULL != testReqThread) {
        NCNetworkStartFirewallReq testReq(1);
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, NCNetworkStopCurrentProcessReq)
{
    if (NULL != testReqThread) {
        NCNetworkStopCurrentProcessReq testReq;
        testReq.doTimeout();
        testReq.doAction(testReqThread);
    }
}

TEST_F(TestNCNetworkReq, stop)
{
    if (NULL != testReqThread) {
        delete testReqThread;
        testReqThread = NULL;
    }
}


/* EOF */

