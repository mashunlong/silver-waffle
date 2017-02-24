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

#include <ncore/NCLog.h>
#include <ncore/NCFile.h>
#include <ncore/NCError.h>
#include <cutils/properties.h>
#include <sys/types.h>
#include <signal.h>
#include <nhinterface/NINetworkDefine.h>
#include "nhnetwork/NHNetworkDefine.h"
#include "nhnetwork/NHNetworkHandler.h"
#include "NHNetworkDhcpInfo.h"
#include "NHNetworkCheckInterface.h"
#include "NHNetworkDhcp.h"

#define NC_LOG_TAG "NHNetworkHandler"
#define PROPERTY_DHCLIENT_CANCEL_FLAG "dhclient.cancel.flag"

/// Wait 45 seconds to request ip address
const int START_DHCLIENT_TIMEOUT = 45;

namespace nutshell
{

    NHNetworkDhcp::NHNetworkDhcp()
    {

    }

    NHNetworkDhcp::~NHNetworkDhcp()
    {

    }

    UINT32
    NHNetworkDhcp::startClient(const NCString& deviceName)
    {
        NCLOGD("NHNetworkDhcp::startClient, Start, Interface name [%s]", deviceName.getString());
        // Check interrupt flag
        if (checkCancelFlag()) {
            NCLOGD("NHNetworkDhcp::startClient, Interrupt success before start dhclient");
            return NI_ERROR_HAL_NETWORK_FAIL;
        }

        // Check interface is exist or not
        NHNetworkCheckInterface check;
        if (!check.interfaceExist(deviceName)) {
            NCLOGD("NHNetworkDhcp::startClient, Interface is not exist");
            return NI_ERROR_HAL_NETWORK_FAIL;
        }

        // Check dhclient for interface is running or not
        DhclientInfo info;
        NC_BOOL find = NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->get(deviceName, info);
        if (find) {
            if (NCString("running") == info.status) {
                releaseIp(deviceName);
            }

            NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->remove(deviceName);
        }

        NCCHAR cmd[128] = { 0 };
        snprintf(cmd, sizeof(cmd)-1, "dhclient -d %s &", deviceName.getString());
        system(cmd);

        UINT32 count = 0;
        while (count < START_DHCLIENT_TIMEOUT) {
            NCLOGD("NHNetworkDhcp::startClient, count [%d]", count);
            if (checkCancelFlag()) {
                NCLOGD("NHNetworkDhcp::startClient, Interrupt success in while loop");
                return NI_ERROR_HAL_NETWORK_FAIL;
            }

            DhclientInfo tempInfo;
            if (NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->get(deviceName, tempInfo)) {
                NCLOGD("NHNetworkDhcp::startClient, status [%s]", tempInfo.status.getString());
                if (NCString("cancel") == tempInfo.status) {
                    NCLOGD("NHNetworkDhcp::startClient, Interrupt success in dhclient process");
                    rmDhclientPid(deviceName);
                    return NI_ERROR_HAL_NETWORK_FAIL;
                }
                else if (NCString("running") == tempInfo.status) {
                    NCLOGD("NHNetworkDhcp::startClient, Request IP success");
                    return NI_ERROR_HAL_NETWORK_SUCCESS;
                }
            }

            sleep(1);
            ++count;
        }

        NCLOGD("NHNetworkDhcp::startClient, Start dhclient timeout");
        NCErrorLog(NI_ERROR_HAL_NETWORK_FAIL, 0);
        rmDhclientPid(deviceName);
        return NI_ERROR_HAL_NETWORK_FAIL;

    }

