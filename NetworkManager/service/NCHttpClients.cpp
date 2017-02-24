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

#include <dirent.h>
#include <ncore/NCLog.h>
#include "networkmanager/NCNetworkDef.h"
#include "NCNetworkManagerDef.h"
#include "NCHttpClients.h"

#define NC_LOG_TAG "NCHttpClientManager"

namespace nutshell
{
    NCHttpClients::NCHttpClients()
    {
        mAccessList.clear();
        mTotalAccessNum = 0;
    }

    NCHttpClients::~NCHttpClients()
    {

    }

    VOID
    NCHttpClients::onCommand(UINT32 code, const NCParcel& data, NCParcel* reply, UINT32 flags)
    {
        switch (code) {
        case FunctionId_RequestHttpClientAccess:
        {
            NCString taskName(data.readCString());
            INT32 pID = data.readInt32();
            INT32 tID = data.readInt32();
            INT32 result = getAccessPermission(taskName, pID, tID);
            NCLOGD("NCHttpClients::onCommand - Request HttpClient access. PID[%d] TID[%d] Restlt[0x%x]", pID, tID, result);

            if (NULL != reply) {
                reply->write(&result, sizeof(INT32));
            }
            else {
                NCLOGD("NCHttpClients::onCommand - Reply is NULL [FunctionId_RequestHttpClientAccess]");
            }
        }
        break;
        case FunctionId_ReleaseHttpClientAccess:
        {
            INT32 pID = data.readInt32();
            INT32 tID = data.readInt32();
            INT32 result = releaseAccessPermission(pID, tID);
            NCLOGD("NCHttpClients::onCommand - Release HttpClient access. PID[%d] TID[%d] Restlt[0x%x]", pID, tID, result);

            if (NULL != reply) {
                reply->write(&result, sizeof(INT32));
            }
            else {
                NCLOGD("NCHttpClients::onCommand - Reply is NULL [FunctionId_ReleaseHttpClientAccess]");
            }
        }
        break;
        case FunctionId_SetHttpLogFlag:
        {
            INT32 logflag = data.readInt32();
            INT32 result = setLogFileFlag(logflag);
            NCLOGD("NCHttpClients::onCommand - Set HttpLog Flag. Flag[%d] Restlt[0x%x]", logflag, result);

            if (NULL != reply) {
                reply->write(&result, sizeof(INT32));
            }
            else {
                NCLOGD("NCHttpClients::onCommand - Reply is NULL [FunctionId_SetHttpLogFlag]");
            }
        }
        break;
        case FunctionId_GetHttpLogFlag:
        {
            INT32 logflag = 0;
            INT32 result = getLogFileFlag(logflag);
            NCLOGD("NCHttpClients::onCommand - Get HttpLog Flag. Flag[%d] Restlt[0x%x]", logflag, result);

            if (NULL != reply) {
                reply->write(&result, sizeof(INT32));
                reply->write(&logflag, sizeof(INT32));
            }
            else {
                NCLOGD("NCHttpClients::onCommand - Reply is NULL [FunctionId_GetHttpLogFlag]");
            }
        }
        break;
        default:
        {
            NCLOGD("NCHttpClients::onCommand - Error code[0x%x]", code);
        }
        break;
        }
    }

    NCString
    NCHttpClients::dumpService()
    {
        NCString out = "";
        NCString str = "";
        int index = 1;
        std::vector<HttpClientAccessNode>::iterator accessListIter;

        out += "NCHttpClients: All socket permission below:\n";

        // printf all data
        for (accessListIter = mAccessList.begin(); accessListIter != mAccessList.end(); accessListIter.operator++()) {
            std::vector<INT32>::iterator tIDListIter;
            for (tIDListIter = accessListIter->tIDList.begin(); tIDListIter != accessListIter->tIDList.end(); tIDListIter++) {
                str.format("[%04d]------>[%05d][%05d][%s]\n", index++, accessListIter->pID, *tIDListIter, accessListIter->TaskName.getString());
                out += str;
            }
        }

        return out;
    }

    INT32
    NCHttpClients::getAccessPermission(const NCString& taskName, const INT32 pID, const INT32 tID)
    {
        NCAutoSync autoSync(m_syncLock);
        INT32 result = NetworkErrCode_Fail;
        std::vector<HttpClientAccessNode>::iterator accessListIter;

        if (mTotalAccessNum+1 > NCNETWORK_HTTPCLIENT_MAXACCESS_TOTAL) {
            return NetworkErrCode_HttpClientMgr_TotalNumFull;
        }

        // get pid in pidlist
        for (accessListIter = mAccessList.begin(); accessListIter != mAccessList.end(); accessListIter.operator++()) {
            if (pID == accessListIter->pID) {
                // get node in list
                break;
            }
        }

        if (accessListIter != mAccessList.end()) {
            if (accessListIter->tIDList.size()+1 <= NCNETWORK_HTTPCLIENT_MAXACCESS_PERTASK) {
                accessListIter->tIDList.push_back(tID);
                mTotalAccessNum++;
                result = NetworkErrCode_Success;
            }
            else {
                result = NetworkErrCode_HttpClientMgr_TaskNumFull;
            }
        }
        else {
            HttpClientAccessNode newNode;
            newNode.tIDList.clear();
            newNode.TaskName = taskName;
            newNode.pID = pID;
            newNode.tIDList.push_back(tID);
            mAccessList.push_back(newNode);
            mTotalAccessNum++;
            result = NetworkErrCode_Success;
        }

        // check idle access permission
        if (NCNETWORK_HTTPCLIENT_ACCESSCHECK_NUM <= mTotalAccessNum) {
            std::vector<INT32> pIDList;
            INT32 ret = getPIDList(pIDList);
            if (0 >= ret) {
                return result;
            }

            for (accessListIter = mAccessList.begin(); accessListIter != mAccessList.end();) {
                if (NC_FALSE == isPIDActive(pIDList, accessListIter->pID)) {
                    mTotalAccessNum -= accessListIter->tIDList.size();
                    accessListIter = mAccessList.erase(accessListIter);

                    if (accessListIter == mAccessList.end()) {
                        break;
                    }
                }
                else {
                    accessListIter.operator++();
                }
            }
        }
        return result;
    }

