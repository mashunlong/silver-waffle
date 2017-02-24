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
#include "NCNetworkAccessStack.h"
#include "NCNetworkPluginCallbackImpl.h"

using namespace nutshell;

/**
 * TestNCNetworkPluginCallbackImpl.
 *
 * gtest for TestNCNetworkPluginCallbackImpl.
 */
class TestNCNetworkPluginCallbackImpl: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkPluginCallbackImpl set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkPluginCallbackImpl tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkPluginCallbackImpl::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkPluginCallbackImpl::SetUpTestCase, start service \n");
            m_service->onInit();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkPluginCallbackImpl::TearDownTestCase, case\n");
        if (m_service != NULL) {
            printf("TestNCNetworkPluginCallbackImpl::TearDownTestCase, end service \n");
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkPluginCallbackImpl::m_service = NULL;
NCNetworkPluginCallbackImpl* testPluginCallback = new NCNetworkPluginCallbackImpl();

TEST_F(TestNCNetworkPluginCallbackImpl, notifyRequestAccessResult)
{
    if (NULL != testPluginCallback) {
        RequestPluginResult info;
        info.reqId = 1;
        info.result = 1;
        info.deviceType = "dcm";

        testPluginCallback->notifyRequestAccessResult(info);
    }
}

TEST_F(TestNCNetworkPluginCallbackImpl, notifyReleaseAccessResult)
{
    if (NULL != testPluginCallback) {
        testPluginCallback->notifyReleaseAccessResult(0, "dcm", 1);
    }
}

TEST_F(TestNCNetworkPluginCallbackImpl, notifyUpdateAccessResult)
{
    if (NULL != testPluginCallback) {
        testPluginCallback->notifyUpdateAccessResult(0, 1);
    }
}

TEST_F(TestNCNetworkPluginCallbackImpl, deinit)
{
    if (NULL != testPluginCallback) {
        delete testPluginCallback;
        testPluginCallback = NULL;
    }
}

/* EOF */