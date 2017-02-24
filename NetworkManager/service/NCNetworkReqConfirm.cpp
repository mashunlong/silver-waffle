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
#include "NCNetworkThread.h"
#include "NCNetworkManager.h"
#include "NCNetworkManagerDef.h"
#include "NCNetworkReqConfirm.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{
    NCNetworkReqConfirm::NCNetworkReqConfirm()
    {

    }

    NCNetworkReqConfirm::~NCNetworkReqConfirm()
    {

    }

    VOID
    NCNetworkReqConfirm::confirm()
    {
        NCLOGD("NCNetworkReqConfirm::confirm - Start");
        NCNetworkManager::Instance()->getNetworkThread()->notifyRequest();
    }

    NCString
    NCNetworkReqConfirm::getDeviceType(const UINT32 reqType)
    {
        NCLOGD("NCNetworkReqConfirm::getDeviceType, Start");
        NCSpThread<NCNetworkReqBase>::sp cur = NULL;
        NCNetworkManager::Instance()->getNetworkThread()->curRequest(cur);
        if (cur == NULL) {
            NCLOGD("NCNetworkReqConfirm::getDeviceType, Get current request failed");
            return NCString("");
        }

        if (reqType != cur->getFuncType()) {
            NCLOGD("NCNetworkReqConfirm::getDeviceType, Current request is not incorrect");
            return NCString("");
        }

        return cur->getDeviceType();
    }

    NCString
    NCNetworkReqConfirm::getDeviceName(const UINT32 reqType)
    {
        NCLOGD("NCNetworkReqConfirm::getDeviceName, Start");
        NCSpThread<NCNetworkReqBase>::sp cur = NULL;
        NCNetworkManager::Instance()->getNetworkThread()->curRequest(cur);
        if (cur == NULL) {
            NCLOGD("NCNetworkReqConfirm::getDeviceName, Get current request failed");
            return NCString("");
        }

        if (reqType != cur->getFuncType()) {
            NCLOGD("NCNetworkReqConfirm::getDeviceName, Current request is not incorrect");
            return NCString("");
        }

        return cur->getDeviceName();
    }

    UINT32
    NCNetworkReqConfirm::getLastRequest(const UINT32 reqType)
    {
        NCLOGD("NCNetworkReqConfirm::getLastRequest, Start");
        NCSpThread<NCNetworkReqBase>::sp cur = NULL;
        NCNetworkManager::Instance()->getNetworkThread()->curRequest(cur);
        if (cur == NULL) {
            NCLOGD("NCNetworkReqConfirm::getLastRequest, Get current request failed");
            return RequestType_Init;
        }

        if (reqType != cur->getFuncType()) {
            NCLOGD("NCNetworkReqConfirm::getLastRequest, Current request is not incorrect");
            return RequestType_Init;
        }

        return cur->getLastRequest();
    }

    NC_BOOL
    NCNetworkReqConfirm::getRemoteFlag(const UINT32 reqType)
    {
        NCLOGD("NCNetworkReqConfirm::getRemoteFlag, Start");
        NCSpThread<NCNetworkReqBase>::sp cur = NULL;
        NCNetworkManager::Instance()->getNetworkThread()->curRequest(cur);
        if (cur == NULL) {
            NCLOGD("NCNetworkReqConfirm::getRemoteFlag, Get current request failed");
            return NC_FALSE;
        }

        if (reqType != cur->getFuncType()) {
            NCLOGD("NCNetworkReqConfirm::getRemoteFlag, Current request is not incorrect");
            return NC_FALSE;
        }

        return cur->getRemoteFlag();
    }

    UINT32
    NCNetworkReqConfirm::getReqState(const UINT32 reqType)
    {
        NCLOGD("NCNetworkReqConfirm::getReqState, Start, reqType [%d]", reqType);
        NCSpThread<NCNetworkReqBase>::sp cur = NULL;
        NCNetworkManager::Instance()->getNetworkThread()->curRequest(cur);
        if (cur == NULL) {
            NCLOGD("NCNetworkReqConfirm::getReqState, Get current request failed");
            return NC_FALSE;
        }

        if (reqType != cur->getFuncType()) {
            NCLOGD("NCNetworkReqConfirm::getReqState, Current request is not incorrect");
            return NC_FALSE;
        }

        return cur->getReqState();
    }

    VOID
    NCNetworkReqConfirm::setReqState(const UINT32 reqType, const UINT32 state)
    {
        NCLOGD("NCNetworkReqConfirm::setReqState, Start, reqType [%d]", reqType);
        NCSpThread<NCNetworkReqBase>::sp cur = NULL;
        NCNetworkManager::Instance()->getNetworkThread()->curRequest(cur);
        if (cur == NULL) {
            NCLOGD("NCNetworkReqConfirm::setReqState, Get current request failed");
            return;
        }

        if (reqType != cur->getFuncType()) {
            NCLOGD("NCNetworkReqConfirm::setReqState, Current request is not incorrect");
            return;
        }

        cur->setReqState(state);
    }

    VOID
    NCNetworkReqConfirm::lockObject(const UINT32 reqType)
    {
        NCLOGD("NCNetworkReqConfirm::lockObject, Start, reqType [%d]", reqType);
        NCSpThread<NCNetworkReqBase>::sp cur = NULL;
        NCNetworkManager::Instance()->getNetworkThread()->curRequest(cur);
        if (cur == NULL) {
            NCLOGD("NCNetworkReqConfirm::lockObject, Get current request failed");
            return;
        }

        if (reqType != cur->getFuncType()) {
            NCLOGD("NCNetworkReqConfirm::lockObject, Current request is not incorrect");
            return;
        }

        cur->lockObject();
    }

    VOID
    NCNetworkReqConfirm::unlockObject(const UINT32 reqType)
    {
        NCLOGD("NCNetworkReqConfirm::unlockObject, Start, reqType [%d]", reqType);
        NCSpThread<NCNetworkReqBase>::sp cur = NULL;
        NCNetworkManager::Instance()->getNetworkThread()->curRequest(cur);
        if (cur == NULL) {
            NCLOGD("NCNetworkReqConfirm::unlockObject, Get current request failed");
            return;
        }

        if (reqType != cur->getFuncType()) {
            NCLOGD("NCNetworkReqConfirm::unlockObject, Current request is not incorrect");
            return;
        }

        cur->unlockObject();
    }

} /* namespace nutshell */
/* EOF */