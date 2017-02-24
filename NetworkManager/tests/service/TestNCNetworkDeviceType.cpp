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
#include "NCNetworkDeviceType.h"

namespace nutshell
{
    NCNetworkDeviceType* testDeviceType = NULL;

    TEST(TestNCNetworkDeviceType, init)
    {
        testDeviceType = new NCNetworkDeviceType();
    }

    TEST(TestNCNetworkDeviceType, add)
    {
        NCString deviceName = "wlan0";
        NCString deviceType = "wifiSta";
        if (NULL != testDeviceType) {
            testDeviceType->add(deviceName, deviceType);

            NCString testName;
            NC_BOOL nameStatus = testDeviceType->getName(deviceType, testName);
            EXPECT_TRUE(nameStatus);
            EXPECT_STREQ(deviceName, testName);

            NCString testType;
            NC_BOOL typeStatus = testDeviceType->getType(deviceName, testType);
            EXPECT_TRUE(typeStatus);
            EXPECT_STREQ(deviceType, testType);
        }
    }

    TEST(TestNCNetworkDeviceType, remove)
    {
        NCString deviceName = "wlan0";
        if (NULL != testDeviceType) {
            testDeviceType->remove(deviceName);

            NCString testType;
            NC_BOOL typeStatus = testDeviceType->getType(deviceName, testType);
            EXPECT_FALSE(typeStatus);
        }
    }

    TEST(TestNCNetworkDeviceType, deinit)
    {
        if (NULL != testDeviceType) {
            delete testDeviceType;
            testDeviceType = NULL;
        }
    }

} /* namespace nutshell */
/* EOF */
