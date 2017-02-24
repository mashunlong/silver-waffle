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
#include <signal.h>
#include <ncore/NCConverter.h>
#include <ncore/NCFile.h>
#include <ncore/NCError.h>
#include <netutils/ifc.h>
#include <netutils/dhcp.h>
#include <cutils/properties.h>
#include <nhinterface/NINetworkDefine.h>
#include <nhinterface/NINetworkHandler.h>
#include "nhnetwork/NHNetworkDefine.h"
#include "nhnetwork/NHNetworkHandler.h"
#include "NHNetworkDhcp.h"
#include "NHNetworkReply.h"
#include "NHNetworkDhcpInfo.h"
#include "NHNetworkCheckInterface.h"
#include "NHNetworkReq.h"

#define NC_LOG_TAG "NHNetworkHandler"

#define NHNETWORK_NAMED_PID_FILE "/var/named.pid"
#define NHNETWORK_ETC_NAMED_CONFIG_FILE "/etc/named.conf.orgin"
#define NHNETWORK_TMP_NAMED_CONFIG_FILE "/tmp/named.conf"


namespace nutshell
{
    const UINT NHNETWORK_FILE_PERMISSIONS = 0x0640;

    NHNetworkDoDhcpReq::NHNetworkDoDhcpReq(const NCString& device)
    : m_device(device)
    {

    }

    NHNetworkDoDhcpReq::~NHNetworkDoDhcpReq()
    {

    }

    VOID
    NHNetworkDoDhcpReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkDoDhcpReq::doAction");
        if (!NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->getDhclientTimerStatus()) {
            NCLOGD("NHNetworkDoDhcpReq::doAction, Start timer for dhclient");
            NCFile flagFile(DHCLIENT_FLAG_FILE);
            if (flagFile.exists()) {
                flagFile.remove();
            }

            if (flagFile.open(NC_FOM_WriteOnly)) {
                NCCHAR init[] = "init";
                flagFile.write(init, sizeof(init));
                flagFile.close();

                NHNetworkHandler::getNetworkHandler()->startTimer(TimerType_Dhclient);
                NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->setDhclientTimerStatus(NC_TRUE);
            }
            else {
                NCLOGD("NHNetworkDoDhcpReq::doAction, Flag file open failed");
                NCErrorLog(NI_ERROR_HAL_NETWORK_OPENFILE_FAILED, 0);
            }
        }

        NHNetworkDhcp dhcp;
        UINT32 ret = dhcp.startClient(m_device);

        DhclientInfo info;
        if (NI_ERROR_HAL_NETWORK_SUCCESS == ret) {
            NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->get(m_device, info);
        }
        else {
            if (!NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->checkDhclientRunningStatus()) {
                NCLOGD("NHNetworkDoDhcpReq::doAction, Stop timer for dhclient");
                NHNetworkHandler::getNetworkHandler()->stopTimer(TimerType_Dhclient);
                NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->setDhclientTimerStatus(NC_FALSE);
            }
        }

