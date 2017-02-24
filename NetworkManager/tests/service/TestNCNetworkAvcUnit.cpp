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
#include <nceventsys/NCRunnableThread.h>
#include <NHAvcProtocolDef.h>
#include <evipservice/NEAvcFilter.h>
#include <evipservice/NEAvcUnit.h>
#include <NDAvcProtocolData_generated.h>
#include <NDAvcNetworkManager_generated.h>
#include "NCNetworkManager.h"
#include "NCNetworkAvcUnit.h"

using namespace nutshell;

/**
 * TestNCNetworkAvcUnit.
 *
 * gtest for TestNCNetworkAvcUnit.
 */
class TestNCNetworkAvcUnit: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkAvcUnit set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkAvcUnit tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkAvcUnit::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkAvcUnit::SetUpTestCase, start service \n");
            m_service->onInit();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkAvcUnit::TearDownTestCase, stop service\n");
        if (m_service != NULL) {
            printf("TestNCNetworkAvcUnit::SetUpTestCase, end service \n");
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkAvcUnit::m_service = NULL;
NCNetworkAvcUnit* testAvcUnit = new NCNetworkAvcUnit();

TEST_F(TestNCNetworkAvcUnit, notifyCommStatusChange)
{
    if (NULL != testAvcUnit) {
        testAvcUnit->notifyCommStatusChange();
    }
}

TEST_F(TestNCNetworkAvcUnit, stop)
{
    if (NULL != testAvcUnit) {
        testAvcUnit->stop();
    }
}

TEST_F(TestNCNetworkAvcUnit, onRecvExtData_dcm)
{
    if (NULL != testAvcUnit) {
        NCString filter = "testfilter";

        flatbuffers::FlatBufferBuilder fbb;
        extavc::NDAvcProtocolDataBuilder pdb(fbb);
        pdb.add_head(extavc::CreateNDAvcDataHeader(fbb,
            NHNetworkManagerOpcCode_NotifyCommStatusChange,
            NHAVC_UINIT_NETWORK_MANAGER2, NHAVC_UINIT_NETWORK_MANAGER1, extavc::NDAvcCommandType_PointToPoint, 0, 0));

        flatbuffers::FlatBufferBuilder fbb2;
        extavc::NDNotifyCommStatusChangeBuilder oob(fbb2);
        UINT32 commDevice = 33554432;
        UINT32 commMode = 33554432;
        oob.add_commDevice(commDevice);
        oob.add_routeStatus(commMode);

        const BYTE wifiSTAType = 0x00;
        const BYTE mirrorlinkStatus = 0x00;
        std::vector<INT8> tmpVecType;
        const INT maxLenInfo = 16;
        for (INT i = 0; i < maxLenInfo-1; ++i) {
            tmpVecType.push_back(0);
        }

        oob.add_wifiSTAType(wifiSTAType);
        oob.add_mirrorlinkStatus(mirrorlinkStatus);
        oob.add_carrierInfo(fbb2.CreateVector(tmpVecType));

        const INT maxLenDns1 = 4;
        std::vector<INT8> tmpVecDns1;
        for (INT i = 0; i < maxLenDns1; ++i) {
            tmpVecDns1.push_back(i+10);
        }

        const INT maxLenDns2 = 4;
        std::vector<INT8> tmpVecDns2;
        for (INT i = 0; i < maxLenDns2; ++i) {
            tmpVecDns2.push_back(i+20);
        }

        oob.add_dns1(fbb.CreateVector(tmpVecDns1));
        oob.add_dns2(fbb.CreateVector(tmpVecDns2));

        fbb2.Finish(oob.Finish());

        pdb.add_data(fbb.CreateString(reinterpret_cast<char*>(
            fbb2.GetBufferPointer()), fbb2.GetSize()));
        fbb.Finish(pdb.Finish());

        NCData outData((char*)fbb.GetBufferPointer(), fbb.GetSize());
        testAvcUnit->onRecvExtData(filter, outData);
    }

}

TEST_F(TestNCNetworkAvcUnit, onRecvExtData_dun)
{
    if (NULL != testAvcUnit) {
        NCString filter = "testfilter";

        flatbuffers::FlatBufferBuilder fbb;
        extavc::NDAvcProtocolDataBuilder pdb(fbb);
        pdb.add_head(extavc::CreateNDAvcDataHeader(fbb,
            NHNetworkManagerOpcCode_NotifyCommStatusChange,
            NHAVC_UINIT_NETWORK_MANAGER2, NHAVC_UINIT_NETWORK_MANAGER1, extavc::NDAvcCommandType_PointToPoint, 0, 0));

        flatbuffers::FlatBufferBuilder fbb2;
        extavc::NDNotifyCommStatusChangeBuilder oob(fbb2);
        UINT32 commDevice = 64;
        UINT32 commMode = 64;
        oob.add_commDevice(commDevice);
        oob.add_routeStatus(commMode);
        fbb2.Finish(oob.Finish());

        pdb.add_data(fbb.CreateString(reinterpret_cast<char*>(
            fbb2.GetBufferPointer()), fbb2.GetSize()));
        fbb.Finish(pdb.Finish());

        NCData outData((char*)fbb.GetBufferPointer(), fbb.GetSize());
        testAvcUnit->onRecvExtData(filter, outData);
    }

}

TEST_F(TestNCNetworkAvcUnit, onRecvExtData_wifiSta)
{
    if (NULL != testAvcUnit) {
        NCString filter = "testfilter";

        flatbuffers::FlatBufferBuilder fbb;
        extavc::NDAvcProtocolDataBuilder pdb(fbb);
        pdb.add_head(extavc::CreateNDAvcDataHeader(fbb,
            NHNetworkManagerOpcCode_NotifyCommStatusChange,
            NHAVC_UINIT_NETWORK_MANAGER2, NHAVC_UINIT_NETWORK_MANAGER1, extavc::NDAvcCommandType_PointToPoint, 0, 0));

        flatbuffers::FlatBufferBuilder fbb2;
        extavc::NDNotifyCommStatusChangeBuilder oob(fbb2);
        UINT32 commDevice = 65536;
        UINT32 commMode = 65536;
        oob.add_commDevice(commDevice);
        oob.add_routeStatus(commMode);
        fbb2.Finish(oob.Finish());

        pdb.add_data(fbb.CreateString(reinterpret_cast<char*>(
            fbb2.GetBufferPointer()), fbb2.GetSize()));
        fbb.Finish(pdb.Finish());

        NCData outData((char*)fbb.GetBufferPointer(), fbb.GetSize());
        testAvcUnit->onRecvExtData(filter, outData);
    }
}

TEST_F(TestNCNetworkAvcUnit, getCommDevice)
{
    testAvcUnit->getCommDevice();
}

TEST_F(TestNCNetworkAvcUnit, getCommMode)
{
    testAvcUnit->getCommMode();
}

TEST_F(TestNCNetworkAvcUnit, updateCommDevice)
{
    testAvcUnit->updateCommDevice(0);
}

TEST_F(TestNCNetworkAvcUnit, setFlag)
{
    testAvcUnit->setFlag(NC_TRUE);
}

TEST_F(TestNCNetworkAvcUnit, getFlag)
{
    testAvcUnit->getFlag();
}

TEST_F(TestNCNetworkAvcUnit, deinit)
{
    if (NULL != testAvcUnit) {
        delete testAvcUnit;
        testAvcUnit = NULL;
    }
}



/* EOF */
