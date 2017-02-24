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
#include "NCNetworkReq.h"
#include "NCNetworkThread.h"

using namespace nutshell;

NCNetworkThread* testThread = new NCNetworkThread(NCNETWORK_REQ_THREAD);

TEST(TestNCNetworkThread, insertRequest)
{
    if (NULL != testThread) {
        testThread->insertRequest(new NCNetworkInitResolvConfReq());
    }
}

TEST(TestNCNetworkThread, stop)
{
    if (NULL != testThread) {
        testThread->stopThread();
    }
}

TEST(TestNCNetworkThread, onDeinit)
{
    if (NULL != testThread) {
        delete testThread;
        testThread = NULL;
    }
}


/* EOF */