        NHNetworkReply reply;
        reply.notifyDoDhcpResult(m_device, info, ret);
    }

    NHNetworkStopDhcpReq::NHNetworkStopDhcpReq(const NCString& device)
    : m_device(device)
    {

    }

    NHNetworkStopDhcpReq::~NHNetworkStopDhcpReq()
    {

    }

    VOID
    NHNetworkStopDhcpReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkStopDhcpReq::doAction");
        NHNetworkDhcp dhcp;
        UINT32 ret = dhcp.stopClient(m_device);
        NHNetworkReply reply;
        reply.notifyStopDhcpResult(m_device, ret);
    }

    NHNetworkReleaseDhcpLeaseReq::NHNetworkReleaseDhcpLeaseReq(const NCString& device)
    : m_device(device)
    {

    }

    NHNetworkReleaseDhcpLeaseReq::~NHNetworkReleaseDhcpLeaseReq()
    {

    }

    VOID
    NHNetworkReleaseDhcpLeaseReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkReleaseDhcpLeaseReq::doAction");
        NHNetworkDhcp dhcp;
        UINT32 ret = dhcp.releaseIp(m_device);

        if (!NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->checkDhclientRunningStatus()) {
            NCLOGD("NHNetworkReleaseDhcpLeaseReq::doAction, Stop timer for dhclient");
            NHNetworkHandler::getNetworkHandler()->stopTimer(TimerType_Dhclient);
            NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->setDhclientTimerStatus(NC_FALSE);
        }

        NHNetworkReply reply;
        reply.notifyReleaseDhcpLeaseResult(m_device, ret);
    }

    NHNetworkSetStaticIpReq::NHNetworkSetStaticIpReq(const NCString& device, const NCString& staticIp)
    : m_device(device)
    , m_staticIp(staticIp)
    {

    }

    NHNetworkSetStaticIpReq::~NHNetworkSetStaticIpReq()
    {

    }

    VOID
    NHNetworkSetStaticIpReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkSetStaticIpReq::doAction - device = [%s], Ip = [%s]", m_device.getString(), m_staticIp.getString());
        UINT32 ret = setIpAddr();
        NHNetworkReply reply;
        reply.notifySetStaticIpResult(m_device, ret);
    }

    UINT32
    NHNetworkSetStaticIpReq::setIpAddr()
    {
        if (ifc_init()) {
            NCLOGD("NHNetworkSetStaticIpReq::setIpAddr - Ifc init failed!");
            return NI_ERROR_HAL_NETWORK_FAIL;
        }

        if (ifc_up(m_device)) {
            NCLOGD("NHNetworkSetStaticIpReq::setIpAddr - Ifc up failed!");
            ifc_close();
            return NI_ERROR_HAL_NETWORK_FAIL;
        }

        in_addr ipAddr;
        INT32 temp = inet_aton(m_staticIp.getString(), &ipAddr);
        if (!temp) {
            NCLOGD("NHNetworkSetStaticIpReq::setIpAddr - Inet aton failed!");
            ifc_close();
            return NI_ERROR_HAL_NETWORK_FAIL;
        }

        INT32 ret = ifc_set_addr(m_device.getString(), ipAddr.s_addr);
        if (ret) {
            NCLOGD("NHNetworkSetStaticIpReq::setIpAddr - Set ip failed");
            ifc_close();
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        else {
            NCLOGD("NHNetworkSetStaticIpReq::setIpAddr - Set ip success");
            ifc_close();
            return NI_ERROR_HAL_NETWORK_SUCCESS;
        }
    }

    NHNetworkSetMaskReq::NHNetworkSetMaskReq(const NCString& device, const UINT32& prefixLength)
    : m_device(device)
    , m_prefixLength(prefixLength)
    {

    }

    NHNetworkSetMaskReq::~NHNetworkSetMaskReq()
    {

    }

    VOID
    NHNetworkSetMaskReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkSetMaskReq::doAction - device = [%s], prefixLength = [%d]", m_device.getString(), m_prefixLength);
        UINT32 ret = setMask();
        NHNetworkReply reply;
        reply.notifySetMaskResult(m_device, ret);
    }

    UINT32
    NHNetworkSetMaskReq::setMask()
    {
        if (ifc_init()) {
            NCLOGD("NHNetworkSetMaskReq::setMask - Ifc init failed!");
            return NI_ERROR_HAL_NETWORK_FAIL;
        }

        INT32 ret = ifc_set_prefixLength(m_device.getString(), m_prefixLength);
        if (ret) {
            NCLOGD("NHNetworkSetMaskReq::setMask - Set mask failed!");
            ifc_close();
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        else {
            NCLOGD("NHNetworkSetMtuReq::setMask - Set mask success");
            ifc_close();
            return NI_ERROR_HAL_NETWORK_SUCCESS;
        }
    }

    NHNetworkSetMtuReq::NHNetworkSetMtuReq(const NCString& device, const UINT32 mtu)
    : m_device(device)
    , m_mtu(mtu)
    {

    }

    NHNetworkSetMtuReq::~NHNetworkSetMtuReq()
    {

    }

    VOID
    NHNetworkSetMtuReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkSetMtuReq::doAction, device = [%s], mtu = [%d]", m_device.getString(), m_mtu);
        UINT32 ret = setMtu();
        NHNetworkReply reply;
        reply.notifySetMtuResult(m_device, ret);
    }

    UINT32
    NHNetworkSetMtuReq::setMtu()
    {
        if (ifc_init()) {
            NCLOGD("NHNetworkSetMtuReq::setMtu - Ifc init failed!");
            return NI_ERROR_HAL_NETWORK_FAIL;
        }

        INT32 ret = ifc_set_mtu(m_device.getString(), m_mtu);
        if (ret) {
            NCLOGD("NHNetworkSetMtuReq::setMtu - Set mtu faild");
            ifc_close();
            return NI_ERROR_HAL_NETWORK_FAIL;
        }
        else {
            NCLOGD("NHNetworkSetMtuReq::setMtu - Set mtu success");
            ifc_close();
            return NI_ERROR_HAL_NETWORK_SUCCESS;
        }
    }

    NHNetworkSetStaticIpMaskMtuReq::NHNetworkSetStaticIpMaskMtuReq(const NCString& device, const NCString& staticIp, const UINT32& prefixLength, const UINT32& mtu)
    : m_device(device)
    , m_staticIp(staticIp)
    , m_prefixLength(prefixLength)
    , m_mtu(mtu)
    {

    }

    NHNetworkSetStaticIpMaskMtuReq::~NHNetworkSetStaticIpMaskMtuReq()
    {

    }

    VOID
    NHNetworkSetStaticIpMaskMtuReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkSetStaticIpMaskMtuReq::doAction, Start, device [%s], ip [%s], prefixLength [%d], mtu [%d]", m_device.getString(), m_staticIp.getString(), m_prefixLength, m_mtu);
        NHNetworkReply reply;

        NHNetworkSetStaticIpReq ipReq(m_device, m_staticIp);
        UINT32 ipRet = ipReq.setIpAddr();
        if (NI_ERROR_HAL_NETWORK_SUCCESS != ipRet) {
            NCLOGD("NHNetworkSetStaticIpMaskMtuReq::doAction - Set IP failed!");
            reply.notifySetStaticIpMaskMtuResult(m_device, NI_ERROR_HAL_NETWORK_FAIL);
            return;
        }

        NHNetworkSetMaskReq maskReq(m_device, m_prefixLength);
        UINT32 maskRet = maskReq.setMask();
        if (NI_ERROR_HAL_NETWORK_SUCCESS != maskRet) {
            NCLOGD("NHNetworkSetStaticIpMaskMtuReq::doAction - Set mask failed!");
            reply.notifySetStaticIpMaskMtuResult(m_device, NI_ERROR_HAL_NETWORK_FAIL);
            return;
        }

        if (0 == m_mtu) {
            reply.notifySetStaticIpMaskMtuResult(m_device, NI_ERROR_HAL_NETWORK_SUCCESS);
            return;
        }
        else {
            NHNetworkSetMtuReq mtuReq(m_device, m_mtu);
            INT32 mtuRet = mtuReq.setMtu();
            if (NI_ERROR_HAL_NETWORK_SUCCESS != mtuRet) {
                NCLOGD("NHNetworkSetStaticIpMaskMtuReq::doAction - Set mtu faild");
                reply.notifySetStaticIpMaskMtuResult(m_device, NI_ERROR_HAL_NETWORK_FAIL);
                return;
            }
            else {
                NCLOGD("NHNetworkSetStaticIpMaskMtuReq::doAction - Set ip mask mtu faild");
                reply.notifySetStaticIpMaskMtuResult(m_device, NI_ERROR_HAL_NETWORK_SUCCESS);
                return;
            }
        }
    }


    NHNetworkSetGatewayReq::NHNetworkSetGatewayReq(const NCString& device, const NCString& gateway)
    : m_device(device)
    , m_gateway(gateway)
    {

    }

    NHNetworkSetGatewayReq::~NHNetworkSetGatewayReq()
    {

    }

    VOID
    NHNetworkSetGatewayReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkSetGatewayReq::doAction");
    }

    NHNetworkAddDefauteRouteReq::NHNetworkAddDefauteRouteReq(const NCString& device, const NCString& gateway, const NC_BOOL updateFlag)
    : m_device(device)
    , m_gateway(gateway)
    , m_updateFlag(updateFlag)
    {

    }

    NHNetworkAddDefauteRouteReq::~NHNetworkAddDefauteRouteReq()
    {

    }

    VOID
    NHNetworkAddDefauteRouteReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkAddDefauteRouteReq::doAction - device = [%s], gateway = [%s]", m_device.getString(), m_gateway.getString());
        in_addr gateway;
        INT32 temp = inet_aton(m_gateway.getString(), &gateway);
        if (!temp) {
            NCLOGD("NHNetworkAddDefauteRouteReq::doAction - inet aton failed!");
            return;
        }

        INT32 ret = ifc_set_default_route(m_device.getString(), gateway.s_addr);
        if (!ret) {
            NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->updateGateway(m_device, m_gateway);
        }
        else {
            NCLOGD("NHNetworkAddDefauteRouteReq::doAction - Add default  route failed!");
        }

        if (!m_updateFlag) {
            NHNetworkReply reply;
            reply.notifyAddDefaultRouteResult(m_device, ret);
        }
        else {
            NCLOGD("NHNetworkAddDefauteRouteReq::doAction, Update flag is true");
        }

    }

    NHNetworkRemoveDefauteRouteReq::NHNetworkRemoveDefauteRouteReq(const NCString& device)
    : m_device(device)
    {

    }

    NHNetworkRemoveDefauteRouteReq::~NHNetworkRemoveDefauteRouteReq()
    {

    }

    VOID
    NHNetworkRemoveDefauteRouteReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkRemoveDefauteRouteReq::doAction - device = [%s]", m_device.getString());
        INT32 ret = ifc_remove_default_route(m_device.getString());

        NHNetworkReply reply;
        if (ret) {
            NHNetworkCheckInterface check;
            if (!check.interfaceExist(m_device)) {
                NCLOGD("NHNetworkRemoveDefauteRouteReq::doAction, Interface is not exist");
                reply.notifyRemoveDefaultRouteResult(m_device, NI_ERROR_HAL_NETWORK_SUCCESS);
            }
            else {
                in_addr ipAddr;
                if (ifc_get_addr(m_device.getString(), &ipAddr.s_addr)) {
                    NCLOGD("NHNetworkRemoveDefauteRouteReq::doAction, Get Ip failed");
                    reply.notifyRemoveDefaultRouteResult(m_device, NI_ERROR_HAL_NETWORK_SUCCESS);
                }
                else {
                    NCString ip = inet_ntoa(ipAddr);
                    if (NCString("0.0.0.0") == ip) {
                        NCLOGD("NHNetworkRemoveDefauteRouteReq::doAction, Ip is 0.0.0.0");
                        reply.notifyRemoveDefaultRouteResult(m_device, NI_ERROR_HAL_NETWORK_SUCCESS);
                    }
                    else {
                        NCLOGD("NHNetworkRemoveDefauteRouteReq::doAction - Remove default route failed");
                        reply.notifyRemoveDefaultRouteResult(m_device, NI_ERROR_HAL_NETWORK_FAIL);
                    }
                }
            }
        }
        else {
            NCLOGD("NHNetworkRemoveDefauteRouteReq::doAction - Remove default route success");
            reply.notifyRemoveDefaultRouteResult(m_device, NI_ERROR_HAL_NETWORK_SUCCESS);
        }

    }

    NHNetworkAddRouteReq::NHNetworkAddRouteReq(const NCString& device, const NHNetworkRouteInfo& route)
    : m_device(device)
    , m_route(route)
    {

    }

    NHNetworkAddRouteReq::~NHNetworkAddRouteReq()
    {

    }

    VOID
    NHNetworkAddRouteReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkAddRouteReq::doAction - device = [%s], destination = [%s], prefixLength = [%d], gateway = [%s]",
               m_device.getString(), m_route.destination.getString(), m_route.prefixLength, m_route.gateway.getString());
        INT32 ret = ifc_add_route(m_device.getString(), m_route.destination.getString(), m_route.prefixLength, m_route.gateway.getString());
        NHNetworkReply reply;
        if (!ret) {
            NCLOGD("NHNetworkAddRouteReq::doAction - Add route success");
            reply.notifyAddRouteResult(m_device, NI_ERROR_HAL_NETWORK_SUCCESS);
        }
        else {
            NCLOGD("NHNetworkAddRouteReq::doAction - Add route failed");
            reply.notifyAddRouteResult(m_device, NI_ERROR_HAL_NETWORK_FAIL);
        }
    }

    NHNetworkRemoveRouteReq::NHNetworkRemoveRouteReq(const NCString& device, const NHNetworkRouteInfo& route)
    : m_device(device)
    , m_route(route)
    {

    }

    NHNetworkRemoveRouteReq::~NHNetworkRemoveRouteReq()
    {

    }

    VOID
    NHNetworkRemoveRouteReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkRemoveRouteReq::doAction - device = [%s], destination = [%s], prefix length = [%d], gateway = [%s]",
               m_device.getString(), m_route.destination.getString(), m_route.prefixLength, m_route.gateway.getString());
        INT32 ret = ifc_remove_route(m_device.getString(), m_route.destination.getString(), m_route.prefixLength, m_route.gateway.getString());
        if (ret) {
            NCLOGD("NHNetworkRemoveRouteReq::doAction - Remove route failed!");
        }
    }

    NHNetworkSetFirewallDefaultRuleReq::NHNetworkSetFirewallDefaultRuleReq()
    {

    }

    NHNetworkSetFirewallDefaultRuleReq::~NHNetworkSetFirewallDefaultRuleReq()
    {

    }

    VOID
    NHNetworkSetFirewallDefaultRuleReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkSetFirewallDefaultRuleReq::doAction - Start");
    }

    NHNetworkOpenFirewallPortReq::NHNetworkOpenFirewallPortReq(const NCString& protocol, UINT32 port)
    : m_protocol(protocol)
    , m_port(port)
    {

    }

    NHNetworkOpenFirewallPortReq::~NHNetworkOpenFirewallPortReq()
    {

    }

    VOID
    NHNetworkOpenFirewallPortReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkOpenFirewallPortReq::doAction - Start");
