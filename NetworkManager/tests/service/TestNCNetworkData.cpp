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
#include "networkmanager/NCNetworkDataReplier.h"
#include "TestNCNetworkData.h"

namespace nutshell
{
    /**
     * @brief The replier class of NCNetworkData.
     *
     * TestNCNetworkDataReplier.
     */
    class TestNCNetworkDataReplier : public NCNetworkDataReplier
    {
    public:
        virtual VOID replyDeviceInfo(const NCString& device, const NetworkDeviceInfo& info)
        {

        }

        virtual VOID replyConnectedClientInfo(const NCString& mac, const ConnectedClientInfo& info)
        {

        }

    };

    TestNCNetworkData* TestNCNetworkData::instance = NULL;
    
    TestNCNetworkData::TestNCNetworkData()
    : m_proxy(NULL)
    {
        m_proxy = new NCNetworkData();
        instance = this;
    }

    TestNCNetworkData::~TestNCNetworkData()
    {

    }

    VOID
    TestNCNetworkData::onInit()
    {
        m_proxy->bindService();
    }

    VOID
    TestNCNetworkData::onDeinit()
    {
        m_proxy->unbindService();
    }

    TestNCNetworkData*
    TestNCNetworkData::Instance()
    {
        return instance;
    }

    NCNetworkData*
    TestNCNetworkData::proxy()
    {
        return m_proxy.get();
    }

} /* namespace nutshell */


using namespace nutshell;

TEST(TestNCNetworkData, deviceInfo)
{
    NCString deviceType = "wifiSta";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkData::Instance()->proxy()->deviceInfo(deviceType));
}

TEST(TestNCNetworkData, connectedClientInfo)
{
    NCString mac = "00:54:9a:10:6d:b3";
    EXPECT_EQ(NC_NOERROR, TestNCNetworkData::Instance()->proxy()->connectedClientInfo(mac));
}

TEST(TestNCNetworkData, currentUseNetwork)
{
    NCString route = TestNCNetworkData::Instance()->proxy()->currentUseNetwork();
    EXPECT_STREQ("", route);
}

TEST(TestNCNetworkData, getDeviceStatus)
{
    NCString deviceType = "dcm";
    TestNCNetworkData::Instance()->proxy()->getDeviceStatus(deviceType);
}

TEST(TestNCNetworkData, networkSignal)
{
    NCString deviceType = "wifiSta";
    EXPECT_FALSE(TestNCNetworkData::Instance()->proxy()->networkSignal(deviceType));
}

TEST(TestNCNetworkData, onReceiveMessage)
{
    // Receive the message of device info
    nutshell::networkmanager::NDNetworkDevice deviceMessage;
    NCString deviceType = "wifiSta";
    NCString ip = "192.168.45.1";
    NCString mac = "06:1e:23:9b:5c:32";
    UINT32 prefixLength = 24;
    NCString gateway = "192.168.45.1";
    NCString serverid = "192.168.45.1";

    deviceMessage.set_name(std::string(deviceType.getString()));
    deviceMessage.set_ip(std::string(ip.getString()));
    deviceMessage.set_mac(std::string(mac.getString()));
    deviceMessage.set_prefixlength(prefixLength);
    deviceMessage.set_gateway(std::string(gateway.getString()));
    deviceMessage.set_serverid(std::string(serverid.getString()));

    std::string deviceParam("");
    deviceMessage.SerializeToString(&deviceParam);

    NEMessage deviceMsg;
    deviceMsg.setType(NEMessage::TYPE_NOTIFY);
    deviceMsg.setMethodId(MethodId_DeviceInfo);
    deviceMsg.setParam(deviceParam);
    TestNCNetworkData::Instance()->proxy()->onReceiveMessage(deviceMsg);

    // Receive the message of connected client info
    nutshell::networkmanager::NDNetworkClient clientMessage;

    NCString name = "hostname";
    clientMessage.set_mac(std::string(mac.getString()));
    clientMessage.set_ip(std::string(ip.getString()));
    clientMessage.set_name(std::string(name.getString()));
    std::string clientParam("");
    clientMessage.SerializeToString(&clientParam);

    NEMessage clientMsg;
    clientMsg.setType(NEMessage::TYPE_NOTIFY);
    clientMsg.setMethodId(MethodId_ConnectedClientInfo);
    clientMsg.setParam(clientParam);
    TestNCNetworkData::Instance()->proxy()->onReceiveMessage(clientMsg);
}

/* EOF */



