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
#include "NCNetworkPluginCallbackImpl.h"
#include "NCNetworkPluginManager.h"

using namespace nutshell;

/**
 * TestNCNetworkPluginManager.
 *
 * gtest for TestNCNetworkPluginManager.
 */
class TestNCNetworkPluginManager: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkPluginManager set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkPluginManager tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkPluginManager set-up test case\n");
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkPluginManager tear-down test case\n");
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkPluginManager::m_service = new NCNetworkManager();
NCNetworkPluginManager* testPlugin = new NCNetworkPluginManager(new NCNetworkPluginCallbackImpl());

TEST_F(TestNCNetworkPluginManager, init)
{
    m_service->onInit();
    m_service->onAwake();
}

TEST_F(TestNCNetworkPluginManager, onInit)
{
    if (NULL != testPlugin) {
        testPlugin->onInit();
    }
}

TEST_F(TestNCNetworkPluginManager, readPluginConfigFile)
{
    if (NULL != testPlugin) {
        testPlugin->readPluginConfigFile();
    }
}

TEST_F(TestNCNetworkPluginManager, loadPlugin)
{
    if (NULL != testPlugin) {
        NCString section = "dcm";
        NCString path = "/usr/lib/libnwplugindcm.so";
        testPlugin->loadPlugin(section, path);
    }
}

TEST_F(TestNCNetworkPluginManager, requestNetworkAccess)
{
    if (NULL != testPlugin) {
        AccessParameter param;
        param.deviceType = "dcm";
        testPlugin->requestNetworkAccess(param);
    }
}

TEST_F(TestNCNetworkPluginManager, releaseNetworkAccess)
{
    if (NULL != testPlugin) {
        testPlugin->releaseNetworkAccess("dcm", 1);
        testPlugin->releaseNetworkAccess("btDun", 2);
        testPlugin->releaseNetworkAccess("test", 3);
    }
}

TEST_F(TestNCNetworkPluginManager, updateNetworkAccess)
{
    if (NULL != testPlugin) {
        testPlugin->updateNetworkAccess("dcm", "123456", 1);
        testPlugin->updateNetworkAccess("btDun", "123456", 2);
        testPlugin->updateNetworkAccess("test", "123456", 3);
    }
}

TEST_F(TestNCNetworkPluginManager, unloadPlugin)
{
    if (NULL != testPlugin) {
        testPlugin->unloadPlugin();
    }
}

TEST_F(TestNCNetworkPluginManager, deinit)
{
    if (NULL != testPlugin) {
        delete testPlugin;
        testPlugin = NULL;
    }

    m_service->onStop();
    m_service->onDeinit();
}

/* EOF */