#if 0
        NCCHAR cmd[128] = { 0 };
        snprintf(cmd, sizeof(cmd)-1, "iptables -t filter -A OUTPUT -p %s --sport %d -j ACCEPT", m_protocol.getString(), m_port);
        system(cmd);
        snprintf(cmd, sizeof(cmd)-1, "iptables -t filter -A INPUT -p %s --dport %d -j ACCEPT", m_protocol.getString(), m_port);
        system(cmd);
#endif
    }


    NHNetworkCloseFirewallPortReq::NHNetworkCloseFirewallPortReq(const NCString& protocol, UINT32 port)
    : m_protocol(protocol)
    , m_port(port)
    {

    }

    NHNetworkCloseFirewallPortReq::~NHNetworkCloseFirewallPortReq()
    {

    }

    VOID
    NHNetworkCloseFirewallPortReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkCloseFirewallPortReq::doAction - Start");
#if 0
        NCCHAR cmd[128] = { 0 };
        snprintf(cmd, sizeof(cmd)-1, "iptables -t filter -D OUTPUT -p %s --sport %d -j ACCEPT", m_protocol.getString(), m_port);
        system(cmd);
        snprintf(cmd, sizeof(cmd)-1, "iptables -t filter -D INPUT -p %s --dport %d -j ACCEPT", m_protocol.getString(), m_port);
        system(cmd);