    INT32
    NCHttpClients::releaseAccessPermission(const INT32 pID, const INT32 tID)
    {
        NCAutoSync autoSync(m_syncLock);
        INT32 result = NetworkErrCode_HttpClientMgr_TaskUnknow;
        std::vector<HttpClientAccessNode>::iterator accessListIter;

        if (0 >= mTotalAccessNum) {
            return NetworkErrCode_Fail;
        }

        for (accessListIter = mAccessList.begin(); accessListIter != mAccessList.end(); accessListIter.operator++()) {
            if (pID == accessListIter->pID) {
                std::vector<INT32>::iterator tIDListIter;
                for (tIDListIter = accessListIter->tIDList.begin(); tIDListIter != accessListIter->tIDList.end(); tIDListIter++) {
                    if (tID == *tIDListIter) {
                        accessListIter->tIDList.erase(tIDListIter);
                        mTotalAccessNum--;
                        result = NetworkErrCode_Success;
                        break;
                    }
                }

                if (0 == accessListIter->tIDList.size()) {
                    mAccessList.erase(accessListIter);
                }
                break;
            }
        }

        return result;
    }

    INT32
    NCHttpClients::setLogFileFlag(INT32 flag)
    {
        INT32 result = NetworkErrCode_Success;
        INT32 accRet = access(NCNETWORK_HTTPCLIENT_LOGFLAG_FILEPATH, F_OK);

        NCLOGD("NCHttpClients::setLogFileFlag - Flag[0x%x] access[0x%x]", flag, accRet);

        if (NC_TRUE == flag && 0 != accRet) {
            FILE *pf = NULL;
            pf = fopen(NCNETWORK_HTTPCLIENT_LOGFLAG_FILEPATH, "w");
            if (NULL != pf) {
                fclose(pf);
            }
            else {
                NCLOGD("NCHttpClients::setLogFileFlag - create file Error[0x%x]", pf);
                result = NetworkErrCode_HttpClientMgr_FileFlgError;
            }
        }
        else if (NC_FALSE == flag && 0 == accRet) {
            INT32 ret = unlink(NCNETWORK_HTTPCLIENT_LOGFLAG_FILEPATH);
            if (0 != ret) {
                NCLOGD("NCHttpClients::setLogFileFlag - unlink Error[0x%x]", ret);
                result = NetworkErrCode_HttpClientMgr_FileFlgError;
            }
        }

        return result;
    }

    INT32
    NCHttpClients::getLogFileFlag(INT32 &flag)
    {
        INT32 result = NetworkErrCode_Success;

        if (0 == access(NCNETWORK_HTTPCLIENT_LOGFLAG_FILEPATH, F_OK)) {
            flag = (INT32)(NC_TRUE);
        }
        else {
            flag = (INT32)(NC_FALSE);
        }

        return result;
    }

    INT32
    NCHttpClients::getPIDList(std::vector<INT32>& pIDList)
    {
        DIR *pdir = NULL;
        INT32 pID = 0;
        struct dirent *pde = NULL;
        pIDList.clear();

        pdir = opendir("/proc");
        if  (NULL == pdir) {
          return -1;
         }

        pde = readdir(pdir);
        while (NULL != pde) {
            if ((pde->d_name[0] < '0')  || (pde->d_name[0] > '9')) {
                pde = readdir(pdir);
                continue;
            }
            pID = atoi(pde->d_name);
            pIDList.push_back(pID);

            pde = readdir(pdir);
        }
        closedir(pdir);

        return pIDList.size();
    }

    NC_BOOL
    NCHttpClients::isPIDActive(const std::vector<INT32> &pIDList, INT32 PID)
    {
        std::vector<INT32>::const_iterator pIDListIter;
        for (pIDListIter = pIDList.begin(); pIDListIter != pIDList.end(); pIDListIter.operator++()) {
            if (PID == *pIDListIter) {
                return NC_TRUE;
            }
            else if (PID < *pIDListIter) {
                return NC_FALSE;
            }
        }
        return NC_FALSE;
    }

} /* namespace nutshell */
/* EOF */
