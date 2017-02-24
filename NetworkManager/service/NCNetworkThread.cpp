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
#include <ncore/NCError.h>
#include <cutils/properties.h>
#include "NCNetworkManagerDef.h"
#include "NCNetworkManager.h"
#include "NCNetworkTimer.h"
#include "NCNetworkStatus.h"
#include "NCNetworkHandler.h"
#include "NCNetworkPluginManager.h"
#include "NCNetworkNotify.h"
#include "NCNetworkAccessStack.h"
#include "NCNetworkThread.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{
    NCNetworkThread::NCNetworkThread(const NCString& threadName)
    : NCSpThread<NCNetworkReqBase>(threadName)
    {

    }

    NCNetworkThread::~NCNetworkThread()
    {
        
    }

    VOID
    NCNetworkThread::insertRequest(NCNetworkReqBase* req)
    {
        mSyncObj.syncStart();
        startThread(mThreadName);
        mReqList.insertAt(req, 0);
        mSyncObj.syncEnd();
        toNotify();

    }

    VOID
    NCNetworkThread::removeSimilarDhcpRequestInList(const UINT32& requestType, const NCString& deviceType, const NCString& deviceName)
    {
        NCLOGD("NCNetworkThread::removeSimilarDhcpRequestInList, Start, requestType [%d], deviceType [%s], deviceName [%s]", requestType, 
            deviceType.getString(), deviceName.getString());
        if (RequestType_ReleaseIp == requestType) {
            if (lockReqState(RequestType_RequestIp)) {
                NCLOGD("NCNetworkThread::removeSimilarDhcpRequestInList, Current reqest is request ip");
                NCString type = curReq->getDeviceType();
                NCString name = curReq->getDeviceName();
                if (type == deviceType && name == deviceName) {
                    property_set(NCNETWORK_DHCLIENT_CANCEL_FLAG, "on");
                }

                unlockReqState(RequestType_RequestIp);
            }
            else {
                NCLOGD("NCNetworkThread::removeSimilarDhcpRequestInList, Current reqest is not request ip");
            }
        }

        NCAutoSync lock(mSyncObj);
        if (mReqList.isEmpty()) {
            NCLOGD("NCNetworkThread::removeSimilarDhcpRequestInList, List is empty");
            return;
        }

        for (Vector::iterator it = mReqList.begin(); it != mReqList.end();) {
            NCSpThread<NCNetworkReqBase>::sp cur = *it;
            if (RequestType_RequestIp == cur->getFuncType() || RequestType_ReleaseIp == cur->getFuncType()) {
                NCString type = cur->getDeviceType();
                NCString name = cur->getDeviceName();
                if (type == deviceType && name == deviceName) {
                    NCLOGD("NCNetworkThread::removeSimilarDhcpRequestInList, Remove success");
                    it = mReqList.erase(it);
                }
                else {
                    ++it;
                }
            }
            else {
                ++it;
            }
        }
    }

    NC_BOOL
    NCNetworkThread::checkReqList(const AccessParameter& param)
    {
        NCLOGD("NCNetworkThread::checkReqList, Start");
        NCAutoSync lock(mSyncObj);
        if (!mReqList.isEmpty()) {
            NCLOGD("NCNetworkThread::checkRelesae, list is not empty");
            NCSpThread<NCNetworkReqBase>::sp lastReq = NULL;
            for (Vector::iterator it = mReqList.begin(); it != mReqList.end();) {
                NCSpThread<NCNetworkReqBase>::sp cur = *it;
                if (RequestType_Access == cur->getFuncType() && param.deviceType == cur->getDeviceType()) {
                    lastReq = *it;
                }
                else if (RequestType_ClearDefaultNetwork == cur->getFuncType() && param.deviceType == cur->getDeviceType()) {
                    lastReq = *it;
                }

                ++it;
            }

            if (lastReq != NULL) {
                NCLOGD("NCNetworkThread::checkReqList, lastReq is not NULL");
                if (RequestType_Access == lastReq->getFuncType() && param.deviceType == lastReq->getDeviceType()) {
                    NCLOGD("NCNetworkThread::checkReqList, add request to list");
                    lastReq->addRequestList(param);
                    return NC_TRUE;
                }
                else if (RequestType_ClearDefaultNetwork == lastReq->getFuncType() && param.deviceType == lastReq->getDeviceType()) {
                    NCLOGD("NCNetworkThread::checkReqList, post request to list");
                    switch (param.reqType) {
                    case RequestType_Access:
                    {
                        NCLOGD("NCNetworkThread::checkReqList, RequestType_Access");
                        postRequest(new NCNetworkRequestAccessReq(param));
                        return NC_TRUE;
                    }
                    case RequestType_Release:
                    {
                        NCLOGD("NCNetworkThread::checkReqList, RequestType_Release");
                        postRequest(new NCNetworkReleaseReq(param));
                        return NC_TRUE;
                    }
                    default:
                    {
                        NCLOGD("NCNetworkThread::checkReqList, default");
                        return NC_FALSE;
                    }
                    }
                }
            }
        }

        NCLOGD("NCNetworkThread::checkReqList, List last req is null");
        return NC_FALSE;
    }


    NC_BOOL
    NCNetworkThread::checkCurrentReq(const AccessParameter& param)
    {
        NCLOGD("NCNetworkThread::checkCurrentReq, Start");
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkPluginManager* myWorkPluginManager = NCNetworkManager::Instance()->getNetworkPluginManager();
        if (NULL == myAccessStack || NULL == myStatus || NULL == myWorkPluginManager) {
            NCLOGD("NCNetworkThread::checkCurrentReq, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return NC_FALSE;
        }

        if (!lockReqState(RequestType_Access)) {
            NCLOGD("NCNetworkThread::checkCurrentReq, lock reqstate failed");
            return NC_FALSE;
        }

        if (param.deviceType == curReq->getDeviceType()) {
            curReq->lockObject();
            // before pppd
            if (REQ_INIT == curReq->getReqState()) {
                NCLOGD("NCNetworkThread::checkCurrentReq, Before dial pppd");
                curReq->addRequestList(param);
                curReq->unlockObject();
                unlockReqState(RequestType_Access);
                return NC_TRUE;
            }

            // in pppd
            if (REQ_DIALSTART == curReq->getReqState()) {
                NCLOGD("NCNetworkThread::checkCurrentReq, Dialing pppd ...");
                switch (param.reqType) {
                case RequestType_Access:
                {
                    NCLOGD("NCNetworkThread::checkCurrentReq, RequestType_Access");
                    myAccessStack->addRequestMap(RequestMapType_Init, param.reqId, param);
                    myWorkPluginManager->requestNetworkAccess(param);
                    curReq->unlockObject();
                    unlockReqState(RequestType_Access);
                    return NC_TRUE;
                }
                case RequestType_Release:
                {
                    NCLOGD("NCNetworkThread::checkCurrentReq, RequestType_Release");
                    myAccessStack->addRequestMap(RequestMapType_Init, param.reqId, param);
                    myWorkPluginManager->updateNetworkAccess(param.deviceType, param.resourceId, param.reqId);
                    curReq->unlockObject();
                    unlockReqState(RequestType_Access);
                    return NC_TRUE;
                }
                case RequestType_Disconnect:
                {
                    NCLOGD("NCNetworkThread::checkCurrentReq, RequestType_Disconnect");
                    myStatus->setLinkState(param.deviceType, DialState_Disconnecting, DialType_None);
                    myAccessStack->addRequestMap(RequestMapType_Init, param.reqId, param);
                    myWorkPluginManager->releaseNetworkAccess(param.deviceType, param.reqId);
                    curReq->unlockObject();
                    unlockReqState(RequestType_Access);
                    return NC_TRUE;
                }
                default:
                {
                    NCLOGD("NCNetworkThread::checkReqList, Bug, reqType error");
                    curReq->unlockObject();
                    unlockReqState(RequestType_Access);
                    return NC_FALSE;
                }
                }
            }

            curReq->unlockObject();
        }

        unlockReqState(RequestType_Access);
        return NC_FALSE;
    }

    NC_BOOL
    NCNetworkThread::lockReqState(const UINT32 reqType)
    {
        NCLOGD("NCNetworkThread::lockReqState, Start, reqType [%d]", reqType);
        NCAutoSync lock(mSyncObj);
        if (curReq == NULL) {
            NCLOGD("NCNetworkThread::lockReqState, Current req is null");
            return NC_FALSE;
        }

        UINT32 currReqType = curReq->getFuncType();
        if (reqType != currReqType) {
            NCLOGD("NCNetworkThread::lockReqState, Current req is not match");
            return NC_FALSE;
        }

        UINT32 reqState = curReq->getReqState();
        if ((REQ_CONFIRMED == reqState) || (REQ_TIMEOUT == reqState)) {
            NCLOGD("NCNetworkThread::lockReqState, Current req is confirmed or timeout");
            return NC_FALSE;
        }

        curReq->lockReqState();
        return NC_TRUE;
    }

    NC_BOOL
    NCNetworkThread::unlockReqState(const UINT32 reqType)
    {
        NCLOGD("NCNetworkThread::unlockReqState, Start, reqType [%d]", reqType);
        NCAutoSync lock(mSyncObj);
        if (curReq == NULL) {
            NCLOGD("NCNetworkThread::unlockReqState, Current req is null");
            return NC_FALSE;
        }

        UINT32 currReqType = curReq->getFuncType();
        if (reqType != currReqType) {
            NCLOGD("NCNetworkThread::unlockReqState, Current req is not match");
            return NC_FALSE;
        }

        curReq->unlockReqState();
        return NC_TRUE;
    }

} /* namespace nutshell */
/* EOF */