    UINT32
    NHNetworkDhcp::stopClient(const NCString& deviceName)
    {
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    UINT32
    NHNetworkDhcp::releaseIp(const NCString& deviceName)
    {
        NCLOGD("NHNetworkDhcp::releaseIp - Start, deviceName [%s]", deviceName.getString());
        // Check dhclient pid is exist or not
        pid_t pid;
        LONG temp;
        NC_BOOL find = readDhclientPid(deviceName, temp);
        if (find) {
            pid = (pid_t)temp;
        }
        else {
            NCLOGD("NHNetworkDhcp::releaseIp, Release success, Dhclient pid is not exist");
            NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->remove(deviceName);
            return NI_ERROR_HAL_NETWORK_SUCCESS;
        }

        // Check interface is exist or not
        NHNetworkCheckInterface check;
        if (!check.interfaceExist(deviceName)) {
            NCLOGD("NHNetworkDhcp::releaseIp, Release success, interface is not exist");
            rmDhclientPid(deviceName);
            NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->remove(deviceName);
            return NI_ERROR_HAL_NETWORK_SUCCESS;
        }

        // Check dhclient is running or not
        DhclientInfo info;
        NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->get(deviceName, info);
        if (strcmp(info.status.getString(), "running") != 0) {
            NCLOGD("NHNetworkDhcp::releaseIp - Release success, dhclient is not running");
            rmDhclientPid(deviceName);
            NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->remove(deviceName);
            return NI_ERROR_HAL_NETWORK_SUCCESS;
        }

        // Dhclient is running, release it 
        NCCHAR cmd[128] = { 0 };
        snprintf(cmd, sizeof(cmd)-1, "dhclient -x %s", deviceName.getString());
        system(cmd);

        rmDhclientPid(deviceName);
        NHNetworkHandler::getNetworkHandler()->getDhcpInfoPointer()->remove(deviceName);
        NCLOGD("NHNetworkDhcp::releaseIp - Release success");
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    UINT32
    NHNetworkDhcp::startServer(const NCString& deviceName)
    {
        NCLOGD("NHNetworkDhcp::startServer - Start");
        NCString pidFile = VAR_DHCPD_PID;
        if (NCFile::exists(pidFile)) {
            NCLOGD("NHNetworkDhcp::startServer - There is a dhcpd process running");
            stopServer(deviceName);
        }

        NCCHAR cmd[128] = { 0 };
        snprintf(cmd, sizeof(cmd)-1, "dhcpd -cf %s %s", VAR_DHCPD_CONFIG_FILE, deviceName.getString());
        INT ret = system(cmd);
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    UINT32
    NHNetworkDhcp::stopServer(const NCString& deviceName)
    {
        NCLOGD("NHNetworkDhcp::stopServer - Start");
        NCString pidFile = VAR_DHCPD_PID;
        if (!NCFile::exists(pidFile)) {
            NCLOGD("NHNetworkStopDhcpServerReq::doAction - File [%s] is not existed", pidFile.getString());
            return NI_ERROR_HAL_NETWORK_SUCCESS;
        }

        FILE* fp = fopen(pidFile, "r");
        if (fp == NULL) {
            NCLOGD("NHNetworkStopDhcpServerReq::doAction - Open file [%s] failed", pidFile.getString());
            NCErrorLog(NI_ERROR_HAL_NETWORK_OPENFILE_FAILED, 0);
            NCFile::remove(pidFile);
            return NI_ERROR_HAL_NETWORK_SUCCESS;
        }

        INT dhcpdPid;
        INT e = fscanf(fp, "%5d", &dhcpdPid);
        if (e != 1) {
            NCLOGD("NHNetworkStopDhcpServerReq::doAction - Read file [%s] failed", pidFile.getString());
            NCErrorLog(NI_ERROR_HAL_NETWORK_READFILE_FAILED, 0);
            fclose(fp);
            NCFile::remove(pidFile);
            return NI_ERROR_HAL_NETWORK_SUCCESS;
        }

        pid_t tempPid = (pid_t)dhcpdPid;
        if (tempPid) {
            kill(tempPid, SIGKILL);
        }

        fclose(fp);
        NCFile::remove(pidFile);
        return NI_ERROR_HAL_NETWORK_SUCCESS;
    }

    NC_BOOL
    NHNetworkDhcp::checkCancelFlag()
    {
        NCLOGD("NHNetworkDhcp::checkCancelFlag, Start");
        NCCHAR flag[PROPERTY_VALUE_MAX] = { 0 };
        property_get(PROPERTY_DHCLIENT_CANCEL_FLAG, flag, NULL);
        if (strcmp(flag, "on") == 0) {
            property_set(PROPERTY_DHCLIENT_CANCEL_FLAG, "");
            return NC_TRUE;
        }
        else {
            return NC_FALSE;
        }
    }

    NC_BOOL
    NHNetworkDhcp::readDhclientPid(const NCString& deviceName, LONG& pid)
    {
        NCLOGD("NHNetworkDhcp::readDhclientPid - Start");
        NCCHAR pidFile[] = VAR_DHCLIENT_PID;
        FILE* fp = fopen(pidFile, "r");
        if (NULL == fp) {
            NCLOGD("NHNetworkDhcp::readDhclientPid, open pid file failed");
            return NC_FALSE;
        }

        NCCHAR buf[128] = { 0 };
        while (NULL != fgets(buf, sizeof(buf) -1, fp)) {
            if (!strncmp(deviceName.getString(), buf, strlen(deviceName.getString()))) {
                sscanf(buf, "%*s%10ld", &pid);
                fclose(fp);
                return NC_TRUE;
            }
        }

        fclose(fp);
        return NC_FALSE;
    }

    VOID
    NHNetworkDhcp::rmDhclientPid(const NCString& deviceName)
    {
        NCLOGD("NHNetworkDhcp::rmDhclientPid - Start");
        NCCHAR pidFile[] = VAR_DHCLIENT_PID;
        FILE* fp = fopen(pidFile, "r");
        if (NULL == fp) {
            NCLOGD("NHNetworkDhcp::rmDhclientPid, open pid file failed");
            return;
        }

        NCCHAR tempfile[] = VAR_DHCLIENT_PID_TEMP;
        if (NCFile::exists(tempfile)) {
            NCLOGD("NHNetworkDhcp::rmDhclientPid - [%s] is exist", tempfile);
            NCFile::remove(tempfile);
        }

        LONG temp = 0;
        NC_BOOL tempFlag = NC_FALSE;
        FILE* tempFp = fopen(tempfile, "at+");
        if (NULL != tempFp) {
            NCCHAR buf[128] = { 0 };
            while (NULL != fgets(buf, sizeof(buf) -1, fp)) {
                buf[strlen(buf) - 1] = '\0';
                NCLOGD("NHNetworkDhcp::rmDhclientPid, deviceName [%s], buf [%s]", deviceName.getString(), buf);
                if (strncmp(deviceName.getString(), buf, strlen(deviceName.getString()))) {
                    fprintf (tempFp, "%s\n", buf);
                }
                else {
                    tempFlag = NC_TRUE;
                    sscanf(buf, "%*s%10ld", &temp);
                }
            }

            fclose(tempFp);
        }

        fclose(fp);

        pid_t pid = (pid_t)temp;
        if (tempFlag && pid) {
            if (kill(pid, SIGKILL) == 0) {
                NCLOGD("NHNetworkDhcp::rmDhclientPid, Kill dhclient process success");
            }
        }
        else {
            NCLOGD("NHNetworkDhcp::rmDhclientPid, Find dhclient pid failed");
        }

        NCFile::remove(VAR_DHCLIENT_PID);
        if (!NCFile::move(tempfile, VAR_DHCLIENT_PID)) {
            NCLOGD("NHNetworkDhcp::rmDhclientPid, Move temp file to pid file failed");
        }
    }

} /* namespace nutshell */
/* EOF */
