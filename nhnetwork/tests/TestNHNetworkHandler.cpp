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
#include "nhinterface/NINetworkDefine.h"
#include "nhnetwork/NHNetworkHandler.h"

namespace nutshell
{
    /**
     * @brief This class is used to test interfaces in class NHNetworkHandler
     *
     * Google test is used to test the interfaces.
     */
    class NHNetworkHandlerTest : public NHNetworkHandler
    {
    public:
        NHNetworkHandlerTest() : NHNetworkHandler(NULL) 
        {

        }

        virtual ~NHNetworkHandlerTest() 
        {

        }
    };

    NHNetworkHandlerTest* m_handler = NULL;

    TEST(TestNHNetworkHandler, oniit)
    {
        m_handler = new NHNetworkHandlerTest();
    }

    TEST(TestNHNetworkHandler, onInitialize)
    {
        if (NULL != m_handler) {
            m_handler->onInitialize();
        }
    }

    TEST(TestNHNetworkHandler, onStart)
    {
        if (NULL != m_handler) {
            m_handler->onStart();
        }
    }

    TEST(TestNHNetworkHandler, onStartUp)
    {
        if (NULL != m_handler) {
            m_handler->onStartUp(NULL, 0);
        }
    }

    TEST(TestNHNetworkHandler, onHibernate)
    {
        if (NULL != m_handler) {
            m_handler->onHibernate(NULL, 0);
        }
    }

    TEST(TestNHNetworkHandler, onAwake)
    {
        if (NULL != m_handler) {
            m_handler->onAwake(NULL, 0);
        }
    }

    TEST(TestNHNetworkHandler, doDhcpRequest)
    {
        if (NULL != m_handler) {
            NCString deviceName = "wlan0";
            NC_ERROR ret = m_handler->doDhcpRequest(deviceName);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, stopDhcpRequest)
    {
        if (NULL != m_handler) {
            NCString deviceName = "wlan0";
            NC_ERROR ret = m_handler->stopDhcpRequest(deviceName);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, releaseDhcpLease)
    {
        if (NULL != m_handler) {
            NCString deviceName = "wlan0";
            NC_ERROR ret = m_handler->releaseDhcpLease(deviceName);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, setStaticIp)
    {
        if (NULL != m_handler) {
            NCString deviceName = "wlan0";
            NCString staticIp = "192.168.2.111";
            NC_ERROR  ret = m_handler->setStaticIp(deviceName, staticIp);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, setMask)
    {
        NCString deviceName = "wlan0";
        UINT32 prefixLength = 24;
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->setMask(deviceName, prefixLength);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, setMtu)
    {
        NCString deviceName = "wlan0";
        UINT32 mtu = 1111;
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->setMtu(deviceName, mtu);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, setGateway)
    {
        NCString deviceName = "wlan0";
        NCString gateway = "192.168.2.11";
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->setGateway(deviceName, gateway);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, removeDefaultRoute)
    {
        NCString deviceName = "wlan0";
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->removeDefaultRoute(deviceName);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, addDefaultRoute)
    {
        NCString deviceName = "wlan0";
        NCString gateway = "192.168.2.11";
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->addDefaultRoute(deviceName, gateway);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, addRoute)
    {
        NCString deviceName = "wlan0";
        NINetworkHandler::RouteInfo info;
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->addRoute(deviceName, info);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, removeRoute)
    {
        NCString deviceName = "wlan0";
        NINetworkHandler::RouteInfo info;
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->removeRoute(deviceName, info);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, setFirewallDefaultRule)
    {
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->setFirewallDefaultRule();
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, openFirewallPort)
    {
        NCString protocol = "tcp";
        UINT32 port = 4989;
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->openFirewallPort(protocol, port);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, closeFirewallPort)
    {
        NCString protocol = "tcp";
        UINT32 port = 4989;
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->closeFirewallPort(protocol, port);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, setNatTable)
    {
        NINetworkHandler::NatSetMode mode = NINetworkHandler::NatSetMode_DcuAp;
        NCString deviceName = "wlan0";
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->setNatTable(mode, deviceName);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, networkDeviceInfo)
    {
        NCString deviceName = "eth0";
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->networkDeviceInfo(deviceName);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, connectedClientInfo)
    {
        NCString mac = "f0:92:1c:e1:29:94";
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->connectedClientInfo(mac);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, startDhcpServer)
    {
        NCString deviceName = "eth0";
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->startDhcpServer(deviceName, "192.168.45.1", 24);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, stopDhcpServer)
    {
        NCString deviceName = "eth0";
        if (NULL != m_handler) {
            NC_ERROR ret = m_handler->stopDhcpServer(deviceName);
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, ret);
        }
    }

    TEST(TestNHNetworkHandler, startDnsCacheServer)
    {
        if (NULL != m_handler) {
            NC_ERROR result = m_handler->startDnsCacheServer();
            EXPECT_EQ(NI_ERROR_HAL_NETWORK_SUCCESS, result);
        }
    }

    TEST(TestNHNetworkHandler, getNetworkHandler)
    {
        if (NULL != m_handler) {
            m_handler->getNetworkHandler();
        }
    }

    TEST(TestNHNetworkHandler, onStop)
    {
        if (NULL != m_handler) {
            m_handler->onStop();
        }
    }

    TEST(TestNHNetworkHandler, onFinalize)
    {
        if (NULL != m_handler) {
            m_handler->onFinalize();
        }
    }

    TEST(TestNHNetworkHandler, deinit)
    {
        if (NULL != m_handler) {
            delete m_handler;
            m_handler = NULL;
        }
    }

} /* namespace nutshell */
/* EOF */
