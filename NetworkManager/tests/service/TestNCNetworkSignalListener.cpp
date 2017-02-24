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
#include "NCNetworkSignal.h"
#include "NCNetworkSignalListener.h"

using namespace nutshell;

/**
 * TestNCNetworkSignalListener.
 *
 * gtest for TestNCNetworkSignalListener.
 */
class TestNCNetworkSignalListener: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkSignalListener set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkSignalListener tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkSignalListener::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkSignalListener::SetUpTestCase, start service \n");
            m_service->onInit();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkSignalListener::TearDownTestCase, case\n");
        if (m_service != NULL) {
            printf("TestNCNetworkSignalListener::TearDownTestCase, end service \n");
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkSignalListener::m_service = NULL;
NCNetworkSignalListener* testSignalListener = new NCNetworkSignalListener("network_signal_listener");

TEST_F(TestNCNetworkSignalListener, onNotifyUpdate)
{
if (NULL != testSignalListener) {
        NCString wifiUriString = "content://system/info?wifi/strength";
        NCUri wifiUri(wifiUriString);
        std::string strValue = "0";
        NCVariant variant(strValue); 
        testSignalListener->onNotifyUpdate(wifiUri, variant);

        strValue = "1";
        variant.setValue(strValue);
        testSignalListener->onNotifyUpdate(wifiUri, variant);

        testSignalListener->m_signal = NULL;
        testSignalListener->onNotifyUpdate(wifiUri, variant);
        testSignalListener->m_signal = new NCNetworkSignal();

        NCString dcmUriString = "content://system/info?dcm/strength";
        NCUri dcmUri(dcmUriString);
        strValue = "0";
        variant.setValue(strValue);
        testSignalListener->onNotifyUpdate(dcmUri, variant);

        strValue = "1";
        variant.setValue(strValue);
        testSignalListener->onNotifyUpdate(dcmUri, variant);

        testSignalListener->m_signal = NULL;
        testSignalListener->onNotifyUpdate(dcmUri, variant);
        testSignalListener->m_signal = new NCNetworkSignal();

        NCString btUriString = "content://system/info?bt/linkquality";
        NCUri btUri(btUriString);
        strValue = "disconnected";
        variant.setValue(strValue);
        testSignalListener->onNotifyUpdate(btUri, variant);

        strValue = "good";
        variant.setValue(strValue);
        testSignalListener->onNotifyUpdate(btUri, variant);

        testSignalListener->m_signal = NULL;
        testSignalListener->onNotifyUpdate(btUri, variant);
        testSignalListener->m_signal = new NCNetworkSignal();

        NCString dcmDeviceString = "content://system/info?dcm/devstat";
        NCUri dcmDeviceUri(dcmDeviceString);
        strValue = "inited";
        variant.setValue(strValue);
        testSignalListener->onNotifyUpdate(dcmDeviceUri, variant);

        strValue = "uninited";
        variant.setValue(strValue);
        testSignalListener->onNotifyUpdate(dcmDeviceUri, variant);

        testSignalListener->m_signal = NULL;
        testSignalListener->onNotifyUpdate(btUri, variant);
        testSignalListener->m_signal = new NCNetworkSignal();

        NCString uriString = "testuristring";
        NCUri otherUri(uriString);
        strValue = "testvalue";
        variant.setValue(strValue);
        testSignalListener->onNotifyUpdate(otherUri, variant);
    }
}

TEST_F(TestNCNetworkSignalListener, deinit)
{
    if (NULL != testSignalListener) {
        delete testSignalListener;
        testSignalListener = NULL;
    }

    testSignalListener = new NCNetworkSignalListener("network_signal_listener");
    if (NULL != testSignalListener) {
        testSignalListener->m_signal = NULL;
        delete testSignalListener;
        testSignalListener = NULL;
    }
}


/* EOF */

