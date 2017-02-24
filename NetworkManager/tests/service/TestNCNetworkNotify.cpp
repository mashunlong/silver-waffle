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
#include "NCNetworkManager.h"
#include "NCNetworkDeviceType.h"
#include "NCNetworkReq.h"
#include "NCNetworkThread.h"
#include "NCNetworkNotify.h"

using namespace nutshell;

/**
 * TestNCNetworkNotify.
 *
 * gtest for TestNCNetworkNotify.
 */
class TestNCNetworkNotify: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkNotify set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkNotify tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkNotify::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkNotify::SetUpTestCase, start service \n");
            m_service->onInit();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkNotify::TearDownTestCase, stop service\n");
        if (m_service != NULL) {
            printf("TestNCNetworkNotify::SetUpTestCase, end service \n");
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkNotify::m_service = NULL;
NCNetworkNotify* testNotify = new NCNetworkNotify();

TEST_F(TestNCNetworkNotify, notifyRequestIpResult)
{
    NCString deviceName = "wlan0";
    UINT32 result = NC_NOERROR;
    if (NULL != testNotify) {
        testNotify->notifyRequestIpResult(deviceName, result);
    }
}

TEST_F(TestNCNetworkNotify, notifyReleaseIpResult)
{
    NCString deviceName = "wlan0";
    UINT32 result = NC_NOERROR;
    if (NULL != testNotify) {
        testNotify->notifyReleaseIpResult(deviceName, result);
    }
}

TEST_F(TestNCNetworkNotify, notifyStartDhcpServerResult)
{
    NCString deviceName = "wlan0";
    UINT32 result = NC_NOERROR;
    if (NULL != testNotify) {
        testNotify->notifyStartDhcpServerResult(deviceName, result);
    }
}

TEST_F(TestNCNetworkNotify, notifyStopDhcpServerResult)
{
    NCString deviceName = "wlan0";
    UINT32 result = NC_NOERROR;
    if (NULL != testNotify) {
        testNotify->notifyStopDhcpServerResult(deviceName, result);
    }
}

TEST_F(TestNCNetworkNotify, notifyRequestAccessResult)
{
    NCString resourceId = "resourceid";
    UINT32 result = NC_NOERROR;
    if (NULL != testNotify) {
        testNotify->notifyRequestAccessResult(resourceId, result, 0, 1);
    }
}

TEST_F(TestNCNetworkNotify, notifyReleaseAccessResult)
{
    NCString resourceId = "resourceid";
    UINT32 result = NC_NOERROR;
    if (NULL != testNotify) {
        testNotify->notifyReleaseAccessResult(resourceId, result, 1);
    }
}

TEST_F(TestNCNetworkNotify, notifyDisconnectResult)
{
    NCString deviceType = "dcm";
    UINT32 result = NC_NOERROR;
    if (NULL != testNotify) {
        testNotify->notifyDisconnectResult(deviceType, result, 1);
    }
}

TEST_F(TestNCNetworkNotify, notifyDeviceInfo)
{
    NCString deviceName = "wlan0";
    NCNetworkDataReplier::NetworkDeviceInfo info;
    if (NULL != testNotify) {
        testNotify->notifyDeviceInfo(deviceName, info);
    }
}

TEST_F(TestNCNetworkNotify, notifyConnectedClientInfo)
{
    NCString mac = "22:34:5e:ca:9b:43";
    NCNetworkDataReplier::ConnectedClientInfo info;
    if (NULL != testNotify) {
        testNotify->notifyConnectedClientInfo(mac, info);
    }
}

TEST_F(TestNCNetworkNotify, notifyDialState)
{
    NCString deviceType = "dcm";
    if (NULL != testNotify) {
        testNotify->notifyDialState(deviceType, DialState_Connecting, DialType_Auto);
    }
}

TEST_F(TestNCNetworkNotify, notifyStartFirewallFailed)
{
    if (NULL != testNotify) {
        testNotify->notifyStartFirewallFailed();
    }
}

TEST_F(TestNCNetworkNotify, broadcastNetworkStatus)
{
    NCString identity = NCNETWORK_EVENT_ROUTECHANGED;
    NCString deviceType = "dcm";
    if (NULL != testNotify) {
        testNotify->broadcastNetworkStatus(identity, deviceType);
    }
}

TEST_F(TestNCNetworkNotify, deinit)
{
    if (NULL != testNotify) {
        delete testNotify;
        testNotify = NULL;
    }
}

/* EOF */

