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
#include <ncore/NCDir.h>
#include <ncore/NCFile.h>
#include "NHNetworkReq.h"

namespace nutshell
{

    NCQueuingThread<NHNetworkReqBase>* myReqThread  = NULL;
    
    TEST(TestNHNetworkReq, initThread)
    {
        myReqThread = new NCQueuingThread<NHNetworkReqBase>("NHNETWORK_REQ_THREAD");
        if (NULL == myReqThread) {
            printf("New req thread failed\n");
        }
    }

    TEST(TestNHNetworkReq, initFileConfig)
    {

    }

    TEST(TestNHNetworkReq, StopDhcpReq)
    {
        NCString deviceName = "wlan0";
        NHNetworkStopDhcpReq myReq(deviceName);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }


    TEST(TestNHNetworkReq, SetStaticIpReq)
    {
        NCString deviceName = "wlan0";
        NCString ip = "192.168.2.1";
        NHNetworkSetStaticIpReq myReq(deviceName, ip);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, SetMaskReq)
    {
        NCString deviceName = "wlan0";
        UINT32 prefixLength = 24;
        NHNetworkSetMaskReq myReq(deviceName, prefixLength);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, SetMtuReq)
    {
        NCString deviceName = "wlan0";
        UINT32 mtu = 1111;
        NHNetworkSetMtuReq myReq(deviceName, mtu);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, SetGatewayReq)
    {
        NCString deviceName = "wlan0";
        NCString gateway = "192.168.2.1";
        NHNetworkSetGatewayReq myReq(deviceName, gateway);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, RemoveDefauteRouteReq)
    {
        NCString deviceName = "wlan0";
        NHNetworkRemoveDefauteRouteReq myReq(deviceName);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, AddDefauteRouteReq)
    {
        NCString deviceName = "wlan0";
        NCString gateway = "192.168.2.1";
        NHNetworkAddDefauteRouteReq myReq(deviceName, gateway);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, AddRouteReq)
    {
        NCString deviceName = "wlan0";
        NHNetworkRouteInfo info;
        NHNetworkAddRouteReq myReq(deviceName, info);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, RemoveRouteReq)
    {
        NCString deviceName = "wlan0";
        NHNetworkRouteInfo info;
        NHNetworkRemoveRouteReq myReq(deviceName, info);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, SetFirewallDefaultRuleReq)
    {
        NHNetworkSetFirewallDefaultRuleReq myReq;
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, OpenFirewallPortReq)
    {
        NCString protocol = "udp";
        UINT32 port = 53;
        NHNetworkOpenFirewallPortReq myReq(protocol, port);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, CloseFirewallPortReq)
    {
        NCString protocol = "udp";
        UINT32 port = 53;
        NHNetworkCloseFirewallPortReq myReq(protocol, port);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, SetNatTableReq)
    {
        UINT32 mode = 1;
        NCString deviceName = "";
        NHNetworkSetNatTableReq myReq(mode, deviceName);
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    // TEST(TestNHNetworkReq, DeviceInfoReq)
    // {
    //     NCString deviceName = "eth0";
    //     NHNetworkDeviceInfoReq myReq(deviceName);
    //     if (NULL != myReqThread) {
    //         myReq.doAction(myReqThread);
    //     }
    // }

    // TEST(TestNHNetworkReq, ConnectedClientInfoReq)
    // {
    //     NCString mac = "11:23:34:45:56:67";
    //     NHNetworkConnectedClientInfoReq myReq(mac);
    //     if (NULL != myReqThread) {
    //         myReq.doAction(myReqThread);
    //     }
    // }

    // TEST(TestNHNetworkReq, StartDhcpServerReq)
    // {
    //     NCString deviceName = "eth0";
    //     NHNetworkStartDhcpServerReq myReq(deviceName, "192.168.45.1", 24);
    //     if (NULL != myReqThread) {
    //         myReq.doAction(myReqThread);
    //     }
    // }

    // TEST(TestNHNetworkReq, StopDhcpServerReq)
    // {
    //     NCString deviceName = "eth0";
    //     NHNetworkStopDhcpServerReq myReq(deviceName);
    //     if (NULL != myReqThread) {
    //         myReq.doAction(myReqThread);
    //     }
    // }

    TEST(TestNHNetworkReq, StartDnsCacheServerReq)
    {
        NHNetworkStartDnsCacheServerReq myReq;
        if (NULL != myReqThread) {
            myReq.doAction(myReqThread);
        }
    }

    TEST(TestNHNetworkReq, deinit)
    {
        if (NULL != myReqThread) {
            delete myReqThread;
            myReqThread = NULL;
        }
    }

} /* namespace nutshell */
/* EOF */