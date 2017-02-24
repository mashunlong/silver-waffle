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
#include "NCHttpClients.h"

using namespace nutshell;

/**
 * TestNCHttpClients.
 *
 * gtest for TestNCHttpClients.
 */
class TestNCHttpClients: public testing::Test
{

protected:
    NCHttpClients* httpClients;

    void SetUp()
    {
        printf("TestNCHttpClients set-up\n");
        httpClients = new NCHttpClients();
    }

    void TearDown()
    {
        printf("TestNCHttpClients tear-down\n");
        if (NULL != httpClients) {
            delete httpClients;
            httpClients = NULL;
        }
    }

    static void SetUpTestCase()
    {
        printf("TestNCHttpClients set-up test case\n");
    }

    static void TearDownTestCase()
    {
        printf("TestNCHttpClients tear-down test case\n");
    }
};

TEST_F(TestNCHttpClients, getAccessPermission)
{
    if (NULL != httpClients) {
        UINT32 pID = 1024;
        UINT32 tID = 1566;
        NCString taskName  = "testName";

        NCParcel data, reply;
        data.writeCString(taskName.getString());
        data.writeInt32(pID);
        data.writeInt32(tID);
        data.setDataPosition(0);
        httpClients->onCommand(FunctionId_RequestHttpClientAccess, data, &reply, 0);
    }
}

TEST_F(TestNCHttpClients, releaseAccessPermission)
{
    if (NULL != httpClients) {
        UINT32 pID = 1024;
        UINT32 tID = 1566;

        NCParcel data, reply;
        data.writeInt32(pID);
        data.writeInt32(tID);
        data.setDataPosition(0);
        httpClients->onCommand(FunctionId_ReleaseHttpClientAccess, data, &reply, 0);
    }
}

TEST_F(TestNCHttpClients, setLogFileFlag)
{
    if (NULL != httpClients) {
        NC_BOOL flag = NC_TRUE;
        NCParcel data, reply;
        data.writeInt32(flag);
        data.setDataPosition(0);
        httpClients->onCommand(FunctionId_SetHttpLogFlag, data, &reply, 0);

        flag = NC_FALSE;
        data.writeInt32(flag);
        data.setDataPosition(0);
        httpClients->onCommand(FunctionId_SetHttpLogFlag, data, &reply, 0);
    }
}

TEST_F(TestNCHttpClients, testDefault)
{
    if (NULL != httpClients) {
        NCParcel data, reply;
        data.setDataPosition(0);
        httpClients->onCommand(9946, data, &reply, 0);
    }
}

TEST_F(TestNCHttpClients, getLogFileFlag)
{
    if (NULL != httpClients) {
        NCParcel data, reply;
        httpClients->onCommand(FunctionId_GetHttpLogFlag, data, &reply, 0);
    }
}

TEST_F(TestNCHttpClients, dumpService)
{
    if (NULL != httpClients) {
        httpClients->dumpService();
    }
}

TEST_F(TestNCHttpClients, getPIDList)
{
    if (NULL != httpClients) {
        std::vector<INT32> pIDList;
        httpClients->getPIDList(pIDList);
    }
}

TEST_F(TestNCHttpClients, isPIDActive)
{
    if (NULL != httpClients) {
        std::vector<INT32> pIDList;
        INT32 PID = 1130;
        httpClients->isPIDActive(pIDList, PID);

        pIDList.push_back(1130);
        httpClients->isPIDActive(pIDList, PID);
    }
}

/* EOF */