#endif
    }


    NHNetworkSetNatTableReq::NHNetworkSetNatTableReq(UINT32 setMode, const NCString& deviceName)
    : m_setMode(setMode)
    , m_deviceName(deviceName)
    {

    }

    NHNetworkSetNatTableReq::~NHNetworkSetNatTableReq()
    {

    }

    VOID
    NHNetworkSetNatTableReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkSetNatTableReq::doAction - Start, setMode = [%d], deviceName = [%s]", m_setMode, m_deviceName.getString());
        NCCHAR cmd[128] = { 0 };
        if (NINetworkHandler::NatSetMode_DcuWifi == m_setMode) {
            NCLOGD("NHNetworkSetNatTableReq::doAction, NatSetMode_DcuWifi");
            if (ifc_init()) {
                NCLOGD("NHNetworkSetNatTableReq::doAction, ifc init failed");
                return;
            }

            in_addr ipAddr;
            if (ifc_get_addr(m_deviceName.getString(), &ipAddr.s_addr)) {
                NCLOGD("NHNetworkSetNatTableReq::doAction, ifc get addr failed");
                return;
            }

            NCString ip = inet_ntoa(ipAddr);
            NCLOGD("NHNetworkSetNatTableReq::doAction, ip = [%s]", ip.getString());
            snprintf(cmd, sizeof(cmd)-1, ". %s %d %s %s", FIREWALL_SET_SCRIPT, m_setMode, m_deviceName.getString(), ip.getString());
            system(cmd);
        }
        else if (NINetworkHandler::NatSetMode_DcuAp == m_setMode) {
            NCLOGD("NHNetworkSetNatTableReq::doAction, NatSetMode_DcuAp");
            snprintf(cmd, sizeof(cmd)-1, ". %s %d %s", FIREWALL_SET_SCRIPT, m_setMode, m_deviceName.getString());
            system(cmd);
        }

    }

    NHNetworkDeviceInfoReq::NHNetworkDeviceInfoReq(const NCString& device)
    : m_device(device)
    {

    }

    NHNetworkDeviceInfoReq::~NHNetworkDeviceInfoReq()
    {

    }

    VOID
    NHNetworkDeviceInfoReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkDeviceInfoReq::doAction - Start");
        NCLOGD("NHNetworkDeviceInfoReq::doAction - Start test");

        NHNetworkDeviceInfo info;
        info.device = m_device;
        NHNetworkReply reply;

        if (ifc_init()) {
            NCLOGD("NHNetworkDeviceInfoReq::doAction - Ifc init failed!");
            reply.notifyNetworkDeviceInfoResult(info, NI_ERROR_HAL_NETWORK_FAIL);
            return;
        }

        in_addr ipAddr;
        int prefixLength;
        unsigned flags;
        if (ifc_get_info(m_device.getString(), &ipAddr.s_addr, &prefixLength, &flags)) {
            NCLOGD("NHNetworkDeviceInfoReq::doAction - Get IP and prefixLength failed");
            reply.notifyNetworkDeviceInfoResult(info, NI_ERROR_HAL_NETWORK_FAIL);
            return;
        }

        info.ip = inet_ntoa(ipAddr);
        info.prefixLength = prefixLength;

        unsigned char hwaddr[6] = { 0 };
        if (ifc_get_hwaddr(m_device.getString(), (void*)hwaddr)) {
            NCLOGD("NHNetworkDeviceInfoReq::doAction - Get hardware address failed");
            reply.notifyNetworkDeviceInfoResult(info, NI_ERROR_HAL_NETWORK_FAIL);
            return;
        }

        char mac[18] = { 0 };
        snprintf(mac, sizeof(mac)-1, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
        info.mac = mac;

        DhclientInfo dhcpInfo;
        NC_BOOL found = NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->get(m_device, dhcpInfo);
        if (found) {
            info.gateway = dhcpInfo.gateway;
            info.dns1 = dhcpInfo.dns1;
            info.dns2 = dhcpInfo.dns2;
            info.serverId = dhcpInfo.serverId;
        }

        ifc_close();
        NCLOGD("NHNetworkDeviceInfoReq::doAction - Start notify result");
        reply.notifyNetworkDeviceInfoResult(info, NI_ERROR_HAL_NETWORK_SUCCESS);
    }


    NHNetworkConnectedClientInfoReq::NHNetworkConnectedClientInfoReq(const NCString& mac)
    : m_mac(mac)
    {

    }

    NHNetworkConnectedClientInfoReq::~NHNetworkConnectedClientInfoReq()
    {

    }

    VOID
    NHNetworkConnectedClientInfoReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkConnectedClientInfoReq::doAction - Start, MAC = [%s]", m_mac.getString());
        char lowercaseMac[64] = { 0 };
        NC_BOOL converResult = NCConverter::StrToLower(m_mac.getString(), m_mac.getLength(), lowercaseMac, sizeof(lowercaseMac));
        if (!converResult) {
            NCLOGD("NHNetworkConnectedClientInfoReq::doAction - Convert mac letters from uppercase to lowercase failed!!!");
            return;
        }
        NCLOGD("NHNetworkConnectedClientInfoReq::doAction - Convert mac letters success, MAC = [%s]", lowercaseMac);

        char leaseFileName[] = DHCPD_SERVER_LEASE;
        NHNetworkConnectedClientInfo info;
        UINT32 result = NI_ERROR_HAL_NETWORK_FAIL;

        char lineBuf[512] = { 0 };
        UINT32 attempts = 0;
        NC_BOOL inOneNode = NC_FALSE;
        char *tmp = NULL;

        char bindingTag[] = "binding state";
        char rightBinding[] = "active";
        char bindingString[20] = { 0 };
        NCString bindingStatus;

        char macTag[] = "hardware ethernet";
        char macString[20] = { 0 };

        NC_BOOL hostNameExist = NC_FALSE;
        char hostNameTag[] = "client-hostname";
        char hostNameString[255] = { 0 };
        char hostNameUnknown[] = "Empty Host Name";
        char ipString[20] = { 0 };

        while (attempts < 30) {
            FILE *fp = fopen(leaseFileName, "r");
            if (fp == NULL) {
                NCLOGD("NHNetworkConnectedClientInfoReq::doAction - Failed to open [%s]\n", leaseFileName);
               usleep(500000);
               attempts++;
               continue;
            }

            while (fgets(lineBuf, sizeof(lineBuf), fp) != NULL) {
                if (inOneNode) {
                    tmp = lineBuf;
                    while (*tmp && isspace(*tmp)) {
                        ++tmp;
                    }

                    if (strncmp(tmp, bindingTag, strlen(bindingTag)) == 0) {
                        sscanf(tmp, "%*s%*s%19s", bindingString);
                        bindingString[strlen(bindingString) - 1] = 0;
                    }

                    if (strncmp(tmp, macTag, strlen(macTag)) == 0) {
                        sscanf(tmp, "%*s%*s%19s", macString);
                        macString[strlen(macString) - 1] = 0;
                    }

                    if (strncmp(tmp, hostNameTag, strlen(hostNameTag)) == 0) {
                        hostNameExist = NC_TRUE;
                        sscanf(tmp, "%*s%254s", hostNameString);
                        hostNameString[strlen(hostNameString) - 2] = 0;
                    }
                }

                if (strncmp(lineBuf, "lease ", 6) == 0) {
                    inOneNode = NC_TRUE;
                    tmp = lineBuf;
                    while (*tmp && isspace(*tmp)) {
                        ++tmp;
                    }
                    sscanf(tmp, "%*s%19s", ipString);
                    continue;
                }

                if (strncmp(lineBuf, "}", 1) == 0) {
                    if (strncmp(lowercaseMac, macString, strlen(macString)) == 0) {
                        info.mac = macString;
                        info.ip = ipString;
                        if (hostNameExist) {
                            char* hostTmep = hostNameString + 1;
                            info.hostName = hostTmep;
                        }
                        else {
                            info.hostName = hostNameUnknown;
                        }

                        bindingStatus = bindingString;
                    }
                    inOneNode = NC_FALSE;
                    hostNameExist = NC_FALSE;
                }
            }

            if (strncmp(lowercaseMac, info.mac.getString(), strlen(macString)) == 0
                && strncmp(bindingStatus.getString(), rightBinding, strlen(rightBinding)) == 0) {
                NCLOGD("NHNetworkConnectedClientInfoReq::doAction, Get connected info success, attempts [%d]", attempts);
                fclose(fp);
                break;
            }
            else {
                attempts++;
            }

            fclose(fp);
        }

        if (attempts < 20) {
            result = NI_ERROR_HAL_NETWORK_SUCCESS;
        }

        NHNetworkReply reply;
        reply.notifyConnectedClientInfoResult(info, result);
    }


    NHNetworkStartDhcpServerReq::NHNetworkStartDhcpServerReq(const NCString& device, const NCString& ip, const UINT32& prefixLength)
    : m_device(device)
    , m_ip(ip)
    , m_prefixLength(prefixLength)
    {

    }

    NHNetworkStartDhcpServerReq::~NHNetworkStartDhcpServerReq()
    {

    }

    VOID
    NHNetworkStartDhcpServerReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkStartDhcpServerReq::doAction - Start, device [%s], ip [%s], prefixLength [%d]", m_device.getString(), m_ip.getString(), m_prefixLength);
        NHNetworkReply reply;
        NHNetworkSetStaticIpReq ipReq(m_device, m_ip);
        UINT32 ipRet = ipReq.setIpAddr();
        if (NI_ERROR_HAL_NETWORK_SUCCESS != ipRet) {
            NCLOGD("NHNetworkStartDhcpServerReq::doAction - Set IP failed!");
            reply.notifyStartDhcpServerResult(m_device, NI_ERROR_HAL_NETWORK_FAIL);
            return;
        }

        NHNetworkSetMaskReq maskReq(m_device, m_prefixLength);
        UINT32 maskRet = maskReq.setMask();
        if (NI_ERROR_HAL_NETWORK_SUCCESS != maskRet) {
            NCLOGD("NHNetworkStartDhcpServerReq::doAction - Set mask failed!");
            reply.notifyStartDhcpServerResult(m_device, NI_ERROR_HAL_NETWORK_FAIL);
            return;
        }

        // deal with lease file
        if (!NHNetworkHandler::getNetworkHandler()->isDhcpdFirstInit()) {
            NCFile dhcpdFile(DHCPD_SERVER_LEASE);
            if (dhcpdFile.exists()) {
                NCLOGD("NHNetworkStartDhcpServerReq::doAction, dhcpd.leases is exist, remove it");
                dhcpdFile.remove();
            }

            if (dhcpdFile.open(NC_FOM_Append)) {
                NCLOGD("NHNetworkStartDhcpServerReq::doAction, creat dhcpd.leases success");
                dhcpdFile.setPermissions(NHNETWORK_FILE_PERMISSIONS);
                dhcpdFile.close();
            }

            NHNetworkHandler::getNetworkHandler()->setDhcpdFirstInit(NC_TRUE);
        }

        NHNetworkDhcp dhcp;
        UINT32 result = dhcp.startServer(m_device);
        reply.notifyStartDhcpServerResult(m_device, result);
    }

    NHNetworkStopDhcpServerReq::NHNetworkStopDhcpServerReq(const NCString& device)
    : m_device(device)
    {

    }

    NHNetworkStopDhcpServerReq::~NHNetworkStopDhcpServerReq()
    {

    }

    VOID
    NHNetworkStopDhcpServerReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkStopDhcpServerReq::doAction - Stop DHCP server");
        NHNetworkReply reply;
        NHNetworkDhcp dhcp;
        UINT32 ret = dhcp.stopServer(m_device);
        if (NI_ERROR_HAL_NETWORK_SUCCESS != ret) {
            reply.notifyStopDhcpServerResult(m_device, ret);
            return;
        }

        NHNetworkCheckInterface check;
        if (!check.interfaceExist(m_device)) {
            NCLOGD("NHNetworkStopDhcpServerReq::doAction, Interface is not exist");
            reply.notifyStopDhcpServerResult(m_device, NI_ERROR_HAL_NETWORK_SUCCESS);
        }
        else {
            NHNetworkSetStaticIpReq ipReq(m_device, "0.0.0.0");
            ipReq.setIpAddr();
            reply.notifyStopDhcpServerResult(m_device, NI_ERROR_HAL_NETWORK_SUCCESS);
        }
    }


    NHNetworkRestartDhcpServerReq::NHNetworkRestartDhcpServerReq(const NCString& device)
    : m_device(device)
    {

    }

    NHNetworkRestartDhcpServerReq::~NHNetworkRestartDhcpServerReq()
    {

    }

    VOID
    NHNetworkRestartDhcpServerReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkRestartDhcpServerReq::doAction, device [%s]", m_device.getString());
        NHNetworkDhcp dhcp;
        NHNetworkReply reply;
        UINT32 stopRet = dhcp.stopServer(m_device);
        if (NI_ERROR_HAL_NETWORK_SUCCESS != stopRet) {
            NCLOGD("NHNetworkRestartDhcpServerReq::doAction, Stop dhcp server failed");
            reply.notifyRestartDhcpServerResult(m_device, NI_ERROR_HAL_NETWORK_FAIL);
            return;
        }

        UINT32 startRet = dhcp.startServer(m_device);
        reply.notifyRestartDhcpServerResult(m_device, startRet);
    }

    NHNetworkStartDnsCacheServerReq::NHNetworkStartDnsCacheServerReq()
    {

    }

    NHNetworkStartDnsCacheServerReq::~NHNetworkStartDnsCacheServerReq()
    {

    }

    VOID
    NHNetworkStartDnsCacheServerReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkStartDnsCacheServerReq::doAction - Start");
