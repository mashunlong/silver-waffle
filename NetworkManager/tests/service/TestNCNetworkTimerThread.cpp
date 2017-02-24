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
#include <NCNetworkManagerDef.h>
#include "NCNetworkManager.h"
#include "NCNetworkTimerThread.h"

using namespace nutshell;

/**
 * TestNCNetworkTimerThread.
 *
 * gtest for TestNCNetworkTimerThread.
 */
class TestNCNetworkTimerThread: public testing::Test
{

protected:
    void SetUp()
    {
        printf("TestNCNetworkTimerThread set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkTimerThread tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkTimerThread set-up test case\n");
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkTimerThread tear-down test case\n");
    }
};

NCNetworkTimerThread* testTimerThread = new NCNetworkTimerThread();

TEST_F(TestNCNetworkTimerThread, startTimer)
{
    testTimerThread->startTimer(0);
    testTimerThread->startTimer(TimerType_DcuMeuSync);
    testTimerThread->startTimer(TimerType_AvcLanIp);
    testTimerThread->startTimer(10);
}

TEST_F(TestNCNetworkTimerThread, pop)
{
    testTimerThread->pop();
    testTimerThread->pop();
    testTimerThread->pop();
}

TEST_F(TestNCNetworkTimerThread, stopTimer)
{
    testTimerThread->stopTimer(0);
    testTimerThread->stopTimer(TimerType_DcuMeuSync);
    testTimerThread->stopTimer(TimerType_AvcLanIp);
    testTimerThread->stopTimer(10);
}

TEST_F(TestNCNetworkTimerThread, deinit)
{
    if (NULL != testTimerThread) {
        testTimerThread->stopThread();
        delete testTimerThread;
        testTimerThread = NULL;
    }
}


/* EOF */