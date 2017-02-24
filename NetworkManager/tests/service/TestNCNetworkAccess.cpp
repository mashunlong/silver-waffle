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
#include <NDNetworkManager.pb.h>
#include "NCNetworkManagerDef.h"
#include "TestNCNetworkAccess.h"

namespace nutshell
{
    TestNCNetworkAccess* TestNCNetworkAccess::instance = NULL;

    TestNCNetworkAccess::TestNCNetworkAccess()
    : m_proxy(NULL)
    {
        m_proxy = new NCNetworkAccess();
        instance = this;
    }

    TestNCNetworkAccess::~TestNCNetworkAccess()
    {

    }

    VOID
    TestNCNetworkAccess::onInit()
    {
        m_proxy->bindService();
    }

    VOID
    TestNCNetworkAccess::onDeinit()
    {
        m_proxy->unbindService();
    }

    TestNCNetworkAccess*
    TestNCNetworkAccess::Instance()
    {
        return instance;
    }

    NCNetworkAccess*
    TestNCNetworkAccess::proxy()
    {
        return m_proxy.get();
    }

} /* namespace nutshell */


using namespace nutshell;

TEST(TestNCNetworkAccess, access)
{
    NCString resourceId = "987654321";
    NCString deviceType = "dcm";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkAccess::Instance()->proxy()->request(resourceId, deviceType));

    deviceType = "wifiSta";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkAccess::Instance()->proxy()->request(resourceId, deviceType));
}

TEST(TestNCNetworkAccess, release)
{
    NCString resourceId = "123456789";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkAccess::Instance()->proxy()->release(resourceId));
}

TEST(TestNCNetworkAccess, disconnect)
{
    EXPECT_EQ(NC_NOERROR, TestNCNetworkAccess::Instance()->proxy()->disconnect());
}

TEST(TestNCNetworkAccess, connect)
{
    EXPECT_EQ(NC_NOERROR, TestNCNetworkAccess::Instance()->proxy()->connect());
}

TEST(TestNCNetworkAccess, notifyRequestAccessResult)
{
    NCNetworkAccess::RequestAccessResult result;
    result.deviceName = "eth0";
    result.deviceType = "dcm";
    result.gateway = "172.26.187.1";
    result.dns1 = "172.26.10.91";
    TestNCNetworkAccess::Instance()->proxy()->notifyRequestAccessResult(result);
}

TEST(TestNCNetworkAccess, notifyDisconnectResult)
{
    NCString deviceType = "dcm";
    UINT32 result = 0;
    TestNCNetworkAccess::Instance()->proxy()->notifyDisconnectResult(deviceType, result);
}

TEST(TestNCNetworkAccess, setSlipDevice)
{
    NCNetworkAccess::SlipDeviceInfo info;
    info.deviceName = "sl0";
    info.deviceType = "slip0";
    info.ip = "192.168.2.10";
    info.mask = 24;
    info.mtu = 4096;
    EXPECT_EQ(NC_NOERROR, TestNCNetworkAccess::Instance()->proxy()->setSlipDevice(info));
}

TEST(TestNCNetworkAccess, notifySlipDisconnect)
{
    NCString deviceName = "sl0";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkAccess::Instance()->proxy()->notifySlipDisconnect(deviceName));
}

TEST(TestNCNetworkAccess, onReceiveMessage)
{
    NCString resourceId = "resourceid";
    UINT32 result = NC_NOERROR;
    nutshell::networkmanager::NDNetworkAccessResult message;
    message.set_resourceid(std::string(resourceId.getString()));
    message.set_result(result);
    std::string param("");
    message.SerializeToString(&param);

    // Receive message of request access
    NEMessage msg;
    msg.setType(NEMessage::TYPE_NOTIFY);
    msg.setMethodId(MethodId_Request);
    msg.setParam(param);
    TestNCNetworkAccess::Instance()->proxy()->onReceiveMessage(msg);

    // Receive message of release access
    msg.setMethodId(MethodId_Release);
    TestNCNetworkAccess::Instance()->proxy()->onReceiveMessage(msg);
}

/* EOF */
