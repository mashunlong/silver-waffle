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
#include "networkmanager/NCNetworkDhcpReplier.h"
#include "TestNCNetworkDhcp.h"

namespace nutshell
{
    /**
     * @brief The replier class of NCNetworkDhcp.
     *
     * TestNCNetworkDhcpReplier.
     */
    class TestNCNetworkDhcpReplier : public NCNetworkDhcpReplier
    {
    public:
        virtual VOID replyRequestIp(const NCString& deviceType, UINT32 result)
        {

        }

        virtual VOID replyStopRequestIp(const NCString& deviceType, UINT32 result)
        {

        }

        virtual VOID replyReleaseIp(const NCString& deviceType, UINT32 result)
        {

        }

        virtual VOID replyStartDhcpServer(const NCString& deviceType, UINT32 result)
        {

        }

        virtual VOID replyStopDhcpServer(const NCString& deviceType, UINT32 result)
        {

        }

    };

    TestNCNetworkDhcp* TestNCNetworkDhcp::instance = NULL;

    TestNCNetworkDhcp::TestNCNetworkDhcp()
    : m_proxy(NULL)
    {
        m_proxy = new NCNetworkDhcp();
        instance = this;
    }

    TestNCNetworkDhcp::~TestNCNetworkDhcp()
    {

    }

    VOID
    TestNCNetworkDhcp::onInit()
    {
        m_proxy->bindService();
    }

    VOID
    TestNCNetworkDhcp::onDeinit()
    {
        m_proxy->unbindService();
    }

    TestNCNetworkDhcp*
    TestNCNetworkDhcp::Instance()
    {
        return instance;
    }

    NCNetworkDhcp*
    TestNCNetworkDhcp::proxy()
    {
        return m_proxy.get();
    }

} /* namespace nutshell */

using namespace nutshell;

TEST(TestNCNetworkDhcp, requestIp)
{
    NCString deviceType = "dcm";
    NCString deviceName = "eth0";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkDhcp::Instance()->proxy()->requestIp(deviceType, deviceName));
}

TEST(TestNCNetworkDhcp, stopRequestIp)
{
    NCString deviceType = "wifiSta";
    NCString deviceName = "wlan0";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkDhcp::Instance()->proxy()->stopRequestIp(deviceType, deviceName));
}

TEST(TestNCNetworkDhcp, releaseIp)
{
    NCString deviceType = "wifiSta";
    NCString deviceName = "eth0";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkDhcp::Instance()->proxy()->releaseIp(deviceType, deviceName));
}

TEST(TestNCNetworkDhcp, startDhcpServer)
{
    NCString deviceType = "wifiSta";
    NCString deviceName = "wlan0";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkDhcp::Instance()->proxy()->startDhcpServer(deviceType, deviceName));
}

TEST(TestNCNetworkDhcp, stopDhcpServer)
{
    NCString deviceType = "wifiSta";
    NCString deviceName = "wlan0";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkDhcp::Instance()->proxy()->stopDhcpServer(deviceType, deviceName));
}

TEST(TestNCNetworkDhcp, checkDhcpClientStatus)
{
    NCString deviceType = "wifiSta";
    TestNCNetworkDhcp::Instance()->proxy()->checkDhcpClientStatus(deviceType);
}

TEST(TestNCNetworkDhcp, checkDhcpServerStatus)
{
    TestNCNetworkDhcp::Instance()->proxy()->checkDhcpServerStatus();
}

TEST(TestNCNetworkDhcp, onReceiveMessage)
{
    NCString deviceType = "wifiSta";
    UINT32 result = NC_NOERROR;

    nutshell::networkmanager::NDNetworkDhcpResult message;
    message.set_device(std::string(deviceType.getString()));
    message.set_result(result);
    std::string param("");
    message.SerializeToString(&param);

    // Receive message of request ip
    NEMessage msg;
    msg.setType(NEMessage::TYPE_NOTIFY);
    msg.setMethodId(MethodId_RequestIp);
    msg.setParam(param);
    TestNCNetworkDhcp::Instance()->proxy()->onReceiveMessage(msg);
    
    // Receive message of stop request ip
    msg.setMethodId(MethodId_ReleaseIp);
    TestNCNetworkDhcp::Instance()->proxy()->onReceiveMessage(msg);

    // Receive message of start dhcp server
    msg.setMethodId(MethodId_StartDhcpServer);
    TestNCNetworkDhcp::Instance()->proxy()->onReceiveMessage(msg);

    // Receive message of stop dhcp server
    msg.setMethodId(MethodId_StopDhcpServer);
    TestNCNetworkDhcp::Instance()->proxy()->onReceiveMessage(msg);
}

/* EOF */

