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
#include "NCNetworkTimer.h"

using namespace nutshell;

/**
 * TestNCNetworkTimer.
 *
 * gtest for TestNCNetworkTimer.
 */
class TestNCNetworkTimer: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkTimer set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkTimer tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkTimer::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkTimer::SetUpTestCase, start service \n");
            m_service->onInit();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkTimer::TearDownTestCase, case\n");
        if (m_service != NULL) {
            printf("TestNCNetworkTimer::TearDownTestCase, end service \n");
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkTimer::m_service = NULL;
NCNetworkTimer* testTimer = new NCNetworkTimer(1, 5*1000, NC_FALSE);

TEST_F(TestNCNetworkTimer, OnTimer)
{
    if (NULL != testTimer) {
        testTimer->m_type = TimerType_DcuMeuSync;
        testTimer->OnTimer();

        testTimer->m_type = TimerType_AvcLanIp;
        testTimer->OnTimer();

        testTimer->m_type = 10;
        testTimer->OnTimer();
    }
}

TEST_F(TestNCNetworkTimer, deinit)
{
    if (NULL != testTimer) {
        delete testTimer;
        testTimer = NULL;
    }
}


/* EOF */