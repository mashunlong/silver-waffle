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
#include "NCNetworkStatus.h"

using namespace nutshell;

/**
 * TestNCNetworkStatus.
 *
 * gtest for TestNCNetworkStatus.
 */
class TestNCNetworkStatus: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkStatus set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkStatus tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkStatus::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkStatus::SetUpTestCase, start service \n");
            m_service->onInit();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkStatus::TearDownTestCase, stop service\n");
        if (m_service != NULL) {
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkStatus::m_service = NULL;
NCNetworkStatus* testStatus = NULL;

TEST_F(TestNCNetworkStatus, init)
{
    testStatus = new NCNetworkStatus();
}

TEST_F(TestNCNetworkStatus, setStatus)
{
    NCString deviceType = "wifiSta";
    if (NULL != testStatus) {
        testStatus->setStatus(deviceType, NC_TRUE);
        testStatus->setStatus(deviceType, NC_TRUE);
        testStatus->setStatus(deviceType, NC_FALSE);
    }
}

TEST_F(TestNCNetworkStatus, status)
{
    if (NULL != testStatus) {
        testStatus->status("wifiSta");
        testStatus->status("dcm");
    }
}

TEST_F(TestNCNetworkStatus, setCurrentNetwork)
{
    if (NULL != testStatus) {
        testStatus->setCurrentNetwork("");
        testStatus->setCurrentNetwork("wifiSta");
        testStatus->setCurrentNetwork("wifiSta");
    }
}

TEST_F(TestNCNetworkStatus, currentNetwork)
{
    if (NULL != testStatus) {
        testStatus->currentNetwork();
    }
}

TEST_F(TestNCNetworkStatus, deinit)
{
    if (NULL != testStatus) {
        delete testStatus;
        testStatus = NULL;
    }
}

TEST_F(TestNCNetworkStatus, stop)
{
    if (NULL != testStatus) {
        delete testStatus;
        testStatus = NULL;
    }
}

/* EOF */