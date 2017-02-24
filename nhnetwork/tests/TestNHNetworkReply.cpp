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
#include "nhnetwork/NHNetworkDefine.h"
#include "NHNetworkReply.h"

namespace nutshell
{
    NHNetworkReply* testReply = NULL;

    TEST(TestNHNetworkReply, init)
    {
        testReply = new NHNetworkReply();
    }

    TEST(TestNHNetworkReply, notifyStopDhcpResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyStopDhcpResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifyReleaseDhcpLeaseResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyReleaseDhcpLeaseResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifySetStaticIpResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifySetStaticIpResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifySetMaskResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifySetMaskResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifySetMtuResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifySetMtuResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifyAddDefaultRouteResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyAddDefaultRouteResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifyRemoveDefaultRouteResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyRemoveDefaultRouteResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifyAddRouteResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyAddRouteResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifyRemoveRouteResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyRemoveRouteResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifySetFilterTableDefaultRuleResult)
    {
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifySetFilterTableDefaultRuleResult(result);
        }
    }

    TEST(TestNHNetworkReply, notifyAddFilterTableRuleResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyAddFilterTableRuleResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifyRemoveFilterTableRuleResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyRemoveFilterTableRuleResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifySetNatTableRuleResult)
    {
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifySetNatTableRuleResult(result);
        }
    }

    TEST(TestNHNetworkReply, notifyNetworkDeviceInfoResult)
    {
        NHNetworkDeviceInfo info;
        info.device = "wlan0";
        info.ip = "192.168.45.1";
        info.mac = "0a:34:22:44:b2:23";
        info.prefixLength = 24;
        info.gateway = "192.168.45.1";
        info.dns1 = "192.168.45.1";
        info.dns2 = "192.168.45.2";
        info.serverId = "192.168.45.3";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyNetworkDeviceInfoResult(info, result);
        }
    }

    TEST(TestNHNetworkReply, notifyConnectedClientInfoResult)
    {
        NHNetworkConnectedClientInfo info;
        info.mac = "11:22:33:44:55:66";
        info.ip = "192.168.45.1";
        info.hostName = "hostname";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyConnectedClientInfoResult(info, result);
        }
    }

    TEST(TestNHNetworkReply, notifyStartDhcpServerResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyStartDhcpServerResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, notifyStopDhcpServerResult)
    {
        NCString deviceName = "wlan0";
        UINT32 result = NC_NOERROR;
        if (NULL != testReply) {
            testReply->notifyStopDhcpServerResult(deviceName, result);
        }
    }

    TEST(TestNHNetworkReply, deinit)
    {
        if (NULL != testReply) {
            delete testReply;
            testReply = NULL;
        }
    }
    
} /* namespace nutshell */
/* EOF */