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
#include <system/PfServiceDefine.h>
#include "NCNetworkManagerDef.h"
#include "networkmanager/NCHttpClientManager.h"

#define NC_LOG_TAG "NCHttpClientManager"

namespace nutshell
{
    NCHttpClientManager::NCHttpClientManager()
    : NCBinderServiceProxy(PFSERVICE_NETWORKMANAGER_NAME)
    , NEServiceProxy(NULL)
    {

    }

    NCHttpClientManager::~NCHttpClientManager()
    {

    }

    INT32
    NCHttpClientManager::getAccessPermission()
    {
        INT32 result = -1;
        UINT32 pID = syscall(SYS_getpid);
        UINT32 tID = syscall(SYS_gettid);
        NCString taskName = getSelfTaskName();

        NCLOGD("NCHttpClientManager::getAccessPermission - Start, name[%s] pID[%d] tID[%d]", taskName.getString(), pID, tID);
        NCParcel data, reply;
        data.writeCString(taskName.getString());
        data.writeInt32(pID);
        data.writeInt32(tID);
        INT32 ret = sendCommand(FunctionId_RequestHttpClientAccess, data, &reply);
        if (NC_NOERROR == ret) {
            result = reply.readInt32();
        }
        return result;
    }

    INT32
    NCHttpClientManager::releaseAccessPermission()
    {
        INT32 result = -1;
        UINT32 pID = syscall(SYS_getpid);
        UINT32 tID = syscall(SYS_gettid);

        NCLOGD("NCHttpClientManager::releaseAccessPermission - Start, pID = [%d], tID = [%d]", pID, tID);
        NCParcel data, reply;
        data.writeInt32(pID);
        data.writeInt32(tID);
        INT32 ret = sendCommand(FunctionId_ReleaseHttpClientAccess, data, &reply);
        if (NC_NOERROR == ret) {
            result = reply.readInt32();
        }
        return result;
    }

    INT32
    NCHttpClientManager::setLogFileFlag(NC_BOOL flag)
    {
        INT32 result = -1;

        NCLOGD("NCHttpClientManager::setLogFileFlag - Start, flag[%d]", flag);
        NCParcel data, reply;
        data.writeInt32(flag);
        INT32 ret = sendCommand(FunctionId_SetHttpLogFlag, data, &reply);
        if (NC_NOERROR == ret) {
            result = reply.readInt32();
        }
        return result;
    }

    INT32
    NCHttpClientManager::getLogFileFlag(NC_BOOL &flag)
    {
        INT32 result = -1;

        NCLOGD("NCHttpClientManager::getLogFileFlag - Start");
        NCParcel data, reply;
        INT32 ret = sendCommand(FunctionId_GetHttpLogFlag, data, &reply);
        if (NC_NOERROR == ret) {
            result = reply.readInt32();
            flag = reply.readInt32();
        }
        return result;
    }

    NCString
    NCHttpClientManager::getSelfTaskName()
    {
#define NCHTTPCLIENTMANAGER_TASKNAME_SIZE    1024
        NCString TaskName = NCString("NotFound");
        CHAR processname[NCHTTPCLIENTMANAGER_TASKNAME_SIZE] = { 0 };

        INT32 len = readlink("/proc/self/exe", processname, NCHTTPCLIENTMANAGER_TASKNAME_SIZE - 1);
        if (len <= 0) {
            return TaskName;
        }
        processname[len] = '\0';

        TaskName = NCString(processname);

        return TaskName;
    }
} /* namespace nutshell */
/* EOF */
