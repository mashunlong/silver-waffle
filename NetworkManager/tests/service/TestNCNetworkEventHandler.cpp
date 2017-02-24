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
#include <nceventsys/NCEvent.h>
#include <nceventsys/NCEvSysContext.h>
#include <nhinterface/NINetworkHandler.h>
#include <nhinterface/NIAvcLanHandler.h>
#include <nhinterface/NIPnpHandler.h>
#include "NDNetworkEvent_generated.h"
#include "NDAvcLanEvent_generated.h"
#include "NDPnpEvent_generated.h"
#include "NCNetworkManager.h"
#include "NCNetworkDeviceType.h"
#include "NCNetworkReq.h"
#include "NCNetworkThread.h"
#include "NCNetworkAvcUnit.h"
#include "NCNetworkEventHandler.h"

using namespace nutshell;

/**
 * TestNCNetworkEventHandler.
 *
 * gtest for TestNCNetworkEventHandler.
 */
class TestNCNetworkEventHandler: public testing::Test
{

protected:
    static ncsp<NCNetworkManager>::sp m_service;

    void SetUp()
    {
        printf("TestNCNetworkEventHandler set-up\n");
    }

    void TearDown()
    {
        printf("TestNCNetworkEventHandler tear-down\n");
    }

    static void SetUpTestCase()
    {
        printf("TestNCNetworkEventHandler::SetUpTestCase case\n");
        m_service = new NCNetworkManager();
        if (m_service != NULL) {
            printf("TestNCNetworkEventHandler::SetUpTestCase, start service \n");
            m_service->onInit();
        }
    }

    static void TearDownTestCase()
    {
        printf("TestNCNetworkEventHandler::TearDownTestCase, stop service\n");
        if (m_service != NULL) {
            printf("TestNCNetworkEventHandler::SetUpTestCase, end service \n");
            m_service->onDeinit();
            m_service = NULL;
        }
    }
};

ncsp<NCNetworkManager>::sp TestNCNetworkEventHandler::m_service = NULL;

NCRunnableThread* testEventThread = new NCRunnableThread();
NCNetworkEventHandler* testEventHandler = new NCNetworkEventHandler(testEventThread->looper());

TEST_F(TestNCNetworkEventHandler, doDhcp)
{
    if (NULL != testEventHandler) {
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_DoDhcp;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);

        UINT32 result = NC_NOERROR;
        NCString device = "wlan0";
        NCString gateway = "192.168.45.1";
        NCString dns1 = "192.168.45.2";
        NCString dns2 = "192.168.45.4";

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkDoDhcpBuilder mb(fbb);
        mb.add_result(result);

        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);

        auto autoGateway = fbb.CreateString((char*)gateway.getString());
        mb.add_gateway(autoGateway);

        auto autoDns1 = fbb.CreateString((char*)dns1.getString());
        mb.add_dns1(autoDns1);

        auto autoDns2 = fbb.CreateString((char*)dns2.getString());
        mb.add_dns2(autoDns2);

        fbb.Finish(mb.Finish());
        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());

        testEventHandler->handleEvent(*event);
        evSysContext.releaseEvent(event);
    }
}

TEST_F(TestNCNetworkEventHandler, releaseDhcpLease)
{
    if (NULL != testEventHandler) {
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_ReleaseDhcpLease;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);

        UINT32 result = NC_NOERROR;
        NCString device = "wlan0";
        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkReleaseDhcpLeaseBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());
        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());

        testEventHandler->handleEvent(*event);
        evSysContext.releaseEvent(event);
    }
}

TEST_F(TestNCNetworkEventHandler, startDhcpServer)
{
    if (NULL != testEventHandler) {
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_StartDhcpServer;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);

        UINT32 result = NC_NOERROR;
        NCString device = "wlan0";
        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkStartDhcpServerBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());
        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());

        testEventHandler->handleEvent(*event);
        evSysContext.releaseEvent(event);
    }
}

TEST_F(TestNCNetworkEventHandler, stopDhcpServer)
{
    if (NULL != testEventHandler) {
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_StopDhcpServer;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);

        UINT32 result = NC_NOERROR;
        NCString device = "wlan0";
        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkStopDhcpServerBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());
        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());

        testEventHandler->handleEvent(*event);
        evSysContext.releaseEvent(event);
    }
}

