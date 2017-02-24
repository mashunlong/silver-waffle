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
#include "NCNetworkReqConfirm.h"

using namespace nutshell;

/**
 * TestNCNetworkConfirm.
 *
 * gtest for TestNCNetworkConfirm.
 */
class TestNCNetworkReqConfirm: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkReqConfirm set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkReqConfirm tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkReqConfirm::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkReqConfirm::SetUpTestCase, start service \n");
            m_service->onInit();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkReqConfirm::TearDownTestCase, stop service\n");
        if (m_service != NULL) {
            printf("TestNCNetworkReqConfirm::SetUpTestCase, end service \n");
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkReqConfirm::m_service = NULL;
NCNetworkReqConfirm* testConfirm = new NCNetworkReqConfirm();

TEST_F(TestNCNetworkReqConfirm, getDeviceType)
{
    if (NULL != testConfirm) {
        testConfirm->getDeviceType(RequestType_Init);
    }
}

TEST_F(TestNCNetworkReqConfirm, getDeviceName)
{
    if (NULL != testConfirm) {
        testConfirm->getDeviceName(RequestType_Init);
    }
}

TEST_F(TestNCNetworkReqConfirm, stop)
{
    if (NULL != testConfirm) {
        delete testConfirm;
        testConfirm = NULL;
    }
}

/* EOF */