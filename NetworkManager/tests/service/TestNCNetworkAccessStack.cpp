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
#include "NCNetworkAccessStack.h"

namespace nutshell 
{
    NCNetworkAccessStack* testAccessStack = NULL;

    TEST(TestNCNetworkAccessStack, init)
    {
        testAccessStack = new NCNetworkAccessStack();
    }

    TEST(TestNCNetworkAccessStack, getRequestMap)
    {
        if (NULL != testAccessStack) {
            AccessParameter param;
            UINT32 type = 1;
            UINT32 reqId = 2;
            testAccessStack->getRequestMap(type, reqId, param);
        }
    }

    TEST(TestNCNetworkAccessStack, updateCurrRouteInfo)
    {
        if (NULL != testAccessStack) {
            DeviceRouteDnsInfo info;
            testAccessStack->updateCurrRouteInfo(info);
        }
    }

    TEST(TestNCNetworkAccessStack, getCurrRouteInfo)
    {
        if (NULL != testAccessStack) {
            DeviceRouteDnsInfo info;
            testAccessStack->getCurrRouteInfo(info);
        }
    }

    TEST(TestNCNetworkAccessStack, addResourceId)
    {
        if (NULL != testAccessStack) {
            NCString resourceId = "123456";
            NCString deviceType = "dcm";
            testAccessStack->addResourceId(resourceId, deviceType);
        }
    }
    
    TEST(TestNCNetworkAccessStack, getResourceId)
    {
        if (NULL != testAccessStack) {
            NCString resourceId = "123456";
            NCString deviceType;
            testAccessStack->getResourceId(resourceId, deviceType);
        }
    }

    TEST(TestNCNetworkAccessStack, findResourceId)
    {
        if (NULL != testAccessStack) {
            NCString resourceId = "123456";
            testAccessStack->findResourceId(resourceId);
        }
    }

    TEST(TestNCNetworkAccessStack, deinit)
    {
        if (testAccessStack != NULL) {
            delete testAccessStack;
            testAccessStack = NULL;
        }
    }
    
} /* namespace nutshell */
/* EOF */