TEST_F(TestNCNetworkEventHandler, setStatciIp)
{
    if (NULL != testEventHandler) {
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_SetStaticIp;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);

        UINT32 result = NC_NOERROR;
        NCString device = "wlan0";
        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkSetStaticIpBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());
        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());

        testEventHandler->handleEvent(*event);
        evSysContext.releaseEvent(event);
    }
}

TEST_F(TestNCNetworkEventHandler, removeDefaultRoute)
{
    if (NULL != testEventHandler) {
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_RemoveDefaultRoute;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);

        UINT32 result = NC_NOERROR;
        NCString device = "wlan0";
        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkRemoveDefaultRouteBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());
        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());

        testEventHandler->handleEvent(*event);
        evSysContext.releaseEvent(event);
    }
}

TEST_F(TestNCNetworkEventHandler, addDefaultRoute)
{
    if (NULL != testEventHandler) {
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_AddDefaultRoute;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);

        UINT32 result = NC_NOERROR;
        NCString device = "wlan0";
        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkAddDefaultRouteBuilder mb(fbb);
        mb.add_result(result);
        auto name = fbb.CreateString((char*)device.getString());
        mb.add_name(name);
        fbb.Finish(mb.Finish());
        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());

        testEventHandler->handleEvent(*event);
        evSysContext.releaseEvent(event);
    }
}

TEST_F(TestNCNetworkEventHandler, networkDeviceInfo)
{
    if (NULL != testEventHandler) {
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_NetworkDeviceInfo;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);

        UINT32 result = NC_NOERROR;
        UINT32 prefixLength = 24;
        NCString device = "wlan0";
        NCString ipString = "192.168.45.20";
        NCString macString = "9a:bc:0a:34:56:7c";
        NCString gatewayString = "192.168.45.1";
        NCString dns1String = "192.168.45.2";
        NCString dns2String = "192.168.45.3";
        NCString serverString = "192.168.45.4";

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkDeviceBuilder mb(fbb);
        mb.add_result(result);
        mb.add_prefixLength(prefixLength);
        auto name = fbb.CreateString((char*)device.getString());
        auto ip = fbb.CreateString((char*)ipString.getString());
        auto mac = fbb.CreateString((char*)macString.getString());
        auto gateway = fbb.CreateString((char*)gatewayString.getString());
        auto dns1 = fbb.CreateString((char*)dns1String.getString());
        auto dns2 = fbb.CreateString((char*)dns2String.getString());
        auto autoServer = fbb.CreateString((char*)serverString.getString());


        mb.add_name(name);
        mb.add_ip(ip);
        mb.add_mac(mac);
        mb.add_gateway(gateway);
        mb.add_dns1(dns1);
        mb.add_dns2(dns2);
        mb.add_serverId(autoServer);
        fbb.Finish(mb.Finish());
        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());

        testEventHandler->handleEvent(*event);
        evSysContext.releaseEvent(event);
    }
}

TEST_F(TestNCNetworkEventHandler, ConnectedClientInfo)
{
    if (NULL != testEventHandler) {
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        UINT32 evnetId = NINetworkHandler::EventId_ConnectedClientInfo;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);

        UINT32 result = NC_NOERROR;
        NCString macString = "123456";
        NCString ipString = "192.168.45.20";
        NCString nameString = "android_test";

        flatbuffers::FlatBufferBuilder fbb;
        NDNetworkClientBuilder mb(fbb);
        mb.add_result(result);
        auto mac = fbb.CreateString((char*)macString.getString());
        auto ip = fbb.CreateString((char*)ipString.getString());
        auto hostName = fbb.CreateString((char*)nameString.getString());
        mb.add_mac(mac);
        mb.add_ip(ip);
        mb.add_hostName(hostName);

        fbb.Finish(mb.Finish());
        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());

        testEventHandler->handleEvent(*event);
        evSysContext.releaseEvent(event);
    }
}

TEST_F(TestNCNetworkEventHandler, ConnectStatus)
{
    if (NULL != testEventHandler) {
        NCEvSysContext evSysContext;
        NCString eventSpace(GET_NIEVENTNAMESPACE(NIAvcLanHandler));
        UINT32 evnetId = NIAvcLanHandler::EventId_ConnectStatus;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, evnetId);

        flatbuffers::FlatBufferBuilder fbb;
        NDExtAvcConnectStatusBuilder mb(fbb);

        mb.add_connectStatus((NDExtAvcStatus)NIAvcLanHandler::ConnectStatus_AuthenticationSuccessful);
        fbb.Finish(mb.Finish());
        event->setExtraData(fbb.GetBufferPointer(), fbb.GetSize());

        testEventHandler->handleEvent(*event);
        evSysContext.releaseEvent(event);
    }
}

