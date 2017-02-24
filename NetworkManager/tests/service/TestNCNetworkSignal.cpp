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
#include "NCNetworkSignal.h"

using namespace nutshell;

/**
 * TestNCNetworkSignal.
 *
 * gtest for TestNCNetworkSignal.
 */
class TestNCNetworkSignal: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkSignal set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkSignal tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkSignal::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkSignal::SetUpTestCase, start service \n");
            m_service->onInit();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkSignal::TearDownTestCase, case\n");
        if (m_service != NULL) {
            printf("TestNCNetworkSignal::TearDownTestCase, end service \n");
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkSignal::m_service = NULL;
NCNetworkSignal* testSignal = new NCNetworkSignal();

TEST_F(TestNCNetworkSignal, signal)
{
    if (NULL != testSignal) {
        NCString deviceType = "wifiSta";
        testSignal->signal(deviceType);

        deviceType = "dcm";
        testSignal->signal(deviceType);

        deviceType = "btPhone";
        testSignal->signal(deviceType);

        deviceType = "others";
        testSignal->signal(deviceType);
    }
}

TEST_F(TestNCNetworkSignal, setSignalStatus)
{
    if (NULL != testSignal) {
        NCString deviceType = "wifiSta";
        NCString value = "1";
        testSignal->setSignalStatus(deviceType, value);
        testSignal->setSignalStatus(deviceType, value);
        value = "0";
        testSignal->setSignalStatus(deviceType, value);
        testSignal->setSignalStatus(deviceType, value);

        deviceType = "dcm";
        testSignal->setSignalStatus(deviceType, value);
        testSignal->setSignalStatus(deviceType, value);
        value = "1";
        testSignal->setSignalStatus(deviceType, value);
        testSignal->setSignalStatus(deviceType, value);

        deviceType = "btPhone";
        value = "disconnected";
        testSignal->setSignalStatus(deviceType, value);
        testSignal->setSignalStatus(deviceType, value);

        deviceType = "test";
        testSignal->setSignalStatus(deviceType, value);
    }
}

TEST_F(TestNCNetworkSignal, setDcmDevStatus)
{
    if (NULL != testSignal) {
        testSignal->setDcmDevStatus("inited");
        testSignal->setDcmDevStatus("uninited");
        testSignal->setDcmDevStatus("test");
    }
}

TEST_F(TestNCNetworkSignal, updateNetworkStatus)
{
    if (NULL != testSignal) {
        NCString deviceType = "dcm";
        NCString value = "disconnected";
        testSignal->updateNetworkStatus(deviceType, value);

        value = "connected";
        testSignal->updateNetworkStatus(deviceType, value);

        deviceType = "btDun";
        testSignal->updateNetworkStatus(deviceType, value);

        deviceType = "wifiSta";
        testSignal->updateNetworkStatus(deviceType, value);

        deviceType = "test";
        testSignal->updateNetworkStatus(deviceType, value);

        value = "testvalue";
        testSignal->updateNetworkStatus(deviceType, value);
    }
}


TEST_F(TestNCNetworkSignal, checkSignalStatus)
{
    if (NULL != testSignal) {
        NCString deviceType = "wifiSta";
        NCString value = "0";
        NC_BOOL result = testSignal->checkSignalStatus(deviceType, value);
        EXPECT_FALSE(result);

        value = "1";
        result = testSignal->checkSignalStatus(deviceType, value);
        EXPECT_TRUE(result);

        value = "9";
        result = testSignal->checkSignalStatus(deviceType, value);
        EXPECT_FALSE(result);

        deviceType = "dcm";
        value = "0";
        result = testSignal->checkSignalStatus(deviceType, value);
        EXPECT_FALSE(result);

        value = "4";
        result = testSignal->checkSignalStatus(deviceType, value);
        EXPECT_TRUE(result);

        value = "8";
        result = testSignal->checkSignalStatus(deviceType, value);
        EXPECT_FALSE(result);

        deviceType = "btPhone";
        value = "disconnected";
        result = testSignal->checkSignalStatus(deviceType, value);
        EXPECT_FALSE(result);

        value = "good";
        result = testSignal->checkSignalStatus(deviceType, value);
        EXPECT_TRUE(result);

        value = "test11";
        result = testSignal->checkSignalStatus(deviceType, value);
        EXPECT_FALSE(result);

        deviceType = "others";
        value = "00";
        result = testSignal->checkSignalStatus(deviceType, value);
        EXPECT_FALSE(result);
    }
}


TEST_F(TestNCNetworkSignal, deinit)
{
    if (NULL != testSignal) {
        delete testSignal;
        testSignal = NULL;
    }
}

/* EOF */