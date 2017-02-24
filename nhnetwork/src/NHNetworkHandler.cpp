/**
 * Copyright @ 2013 - 2014 Suntec Software(Shanghai) Co., Ltd.
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

#include <ncore/NCLog.h>
#include <nhbase/NHDevice.h>
#include <nceventsys/NCEvSysContext.h>
#include <cutils/properties.h>
#include "nhnetwork/NHNetworkDefine.h"
#include "NHNetworkTimerThread.h"
#include "NHNetworkDhcpInfo.h"
#include "nhnetwork/NHNetworkHandler.h"

#define NC_LOG_TAG "NHNetworkHandler"

namespace nutshell
{
    NHNetworkHandler* NHNetworkHandler::m_networkHandler = NULL;

    NHNetworkHandler::NHNetworkHandler(NHDevice* device)
    : NINetworkHandlerStub(device)
    , m_reqThread(NULL)
    , m_timerThread(NULL)
    , m_dhcpInfo(new NHNetworkDhcpInfo())
    , m_bDhcpd_firstInit(NC_FALSE)
    {
        m_networkHandler = this;
    }

    NHNetworkHandler::~NHNetworkHandler()
    {
        if (NULL != m_reqThread) {
            delete m_reqThread;
            m_reqThread = NULL;
        }

        if (NULL != m_dhcpInfo) {
            delete m_dhcpInfo;
            m_dhcpInfo = NULL;
        }
    }

    VOID
    NHNetworkHandler::onInitialize()
    {
        NCLOGD("NHNetworkHandler::onInitialize");
        m_reqThread = new NCQueuingThread<NHNetworkReqBase>("NHNETWORK_REQ_THREAD");
        if (NULL == m_reqThread) {
            NCLOGD("NHNetworkHandler::onInitialize - New NCQueuingThread failed!");
        }

        if (NULL == m_timerThread) {
            m_timerThread = new NHNetworkTimerThread();
        }
    }

    VOID
    NHNetworkHandler::onStart()
    {
        NCLOGD("NHNetworkHandler::onStart");
    }

    VOID
    NHNetworkHandler::onStop()
    {
        NCLOGD("NHNetworkHandler::onStop");
        if (NULL != m_timerThread) {
            m_timerThread->stopThread();
        }
    }

    VOID
    NHNetworkHandler::onFinalize()
    {
        NCLOGD("NHNetworkHandler::onFinalize");
        if (NULL != m_timerThread) {
            delete m_timerThread;
            m_timerThread = NULL;
        }
    }

    VOID
    NHNetworkHandler::onStartUp(void* buff, uint32_t size)
    {
        NCLOGD("NHNetworkHandler::onStartUp");
        // Set property for AvcLan IP
        setAvclanIpProperty();

        

        // Start named
        startDnsCacheServer();

        // if netd crash and restart, do something
        checkNetdCrash();
    }

    VOID
    NHNetworkHandler::onHibernate(void* buff, uint32_t size)
    {
        NCLOGD("NHNetworkHandler::onHibernate - Start");
        // Set property for AvcLan IP
        setAvclanIpProperty();

        // Start named
        startDnsCacheServer();
    }

    VOID
    NHNetworkHandler::onAwake(void* buff, uint32_t size)
    {
        NCLOGD("NHNetworkHandler::onAwake - Start");
    }

    NC_ERROR
    NHNetworkHandler::doDhcpRequest(const NCString& device)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkDoDhcpReq* req = new NHNetworkDoDhcpReq(device);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::doDhcpRequest - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::stopDhcpRequest(const NCString& device)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkStopDhcpReq* req = new NHNetworkStopDhcpReq(device);
        if (NULL != m_reqThread) {
            NCLOGD("NHNetworkHandler::stopDhcpRequest - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::releaseDhcpLease(const NCString& device)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkReleaseDhcpLeaseReq* req = new NHNetworkReleaseDhcpLeaseReq(device);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::releaseDhcpLease - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::setStaticIp(const NCString& device, const NCString& staticIp)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkSetStaticIpReq* req = new NHNetworkSetStaticIpReq(device, staticIp);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::setStaticIp - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::setMask(const NCString& device, const UINT32& prefixLength)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkSetMaskReq* req = new NHNetworkSetMaskReq(device, prefixLength);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::setMask - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::setMtu(const NCString& device, const UINT32 mtu)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkSetMtuReq* req = new NHNetworkSetMtuReq(device, mtu);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::setMtu - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::setStaticIpMaskMtu(const NCString& device, const NCString& staticIp, const UINT32& prefixLength, const UINT32& mtu)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkSetStaticIpMaskMtuReq* req = new NHNetworkSetStaticIpMaskMtuReq(device, staticIp, prefixLength, mtu);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::setStaticIpMaskMtu - Start, deviceName [%s], ip [%s], prefixLength [%d] mtu [%d]", device.getString(), staticIp.getString(), prefixLength, mtu);
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::setGateway(const NCString& device, const NCString& gateway)
    {
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::addDefaultRoute(const NCString& device, const NCString& gateway)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkAddDefauteRouteReq* req = new NHNetworkAddDefauteRouteReq(device, gateway);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::addDefaultRoute - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::removeDefaultRoute(const NCString& device)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkRemoveDefauteRouteReq* req = new NHNetworkRemoveDefauteRouteReq(device);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::removeDefaultRoute - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::addRoute(const NCString& device, const struct RouteInfo& route)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkRouteInfo routeInfo;
        IPADDR_ARRAY_TO_STRING(routeInfo.destination, route.destination);
        IPADDR_ARRAY_TO_STRING(routeInfo.gateway, route.gateway);
        routeInfo.prefixLength = route.prefixLength;

        NHNetworkAddRouteReq* req = new NHNetworkAddRouteReq(device, routeInfo);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::addRoute - Start, device = [%s], prefixLength = [%d]", device.getString(), routeInfo.prefixLength);
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::removeRoute(const NCString& device, const struct RouteInfo& route)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkRouteInfo routeInfo;
        IPADDR_ARRAY_TO_STRING(routeInfo.destination, route.destination);
        IPADDR_ARRAY_TO_STRING(routeInfo.gateway, route.gateway);
        routeInfo.prefixLength = route.prefixLength;

        NHNetworkRemoveRouteReq* req = new NHNetworkRemoveRouteReq(device, routeInfo);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::removeRoute - Start", device.getString());
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::setFirewallDefaultRule()
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkSetFirewallDefaultRuleReq* req = new NHNetworkSetFirewallDefaultRuleReq();
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::setFirewallDefaultRule - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::openFirewallPort(const NCString& protocol, const UINT32 port)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkOpenFirewallPortReq* req = new NHNetworkOpenFirewallPortReq(protocol, port);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::openFirewallPort - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::closeFirewallPort(const NCString& protocol, const UINT32 port)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkCloseFirewallPortReq* req = new NHNetworkCloseFirewallPortReq(protocol, port);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::closeFirewallPort - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::setNatTable(enum NatSetMode setMode, const NCString& deviceName)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkSetNatTableReq* req = new NHNetworkSetNatTableReq(setMode, deviceName);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::setNatTable - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::networkDeviceInfo(const NCString& device)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }

        NHNetworkDeviceInfoReq* req = new NHNetworkDeviceInfoReq(device);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::networkDeviceInfo - Start");
            m_reqThread->postRequest(req);
        }

        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::connectedClientInfo(const NCString& mac)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkConnectedClientInfoReq* req = new NHNetworkConnectedClientInfoReq(mac);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::connectedClientInfo - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::startDhcpServer(const NCString& device, const NCString& ip, const UINT32 prefixLength)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkStartDhcpServerReq* req = new NHNetworkStartDhcpServerReq(device, ip, prefixLength);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::startDhcpServer - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::stopDhcpServer(const NCString& device)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkStopDhcpServerReq* req = new NHNetworkStopDhcpServerReq(device);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::stopDhcpServer - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::restartDhcpServer(const NCString& device)
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkRestartDhcpServerReq* req = new NHNetworkRestartDhcpServerReq(device);
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::restartDhcpServer - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::startDnsCacheServer()
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        NHNetworkStartDnsCacheServerReq* req = new NHNetworkStartDnsCacheServerReq();
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::startDnsCacheServer - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::stopDnsCacheServer()
    {
        if (NULL == m_reqThread) {
            return NI_ERROR_HAL_NETWORK_FAIL;
        }

        NHNetworkStopDnsCacheServerReq* req = new NHNetworkStopDnsCacheServerReq();
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::stopDnsCacheServer - Start");
            m_reqThread->postRequest(req);
        }
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_ERROR
    NHNetworkHandler::updateNamedConf(const NCString& option, const NCString& dns1, const NCString& dns2)
    {
        NCLOGD("NHNetworkHandler::updateNamedConf, Start, option [%s], dns1 [%s], dns2 [%s]", option.getString(), dns1.getString(), dns2.getString());
        if (NULL == m_reqThread) {
            NCLOGD("NHNetworkHandler::updateNamedConf, Req thread is NULL");
            return NI_ERROR_HAL_NETWORK_FAIL;
        }

        if (NCString("options") == option) {
            NHNetworkUpdateRemoteDnsReq* req = new NHNetworkUpdateRemoteDnsReq(dns1, dns2);
            if (NULL != req) {
                m_reqThread->postRequest(req);
                return NI_ERROR_HAL_NETWORK_SUCCESS;
            }
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        else if (NCString("g-book.com.cn") == option) {
            NHNetworkUpdateGbookDnsReq* req = new NHNetworkUpdateGbookDnsReq(dns1, dns2);
            if (NULL != req) {
                m_reqThread->postRequest(req);
                return NI_ERROR_HAL_NETWORK_SUCCESS;
            }
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        else {
            NCLOGD("NHNetworkHandler::updateNamedConf, Option is not defined");
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
    }

    NC_ERROR
    NHNetworkHandler::notifyFirewallFailed()
    {
        NCLOGD("NHNetworkHandler::notifyFirewallFailed, Start");
        NCString eventSpace(GET_NIEVENTNAMESPACE(NINetworkHandler));
        NCEvSysContext evSysContext;
        NCEvent* event = evSysContext.obtainEvent(eventSpace, EventId_FirewallFailed);
        if (NULL != event) {
            event->setSticky(NC_TRUE);
            sendEvent(event);
            evSysContext.releaseEvent(event);
            NCLOGD("NHNetworkHandler::notifyFirewallFailed, broadcast event success");
            return NI_ERROR_HAL_NETWORK_SUCCESS;
        }
        else {
            NCLOGD("NHNetworkHandler::notifyFirewallFailed, Get event failed");
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
    }

    NHNetworkHandler*
    NHNetworkHandler::getNetworkHandler()
    {
        NCLOGD("NHNetworkHandler::getNetworkHandler - Start.");
        if (NULL == m_networkHandler) {
            NCLOGD("NHNetworkHandler::getNetworkHandler - m_networkHandler is NULL!");
        }
        return m_networkHandler;
    }

    NHNetworkDhcpInfo*
    NHNetworkHandler::getDhcpInfoPointer()
    {
        return m_dhcpInfo;
    }

    VOID
    NHNetworkHandler::startTimer(const UINT32 type)
    {
        m_timerThread->startTimer(type);
    }

    VOID
    NHNetworkHandler::stopTimer(const UINT32 type)
    {
        m_timerThread->stopTimer(type);
    }

    VOID
    NHNetworkHandler::postRequest(NHNetworkReqBase* req)
    {
        NCLOGD("NHNetworkHandler::postRequest - Start");
        if (NULL != m_reqThread && NULL != req) {
            m_reqThread->postRequest(req);
        }
        else {
            NCLOGD("NHNetworkHandler::postRequest, failed");
        }
    }

    VOID
    NHNetworkHandler::setAvclanIpProperty()
    {
        NCLOGD("NHNetworkHandler::setAvclanIpProperty - Start");
#if defined NCNETWORK_TARGET_PRODUCT_EMULATOR
        NCLOGD("NHNetworkHandler::setAvclanIpProperty - Set property for EMULATOR AvcLan IP");
        property_set("net.localip", "*");
        property_set("net.remoteip", "10.0.2.2");
#endif

#if !defined NCNETWORK_TARGET_PRODUCT_EMULATOR
#if defined NCNETWORK_TARGET_PRODUCT_DCU
        NCLOGD("NHNetworkHandler::setAvclanIpProperty - Set property for DCU AvcLan IP");
        property_set("net.localip", "192.168.128.129");
        property_set("net.remoteip", "192.168.128.130");
#endif

#if defined NCNETWORK_TARGET_PRODUCT_MEU
        NCLOGD("NHNetworkHandler::setAvclanIpProperty - Set property for MEU AvcLan IP");
        property_set("net.localip", "192.168.128.130");
        property_set("net.remoteip", "192.168.128.129");
#endif
#endif
    }

    VOID
    NHNetworkHandler::checkNetdCrash()
    {
        NCLOGD("NHNetworkHandler::checkNetdCrash, Start");
        if (NULL == m_reqThread) {
            NCLOGD("NHNetworkHandler::checkNetdCrash, Request thread is null");
            return;
        }
        
        NHNetworkNetdCrashReq* req = new NHNetworkNetdCrashReq();
        if (NULL != req) {
            NCLOGD("NHNetworkHandler::checkNetdCrash, New NHNetworkNetdCrashReq success");
            m_reqThread->postRequest(req);
        }
    }

    NC_BOOL
    NHNetworkHandler::isDhcpdFirstInit()
    {
        return m_bDhcpd_firstInit;
    }

    VOID
    NHNetworkHandler::setDhcpdFirstInit(const NC_BOOL value)
    {
        m_bDhcpd_firstInit = value;
    }

    DEFINE_DEVICE_HANDLER_CREATOR(NHNetworkHandler);

} /* namespace nutshell */
/* EOF */