#if defined NCNETWORK_TARGET_PRODUCT_EMULATOR
        NCLOGD("NHNetworkStartDnsCacheServerReq::doAction, Emulator");
        return;
#endif

        NCFile tmpFileObj(NHNETWORK_TMP_NAMED_CONFIG_FILE);
        if (tmpFileObj.exists()) {
            NCLOGD("NHNetworkStartDnsCacheServerReq::doAction, /tmp/named.conf exist, remove it");
            tmpFileObj.remove();
        }

        NCFile::copy(NHNETWORK_ETC_NAMED_CONFIG_FILE, NHNETWORK_TMP_NAMED_CONFIG_FILE);
        property_set("ctl.start", "named");
    }

    NHNetworkStopDnsCacheServerReq::NHNetworkStopDnsCacheServerReq()
    {

    }

    NHNetworkStopDnsCacheServerReq::~NHNetworkStopDnsCacheServerReq()
    {

    }

    VOID
    NHNetworkStopDnsCacheServerReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkStopDnsCacheServerReq::doAction - Start");
#if defined NCNETWORK_TARGET_PRODUCT_EMULATOR
        NCLOGD("NHNetworkStopDnsCacheServerReq::doAction, Emulator");
        return;
#endif
        property_set("ctl.stop", "named");
    }

    NHNetworkUpdateRemoteDnsReq::NHNetworkUpdateRemoteDnsReq(const NCString& dns1, const NCString& dns2)
    : m_dns1(dns1)
    , m_dns2(dns2)
    {

    }

    NHNetworkUpdateRemoteDnsReq::~NHNetworkUpdateRemoteDnsReq()
    {

    }

    VOID
    NHNetworkUpdateRemoteDnsReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkUpdateRemoteDnsReq::doAction - Start, dns1 [%s], dns2 [%s]", m_dns1.getString(), m_dns2.getString());
        NCString namedFile = NHNETWORK_TMP_NAMED_CONFIG_FILE;
        NCCHAR tempFile[64] = { 0 };
        snprintf(tempFile, sizeof(tempFile)-1, "%s~", namedFile.getString());
        if (NCFile::exists(tempFile)) {
            NCLOGD("NHNetworkUpdateRemoteDnsReq::doAction - [%s] is exist", tempFile);
            NCFile::remove(tempFile);
        }

        FILE* oldPid = fopen(namedFile.getString(), "r");
        if (NULL == oldPid) {
            NCLOGD("NHNetworkUpdateRemoteDnsReq::doAction - Open %s failed", namedFile.getString());
            return;
        }

        FILE* newPid = fopen(tempFile, "at");
        if (NULL == newPid) {
            NCLOGD("NHNetworkUpdateRemoteDnsReq::doAction - Open %s failed", tempFile);
            fclose(oldPid);
            return;
        }

        NCCHAR options[] = "options";
        NCCHAR find[] = "forwarders";
        NC_BOOL optionsFlag = NC_FALSE;
        NCCHAR buf[128] = { 0 };

        while (NULL != fgets(buf, sizeof(buf)-1, oldPid)) {
            // remove space at the begging of a line
            NCCHAR* tempBuf = buf;
            while (*tempBuf && isspace(*tempBuf)) {
                ++tempBuf;
            }

            if (!strncmp(options, tempBuf, strlen(options))) {
                NCLOGD("NHNetworkUpdateRemoteDnsReq::doAction, Find options success");
                optionsFlag = NC_TRUE;
                fprintf(newPid, "%s", buf);
                continue;
            }

            if (optionsFlag) {
                if (!strncmp(find, tempBuf, strlen(find))) {
                    NCLOGD("NHNetworkUpdateRemoteDnsReq::doAction, Find forwarders in options success");
                    NCCHAR replace[128] = { 0 };
                    if (NCString("") != m_dns1 && NCString("") != m_dns2) {
                        NCLOGD("NHNetworkUpdateRemoteDnsReq::doAction, update, dns1 and dns2 is not NULL");
                        snprintf(replace, sizeof(replace)-1, "%s { %s; %s; };", find, m_dns1.getString(), m_dns2.getString());
                    }
                    else if (NCString("") != m_dns1 && NCString("") == m_dns2) {
                        NCLOGD("NHNetworkUpdateRemoteDnsReq::doAction, update, dns2 is NULL");
                        snprintf(replace, sizeof(replace)-1, "%s { %s; };", find, m_dns1.getString());
                    }
                    else if (NCString("") == m_dns1 && NCString("") != m_dns2) {
                        NCLOGD("NHNetworkUpdateRemoteDnsReq::doAction, update, dns1 is NULL");
                        snprintf(replace, sizeof(replace)-1, "%s { %s; };", find, m_dns2.getString());
                    }
                    else {
                        NCLOGD("NHNetworkUpdateRemoteDnsReq::doAction, update, dns1 and dns2 is NULL");
                        snprintf(replace, sizeof(replace)-1, "%s { };", find);
                    }

                    fprintf(newPid, "    %s\n", replace);
                }
                else {
                    if (!strncmp(tempBuf, "}", 1)) {
                        optionsFlag = NC_FALSE;
                    }
                    fprintf(newPid, "%s", buf);
                }

                continue;
            }

            fprintf(newPid, "%s", buf);
        }

        fclose(newPid);
        fclose(oldPid);

        NCFile::remove(namedFile);
        NCFile::move(tempFile, namedFile);

        NCCHAR cmd[128] = { 0 };
        snprintf(cmd, sizeof(cmd)-1, "rndc reconfig");
        system(cmd);
    }


    NHNetworkUpdateGbookDnsReq::NHNetworkUpdateGbookDnsReq(const NCString& dns1, const NCString& dns2)
    : m_dns1(dns1)
    , m_dns2(dns2)
    {

    }

    NHNetworkUpdateGbookDnsReq::~NHNetworkUpdateGbookDnsReq()
    {

    }

    VOID
    NHNetworkUpdateGbookDnsReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkUpdateGbookDnsReq::doAction, Start, m_dns1 [%s], m_dns2 [%s]", m_dns1.getString(), m_dns2.getString());
        NCString namedFile = NHNETWORK_TMP_NAMED_CONFIG_FILE;
        NCCHAR tempFile[64] = { 0 };
        snprintf(tempFile, sizeof(tempFile)-1, "%s~", namedFile.getString());
        if (NCFile::exists(tempFile)) {
            NCLOGD("NHNetworkUpdateGbookDnsReq::doAction - [%s] is exist", tempFile);
            NCFile::remove(tempFile);
        }

        FILE* oldPid = fopen(namedFile.getString(), "r");
        if (NULL == oldPid) {
            NCLOGD("NHNetworkUpdateGbookDnsReq::doAction - Open file [%s] failed", namedFile.getString());
            return;
        }

        FILE* newPid = fopen(tempFile, "at");
        if (NULL == newPid) {
            NCLOGD("NHNetworkUpdateGbookDnsReq::doAction - Open file [%s] failed", tempFile);
            fclose(oldPid);
            return;
        }

        NCCHAR zone[] = "zone";
        NCCHAR gbookZone[] = "g-book.com.cn";
        NCCHAR find[] = "forwarders";
        NC_BOOL gbookFlag = NC_FALSE;
        NCCHAR buf[128] = { 0 };

        while (NULL != fgets(buf, sizeof(buf)-1, oldPid)) {
            NCCHAR* tempBuf = buf;
            while (*tempBuf && isspace(*tempBuf)) {
                ++tempBuf;
            }

            if (!strncmp(zone, tempBuf, strlen(zone))) {
                tempBuf = tempBuf + 6;
                if (!strncmp(gbookZone, tempBuf, strlen(gbookZone))) {
                    NCLOGD("NHNetworkUpdateGbookDnsReq::doAction, Find gbook zone success");
                    gbookFlag = NC_TRUE;
                }

                fprintf(newPid, "%s", buf);
                continue;
            }

            if (gbookFlag) {
                if (!strncmp(find, tempBuf, strlen(find))) {
                    NCLOGD("NHNetworkUpdateGbookDnsReq::doAction, Find gbook forwards success");
                    NCCHAR replace[128] = { 0 };
                    if (NCString("") != m_dns1 && NCString("") != m_dns2) {
                        NCLOGD("NHNetworkUpdateGbookDnsReq::doAction, update, dns1 and dns2 is not NULL");
                        snprintf(replace, sizeof(replace)-1, "%s { %s; %s; };", find, m_dns1.getString(), m_dns2.getString());
                    }
                    else if (NCString("") != m_dns1 && NCString("") == m_dns2) {
                        NCLOGD("NHNetworkUpdateGbookDnsReq::doAction, update, dns2 is NULL");
                        snprintf(replace, sizeof(replace)-1, "%s { %s; };", find, m_dns1.getString());
                    }
                    else if (NCString("") == m_dns1 && NCString("") != m_dns2) {
                        NCLOGD("NHNetworkUpdateGbookDnsReq::doAction, update, dns1 is NULL");
                        snprintf(replace, sizeof(replace)-1, "%s { %s; };", find, m_dns2.getString());
                    }
                    else {
                        NCLOGD("NHNetworkUpdateGbookDnsReq::doAction, update, dns1 and dns2 is NULL");
                        snprintf(replace, sizeof(replace)-1, "%s { };", find);
                    }
                    
                    fprintf(newPid, "    %s\n", replace);
                }
                else {
                    if (!strncmp(tempBuf, "}", 1)) {
                        gbookFlag = NC_FALSE;
                    }
                    fprintf(newPid, "%s", buf);
                }
                continue;
            }

            fprintf(newPid, "%s", buf);
        }

        fclose(newPid);
        fclose(oldPid);

        NCFile::remove(namedFile);
        NCFile::move(tempFile, namedFile);

        property_set("ctl.stop", "named");
        property_set("ctl.start", "named");
    }

    NHNetworkNetdCrashReq::NHNetworkNetdCrashReq()
    {

    }

    NHNetworkNetdCrashReq::~NHNetworkNetdCrashReq()
    {

    }

    VOID
    NHNetworkNetdCrashReq::doAction(NCQueuingThread<NHNetworkReqBase>* pThread)
    {
        NCLOGD("NHNetworkNetdCrashReq::::doAction, Start");
        // Stop dhclient process
        NCCHAR pidFile[] = VAR_DHCLIENT_PID;
        FILE* fp = fopen(pidFile, "r");
        if (NULL == fp) {
            NCLOGD("NHNetworkNetdCrashReq::::doAction, Open pid file failed");
        }
        else {
            NCCHAR buf[128] = { 0 };
            while (NULL != fgets(buf, sizeof(buf) -1, fp)) {
                NCCHAR device[32] = { 0 };
                sscanf(buf, "%31s ", device);
                NCLOGD("NHNetworkNetdCrashReq::::doAction, Read device name [%s]", device);
                NHNetworkDhcp dhcp;
                UINT32 ret = dhcp.releaseIp(device);
                NHNetworkReply reply;
                reply.notifyReleaseDhcpLeaseResult(device, ret);
            }
            fclose(fp);
        }
    }

} /* namespace nutshell */
/* EOF */