TEST_F(TestNCNetworkEventHandler, doDeviceInfo)
{
    NCString deviceName = "wlan0";
    NCNetworkDataReplier::NetworkDeviceInfo info;
    UINT32 result = 0;

    NCString deviceType = "wifiSta";
    NCNetworkManager::Instance()->getNetworkDeviceType()->add(deviceName, deviceType);

    testEventHandler->doDeviceInfo(deviceName, info, result);
    result = 1;
    testEventHandler->doDeviceInfo(deviceName, info, result);

    AccessParameter param;
    param.resourceId = "123456";
    param.deviceType = "wifiSta";
    param.dealType = 1;
    param.commPriority = 1;
    NCNetworkManager::Instance()->getNetworkThread()->curReq = new NCNetworkRequestAccessReq(param);

    result = 0;
    testEventHandler->doDeviceInfo(deviceName, info, result);
    result = 1;
    testEventHandler->doDeviceInfo(deviceName, info, result);
}

TEST_F(TestNCNetworkEventHandler, doDhcpEvent)
{
    NCString deviceName = "wlan0";
    NCString gateway = "192.168.1.1";
    NCString dns1 = "192.168.1.1";
    NCString dns2 = "192.168.1.2";
    UINT32 result = 0;

    NCString deviceType = "wifiSta";
    NCNetworkManager::Instance()->getNetworkDeviceType()->add(deviceName, deviceType);
    NCNetworkManager::Instance()->getNetworkThread()->curReq = new NCNetworkRequestIpReq("123456", deviceType, deviceName);

    testEventHandler->doDhcpEvent(deviceName, gateway, dns1, dns2, result);
    result = 1;
    testEventHandler->doDhcpEvent(deviceName, gateway, dns1, dns2, result);
}

TEST_F(TestNCNetworkEventHandler, doReleaseIpEvent)
{
    NCString deviceName = "wlan0";
    UINT32 result = 0;

    NCString deviceType = "wifiSta";
    NCNetworkManager::Instance()->getNetworkDeviceType()->add(deviceName, deviceType);
    NCNetworkManager::Instance()->getNetworkThread()->curReq = new NCNetworkReleaseIpReq(deviceType, deviceName);

    testEventHandler->doReleaseIpEvent(deviceName, result);
    result = 1;
    testEventHandler->doReleaseIpEvent(deviceName, result);
}

TEST_F(TestNCNetworkEventHandler, doAddDefaultRoute)
{
    NCString deviceName = "wlan0";
    UINT32 result = 0;

    NCString deviceType = "wifiSta";
    NCNetworkManager::Instance()->getNetworkDeviceType()->add(deviceName, deviceType);
    NCNetworkManager::Instance()->getNetworkAvcUnit()->setFlag(NC_TRUE);

    testEventHandler->doAddDefaultRoute(deviceName, result);

    AccessParameter param;
    NCNetworkManager::Instance()->getNetworkThread()->curReq = new NCNetworkRequestAccessReq(param);
    testEventHandler->doAddDefaultRoute(deviceName, result);

    result = 1;
    testEventHandler->doAddDefaultRoute(deviceName, result);
}

TEST_F(TestNCNetworkEventHandler, doRmDefaultRoute)
{
    NCString deviceName = "usb0";
    UINT32 result = 0;

    NCString deviceType = "dcm";
    NCNetworkManager::Instance()->getNetworkDeviceType()->add(deviceName, deviceType);
    NCNetworkManager::Instance()->getNetworkThread()->curReq = new NCNetworkClearDefaultNetworkReq(RequestType_Disconnect, deviceType, 0);

    testEventHandler->doRmDefaultRoute(deviceName, result);
    result = 1;
    testEventHandler->doRmDefaultRoute(deviceName, result);
}

TEST_F(TestNCNetworkEventHandler, deinit)
{
    if (NULL != testEventHandler) {
        delete testEventHandler;
        testEventHandler = NULL;
    }

    if (NULL != testEventThread) {
        delete testEventThread;
        testEventThread = NULL;
    }
}

/* EOF */
