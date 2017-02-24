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
#include "TestNCHttpClientManager.h"

namespace nutshell
{
    TestNCHttpClientManager* TestNCHttpClientManager::instance = NULL;

    TestNCHttpClientManager::TestNCHttpClientManager()
    {
        instance = this;
    }

    TestNCHttpClientManager::~TestNCHttpClientManager()
    {

    }

    VOID
    TestNCHttpClientManager::onInit()
    {
    }

    VOID
    TestNCHttpClientManager::onDeinit()
    {
    }

    TestNCHttpClientManager*
    TestNCHttpClientManager::Instance()
    {
        return instance;
    }

    ncsp<NCHttpClientManager>::sp
    TestNCHttpClientManager::proxy()
    {
        ncsp<NCHttpClientManager>::sp httpClientManagerProxy = new NCHttpClientManager();
        return httpClientManagerProxy;
    }

} /* namespace nutshell */


using namespace nutshell;

TEST(TestNCHttpClientManager, getAccessPermission)
{
    EXPECT_EQ(NC_NOERROR, TestNCHttpClientManager::Instance()->proxy()->getAccessPermission());
}

TEST(TestNCHttpClientManager, releaseAccessPermission)
{
    EXPECT_EQ(NC_NOERROR, TestNCHttpClientManager::Instance()->proxy()->releaseAccessPermission());
}

TEST(TestNCHttpClientManager, setLogFileFlagTRUE)
{
    EXPECT_EQ(NC_NOERROR, TestNCHttpClientManager::Instance()->proxy()->setLogFileFlag(NC_TRUE));
}

TEST(TestNCHttpClientManager, getLogFileFlagTRUE)
{
    NC_BOOL flag = NC_FALSE;
    TestNCHttpClientManager::Instance()->proxy()->getLogFileFlag(flag);
    EXPECT_EQ(NC_TRUE, flag);
}

TEST(TestNCHttpClientManager, setLogFileFlagFALSE)
{
    EXPECT_EQ(NC_NOERROR, TestNCHttpClientManager::Instance()->proxy()->setLogFileFlag(NC_FALSE));
}

TEST(TestNCHttpClientManager, getLogFileFlagFALSE)
{
    NC_BOOL flag = NC_FALSE;
    TestNCHttpClientManager::Instance()->proxy()->getLogFileFlag(flag);
    EXPECT_EQ(NC_FALSE, flag);
}

/* EOF */
