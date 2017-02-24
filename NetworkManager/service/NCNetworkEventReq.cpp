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
#include "NCNetworkHandler.h"
#include "NCNetworkManager.h"
#include "NCNetworkAccessStack.h"
#include "NCNetworkStatus.h"
#include "NCNetworkNotify.h"
#include "NCNetworkReqConfirm.h"
#include "NCNetworkPluginManager.h"
#include "NCNetworkThread.h"
#include "NCNetworkEventReq.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{
    NCNetworkCheckRequstReq::NCNetworkCheckRequstReq(const AccessParameter& param)
    : m_param(param)
    {

    }

    NCNetworkCheckRequstReq::~NCNetworkCheckRequstReq()
    {

    }

    VOID
    NCNetworkCheckRequstReq::run()
    {
        NCLOGD("NCNetworkCheckRequstReq::run, Start");
        NCLOGD("NCNetworkCheckRequstReq::ReqId [%d], resourceId [%s], deviceType [%s], remoteFlag [%d]", m_param.reqId, 
            m_param.resourceId.getString(), m_param.deviceType.getString(), m_param.remoteFlag);
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkPluginManager* myWorkPluginManager = NCNetworkManager::Instance()->getNetworkPluginManager();
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        if (NULL == myAccessStack || NULL == myStatus || NULL == myWorkPluginManager || NULL == myWorkThread) {
            NCLOGD("NCNetworkCheckRequstReq::run, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            AccessParameter tempParam = m_param;
            tempParam.result = NetworkErrCode_Fail;
            NCNetworkReplyRequestMapReq replyReq(tempParam);
            replyReq.doReplyRequest();
            return;
        }

        if (DCM != m_param.deviceType && BT_DUN != m_param.deviceType && WIFI_STA != m_param.deviceType) {
            NCLOGD("NCNetworkCheckRequstReq::run, Parameters error");
            AccessParameter tempParam = m_param;
            tempParam.result = NetworkErrCode_Fail;
            NCNetworkReplyRequestMapReq replyReq(tempParam);
            replyReq.doReplyRequest();
            return;
        }

        NCNetworkHandler handler;
        myAccessStack->addResourceId(m_param.resourceId, m_param.deviceType);

        NCString currNetwork = myStatus->currentNetwork(); 
        NCLOGD("NCNetworkCheckRequstReq::run, Current network [%s]", currNetwork.getString());
        if (!m_param.remoteFlag) {
            if (!handler.checkConnectPermission(m_param.resourceId)) {
                NCLOGD("NCNetworkCheckRequstReq::run, Verify permission failed");
                AccessParameter tempParam = m_param;
                tempParam.result = NetworkErrCode_Fail;
                NCNetworkReplyRequestMapReq replyReq(tempParam);
                replyReq.doReplyRequest();
                return;
            }

            if (handler.isDummyDcmOn()) {
                NCLOGD("NCNetworkCheckRequstReq::run, Dummy on");
                if (WIFI_STA == currNetwork) {
                    NCLOGD("NCNetworkCheckRequstReq::run, Dummy on, Request network success");
                    AccessParameter tempParam = m_param;
                    tempParam.result = NetworkErrCode_Success;
                    NCNetworkReplyRequestMapReq replyReq(tempParam);
                    replyReq.doReplyRequest();
                }
                else {
                    NCLOGD("NCNetworkCheckRequstReq::run, Dummy on, Request network failed");
                    AccessParameter tempParam = m_param;
                    tempParam.result = NetworkErrCode_Fail;
                    NCNetworkReplyRequestMapReq replyReq(tempParam);
                    replyReq.doReplyRequest();
                }

                return;
            }
        }

        if (NCString("") != currNetwork && m_param.deviceType != currNetwork) {
            NCLOGD("NCNetworkCheckRequstReq::run, Current network is not equal deviceType");
            AccessParameter tempParam = m_param;
            tempParam.result = NetworkErrCode_Fail;
            NCNetworkReplyRequestMapReq replyReq(tempParam);
            replyReq.doReplyRequest();
            return;
        }

        if (WIFI_STA == m_param.deviceType) {
            NCNetworkManager::Instance()->postRequest(new NCNetworkRequestAccessReq(m_param));
            return;
        }

        NC_BOOL isDevExist = handler.isCarryNetworkDevice(m_param.deviceType);
        if (!isDevExist) {
            NCLOGD("NCNetworkCheckRequstReq::run, isRemote");
            myAccessStack->addRequestMap(RequestMapType_Init, m_param.reqId, m_param);            
            if (NCString("") == currNetwork) {
                myStatus->setDialState(m_param.deviceType, DialState_Connecting, m_param.dealType);
            }

            myWorkPluginManager->requestNetworkAccess(m_param);
            return;
        }

        NCLOGD("NCNetworkCheckRequstReq::run, isLocal");
        if (myWorkThread->checkReqList(m_param)) {
            NCLOGD("NCNetworkCheckRequstReq::run, Check req list success");
            return;
        }

        if (myWorkThread->checkCurrentReq(m_param)) {
            NCLOGD("NCNetworkCheckRequstReq::run, Check currrent req success");
            return;
        }

        NCLOGD("NCNetworkCheckRequstReq::run, post request to list");
        NCNetworkManager::Instance()->postRequest(new NCNetworkRequestAccessReq(m_param));
    }

    NCNetworkCheckReleaseReq::NCNetworkCheckReleaseReq(const AccessParameter& param)
    : m_param(param)
    {

    }

    NCNetworkCheckReleaseReq::~NCNetworkCheckReleaseReq()
    {

    }

    VOID
    NCNetworkCheckReleaseReq::run()
    {
        NCLOGD("NCNetworkCheckReleaseReq::run, Start");
        NCLOGD("NCNetworkCheckReleaseReq::ReqId [%d], resourceId [%s]", m_param.reqId, 
            m_param.resourceId.getString());
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        if (NULL == myWorkThread || NULL == myAccessStack) {
            NCLOGD("NCNetworkCheckReleaseReq::run, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            AccessParameter tempParam = m_param;
            tempParam.result = NetworkErrCode_Fail;
            NCNetworkReplyRequestMapReq replyReq(tempParam);
            replyReq.doReplyRequest();
            return;
        }

        myAccessStack->getResourceId(m_param.resourceId, m_param.deviceType);
        NCLOGD("NCNetworkCheckReleaseReq::run, Get deviceType [%s]", m_param.deviceType.getString());
        if (DCM != m_param.deviceType && BT_DUN != m_param.deviceType && WIFI_STA != m_param.deviceType) {
            NCLOGD("NCNetworkCheckReleaseReq::run, Parameters error");
            AccessParameter tempParam = m_param;
            tempParam.result = NetworkErrCode_Fail;
            NCNetworkReplyRequestMapReq replyReq(tempParam);
            replyReq.doReplyRequest();
            return;
        }

        if (WIFI_STA == m_param.deviceType) {
            NCNetworkManager::Instance()->postRequest(new NCNetworkReleaseReq(m_param));
            return;
        }

        if (myWorkThread->checkReqList(m_param)) {
            NCLOGD("NCNetworkCheckReleaseReq::run, Check req list success");
            return;
        }

        if (myWorkThread->checkCurrentReq(m_param)) {
            NCLOGD("NCNetworkCheckReleaseReq::run, Check currrent req success");
            return;
        }

        NCLOGD("NCNetworkCheckReleaseReq::run, post request to list");
        NCNetworkManager::Instance()->postRequest(new NCNetworkReleaseReq(m_param));
        return;
    }

    NCNetworkCheckDisconnectReq::NCNetworkCheckDisconnectReq(const AccessParameter& param)
    : m_param(param)
    {

    }

    NCNetworkCheckDisconnectReq::~NCNetworkCheckDisconnectReq()
    {

    }

    VOID
    NCNetworkCheckDisconnectReq::run()
    {
        NCLOGD("NCNetworkCheckDisconnectReq::run, Start");
        NCLOGD("NCNetworkCheckDisconnectReq::ReqId [%d], reqType [%d], deviceType [%s], remoteFlag [%d]", m_param.reqId, 
            m_param.reqType, m_param.deviceType.getString(), m_param.remoteFlag);
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkPluginManager* myWorkPluginManager = NCNetworkManager::Instance()->getNetworkPluginManager();
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        if (NULL == myAccessStack || NULL == myStatus || NULL == myWorkPluginManager || NULL == myWorkThread) {
            NCLOGD("NCNetworkCheckDisconnectReq::run, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            AccessParameter tempParam = m_param;
            tempParam.result = NetworkErrCode_Fail;
            NCNetworkReplyRequestMapReq replyReq(tempParam);
            replyReq.doReplyRequest();
            return;
        }

        if (DCM != m_param.deviceType && BT_DUN != m_param.deviceType && WIFI_STA != m_param.deviceType) {
            NCLOGD("NCNetworkCheckDisconnectReq::run, Parameters error");
            AccessParameter tempParam = m_param;
            tempParam.result = NetworkErrCode_Fail;
            NCNetworkReplyRequestMapReq replyReq(tempParam);
            replyReq.doReplyRequest();
            return;
        }

        NCNetworkHandler handler;
        NC_BOOL isDevExist = handler.isCarryNetworkDevice(m_param.deviceType);
        NCLOGD("NCNetworkCheckDisconnectReq::run, isDevExist [%d]", isDevExist);

        if (WIFI_STA == m_param.deviceType) {
            if (isDevExist) {
                NCNetworkManager::Instance()->postRequest(new NCNetworkPluginReleaseAccessReq(m_param));
                NCNetworkManager::Instance()->postRequest(new NCNetworkClearDefaultNetworkReq(m_param.reqType, m_param.deviceType, m_param.remoteFlag));
            }
            else {
                NCLOGD("NCNetworkCheckDisconnectReq::run, Do not disconnect wifi from MEU");
                AccessParameter tempParam = m_param;
                tempParam.result = NetworkErrCode_Fail;
                NCNetworkReplyRequestMapReq replyReq(tempParam);
                replyReq.doReplyRequest();
            }

            return;
        }

        if (RequestType_Disconnect == m_param.reqType) {
            if (!isDevExist) {
                NCLOGD("NCNetworkCheckDisconnectReq::run, isRemote");
                myAccessStack->addRequestMap(RequestMapType_Init, m_param.reqId, m_param);
                myWorkPluginManager->releaseNetworkAccess(m_param.deviceType, m_param.reqId);
                return;
            }
            else {
                NCLOGD("NCNetworkCheckDisconnectReq::run, isLocal");
                if (myWorkThread->checkReqList(m_param)) {
                    NCLOGD("NCNetworkCheckDisconnectReq::run, Check req list success");
                    NCNetworkManager::Instance()->postRequest(new NCNetworkClearDefaultNetworkReq(m_param.reqType, m_param.deviceType, m_param.remoteFlag));
                    return;
                }

                if (myWorkThread->checkCurrentReq(m_param)) {
                    NCLOGD("NCNetworkCheckDisconnectReq::run, Check current req success");
                    NCNetworkManager::Instance()->postRequest(new NCNetworkClearDefaultNetworkReq(m_param.reqType, m_param.deviceType, m_param.remoteFlag));
                    return;
                }

                NCNetworkManager::Instance()->postRequest(new NCNetworkPluginReleaseAccessReq(m_param));
                NCNetworkManager::Instance()->postRequest(new NCNetworkClearDefaultNetworkReq(m_param.reqType, m_param.deviceType, m_param.remoteFlag));
            }
        }
        else if (RequestType_NotifyDisconnectResult == m_param.reqType) {
            NCNetworkManager::Instance()->postRequest(new NCNetworkClearDefaultNetworkReq(m_param.reqType, m_param.deviceType, m_param.remoteFlag));
        }
    }

    NCNetworkRequestCallbackReq::NCNetworkRequestCallbackReq(const RequestPluginResult& info)
    : m_reqId(info.reqId)
    , m_result(info.result)
    , m_deviceType(info.deviceType)
    , m_deviceName(info.deviceName)
    , m_gateway(info.gateway)
    , m_dns1(info.dns1)
    , m_dns2(info.dns2)
    {

    }

    NCNetworkRequestCallbackReq::~NCNetworkRequestCallbackReq()
    {

    }

    VOID
    NCNetworkRequestCallbackReq::run()
    {
        NCLOGD("NCNetworkRequestCallbackReq::run, Start");
        NCLOGD("reqId [%d], result = [%d], deviceType [%s], deviceName [%s], gateway [%s], dns1 [%s], dns2 [%s]", m_reqId, 
            m_result, m_deviceType.getString(), m_deviceName.getString(), m_gateway.getString(), m_dns1.getString(), m_dns2.getString());
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        if (NULL == myAccessStack || NULL == myStatus || NULL == myWorkThread) {
            NCLOGD("NCNetworkRequestCallbackReq::run, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        AccessParameter param;
        if (!myAccessStack->getRequestMap(RequestMapType_Init, m_reqId, param)) {
            NCLOGD("NCNetworkRequestCallbackReq::run, Find reqId in init map failed");
            return;
        }
        else {
            myAccessStack->rmRequestMap(RequestMapType_Init, m_reqId);
        }

        NCNetworkHandler handler;
        NC_BOOL isDevExist = handler.isCarryNetworkDevice(param.deviceType);
        NCString currNetwork = myStatus->currentNetwork();
        NCLOGD("NCNetworkRequestCallbackReq::run, currentNetwork [%s], isDevExist [%d]", currNetwork.getString(), isDevExist);
        
        if (!isDevExist) {
            NCLOGD("NCNetworkRequestCallbackReq::run, Remote");
            if (NC_NOERROR != m_result) {
                if (NCString("") == currNetwork) {
                    myStatus->setDialState(param.deviceType, DialState_Init, DialType_None);
                }

                param.result = m_result;
                NCNetworkReplyRequestMapReq replyReq(param);
                replyReq.doReplyRequest();
            }
            else {
                param.result = NetworkErrCode_Success;
                if (param.deviceType == currNetwork) {
                    NCNetworkReplyRequestMapReq replyReq(param);
                    replyReq.doReplyRequest();
                }
                else {
                    myAccessStack->addRequestMap(RequestMapType_Callback, param.reqId, param);
                }
            }
            return;
        }

        NCLOGD("NCNetworkRequestCallbackReq::run, Local");
        if (!myWorkThread->lockReqState(RequestType_Access)) {
            NCLOGD("NCNetworkRequestCallbackReq::run, Local, Lock reqstate failed");
            return;
        }

        if (NC_NOERROR != m_result) {
            NCLOGD("NCNetworkRequestCallbackReq::run, pppd failed");
            param.result = m_result;
            myAccessStack->addRequestMap(RequestMapType_Callback, param.reqId, param);
        }
        else {
            NCLOGD("NCNetworkRequestCallbackReq::run, pppd success");
            DeviceRouteDnsInfo updateInfo;
            updateInfo.deviceType = m_deviceType;
            updateInfo.deviceName = m_deviceName;
            updateInfo.gateway = m_gateway;
            updateInfo.dns1 = m_dns1;
            updateInfo.dns2 = m_dns2;
            myAccessStack->updateCurrRouteInfo(updateInfo);

            param.result = NetworkErrCode_Success;
            myAccessStack->addRequestMap(RequestMapType_Callback, param.reqId, param);
        }

        NCNetworkCheckRequestCompleteReq requestCompleteReq;
        requestCompleteReq.doCheckRequestComplete();
        myWorkThread->unlockReqState(RequestType_Access);
    }

    NCNetworkReleaseCallbackReq::NCNetworkReleaseCallbackReq(const UINT32 result, const UINT32 seqId)
    : m_result(result)
    , m_seqId(seqId)
    {

    }

    NCNetworkReleaseCallbackReq::~NCNetworkReleaseCallbackReq()
    {

    }

    VOID
    NCNetworkReleaseCallbackReq::run()
    {
        NCLOGD("NCNetworkReleaseCallbackReq::run, Start, result [%d], reqId [%d]", m_result, m_seqId);
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        if (NULL == myAccessStack || NULL == myWorkThread) {
            NCLOGD("NCNetworkReleaseCallbackReq::run, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        AccessParameter param;
        if (!myAccessStack->getRequestMap(RequestMapType_Init, m_seqId, param)) {
            NCLOGD("NCNetworkReleaseCallbackReq::run, Find reqId in init map failed");
            return;
        }
        else {
            myAccessStack->rmRequestMap(RequestMapType_Init, m_seqId);
        }

        if (myWorkThread->lockReqState(RequestType_Release)) {
            NCLOGD("NCNetworkReleaseCallbackReq::run, Local, Lock RequestType_Release success");
            param.result = m_result;
            NCNetworkReplyRequestMapReq replyReq(param);
            replyReq.doReplyRequest();

            NCNetworkReqConfirm reqConfirm;
            reqConfirm.confirm();
            myWorkThread->unlockReqState(RequestType_Release);
            return;
        }

        if (myWorkThread->lockReqState(RequestType_Access)) {
            NCLOGD("NCNetworkRequestCallbackReq::run, Local, Lock RequestType_Access success");
            param.result = m_result;
            myAccessStack->addRequestMap(RequestMapType_Callback, param.reqId, param);

            NCNetworkCheckRequestCompleteReq requestCompleteReq;
            requestCompleteReq.doCheckRequestComplete();
            myWorkThread->unlockReqState(RequestType_Access);
            return;
        }

        NCLOGD("NCNetworkReleaseCallbackReq::run, Lock request failed");
        return;
    }

    NCNetworkDisconnectCallbackReq::NCNetworkDisconnectCallbackReq(const UINT32 result, const NCString& deviceType, const UINT32 seqId)
    : m_result(result)
    , m_deviceType(deviceType)
    , m_seqId(seqId)
    {

    }

    NCNetworkDisconnectCallbackReq::~NCNetworkDisconnectCallbackReq()
    {

    }

    VOID
    NCNetworkDisconnectCallbackReq::run()
    {
        NCLOGD("NCNetworkDisconnectCallbackReq::run, Start, result [%d], deviceType [%s], seqId [%d]", m_result, m_deviceType.getString(), m_seqId);
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
        if (NULL == myAccessStack || NULL == myStatus || NULL == myWorkThread) {
            NCLOGD("NCNetworkDisconnectCallbackReq::run, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        AccessParameter param;
        if (!myAccessStack->getRequestMap(RequestMapType_Init, m_seqId, param)) {
            NCLOGD("NCNetworkDisconnectCallbackReq::run, Find seq ID failed");
            return;
        }
        else {
            myAccessStack->rmRequestMap(RequestMapType_Init, m_seqId);
        }

        NCNetworkHandler handler;
        NC_BOOL isDevExist = handler.isCarryNetworkDevice(m_deviceType);
        NCString currNetwork = myStatus->currentNetwork();
        NCLOGD("NCNetworkDisconnectCallbackReq::run, currentNetwork [%s], isDevExist [%d]", currNetwork.getString(), isDevExist);
        if (!isDevExist) {
            NCLOGD("NCNetworkDisconnectCallbackReq::run, Remote");
            if (m_deviceType != currNetwork) {
                param.result = m_result;
                NCNetworkReplyRequestMapReq replyReq(param);
                replyReq.doReplyRequest();
            }
            else {
                myAccessStack->addRequestMap(RequestMapType_Callback, param.reqId, param);
            }

            return;
        }

        NCLOGD("NCNetworkDisconnectCallbackReq::run, Local");
        if (myWorkThread->lockReqState(RequestType_PluginReleaseAccess)) {
            NCLOGD("NCNetworkDisconnectCallbackReq::run, Lock RequestType_Disconnect success");
            myStatus->setLinkState(m_deviceType, DialState_Disconnected, DialType_None);
            if (currNetwork != m_deviceType) {
                param.result = NetworkErrCode_Success;
                NCNetworkReplyRequestMapReq replyReq(param);
                replyReq.doReplyRequest();
            }
            else {
                myAccessStack->addRequestMap(RequestMapType_Callback, param.reqId, param);
            }

            NCNetworkReqConfirm reqConfirm;
            reqConfirm.confirm();
            myWorkThread->unlockReqState(RequestType_PluginReleaseAccess);
            return;
        }

        if (myWorkThread->lockReqState(RequestType_Access)) {
            NCLOGD("NCNetworkDisconnectCallbackReq::run, Lock RequestType_Access success");
            param.result = m_result;
            myAccessStack->addRequestMap(RequestMapType_Callback, param.reqId, param);

            NCNetworkCheckRequestCompleteReq requestCompleteReq;
            requestCompleteReq.doCheckRequestComplete();
            myWorkThread->unlockReqState(RequestType_Access);
            return;
        }

        NCLOGD("NCNetworkDisconnectCallbackReq::run, Lock request failed");
        return;
    }

    NCNetworkCheckRequestCompleteReq::NCNetworkCheckRequestCompleteReq()
    {

    }

    NCNetworkCheckRequestCompleteReq::~NCNetworkCheckRequestCompleteReq()
    {

    }

    VOID
    NCNetworkCheckRequestCompleteReq::run()
    {
        NCLOGD("NCNetworkCheckRequestCompleteReq::run, Start");
        doCheckRequestComplete();
    }

    VOID
    NCNetworkCheckRequestCompleteReq::doCheckRequestComplete()
    {
        NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, Start");
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
        if (NULL == myAccessStack || NULL == myStatus) {
            NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        if (!myAccessStack->isRequestMapEmpty(RequestMapType_Init)) {
            NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, Init map is not empty");
            return;
        }

        NCNetworkReqConfirm reqConfirm;
        NCString deviceType = reqConfirm.getDeviceType(RequestType_Access);

        NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, Init map is empty");
        reqConfirm.setReqState(RequestType_Access, REQ_DIALEND);

        NCString currNetwork = myStatus->currentNetwork();
        NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, Current network [%s]", currNetwork.getString());

        if (myAccessStack->checkRequstType(RequestMapType_Callback, RequestType_Disconnect)) {
            NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, CallbackMap has disconnect");
            if (deviceType == currNetwork) {
                myStatus->setLinkState(deviceType, DialState_Disconnected, DialType_None);
            }
            else {
                myStatus->setLinkState(deviceType, DialState_Init, DialType_None);
                myStatus->setDialState(deviceType, DialState_Init, DialType_None);
                myStatus->setLinkState(deviceType, DialState_Disconnected, DialType_None);
            }

            NCNetworkPopAllRequestMapReq popAllCallbackReq(RequestMapType_Callback, PopResultFlag_Interrupt);
            popAllCallbackReq.doPopAllRequest();
            reqConfirm.confirm();
            return;
        }
        else {
            NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, CallbackMap has no disconnect");
            if (deviceType == currNetwork) {
                NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, Current network has been configured");
                NCNetworkPopAllRequestMapReq popAllCallbackReq(RequestMapType_Callback, PopResultFlag_Init);
                popAllCallbackReq.doPopAllRequest();
                reqConfirm.confirm();
                return;
            }
            else {
                NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, Current network has not been configured");
                if (myAccessStack->checkRequestSuccess(RequestMapType_Callback, RequestType_Access)) {
                    NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, CallbackMap has success request");
                    myStatus->setLinkState(deviceType, DialState_Connected, DialType_None);

                    DeviceRouteDnsInfo configInfo; 
                    myAccessStack->getCurrRouteInfo(configInfo);
                    NCNetworkManager::Instance()->insertRequest(new NCNetworkSetDefaultNetworkReq(RequestType_Access, configInfo));

                    if (DCM == deviceType) {
#if defined NCNETWORK_TARGET_PRODUCT_DCU
                        NCNetworkManager::Instance()->insertRequest(new NCNetworkUpdateDhcpdConfReq(configInfo.dns1, configInfo.dns2));
#endif
#if defined NCNETWORK_TARGET_PRODUCT_MEU
                        myStatus->setDcmDnsInfo(configInfo.dns1, configInfo.dns2);
#endif
                    }

                    reqConfirm.confirm();
                    return;
                }
                else {
                    NCLOGD("NCNetworkCheckRequestCompleteReq::doCheckRequestComplete, CallbackMap has no success request");
                    myStatus->setLinkState(deviceType, DialState_Init, DialType_None);
                    myStatus->setDialState(deviceType, DialState_Init, DialType_None);

                    NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Init);
                    popAllReq.doPopAllRequest();
                    reqConfirm.confirm();
                    return;
                }
            }

        }
    }

    NCNetworkAddRequestMapReq::NCNetworkAddRequestMapReq(const UINT32 mapType, const AccessParameter& param)
    : m_mapType(mapType)
    , m_param(param)
    {

    }

    NCNetworkAddRequestMapReq::~NCNetworkAddRequestMapReq()
    {

    }

    VOID
    NCNetworkAddRequestMapReq::run()
    {
        NCLOGD("NCNetworkAddRequestMapReq::run, Start, mapType [%d], seqId [%d], resourceId [%s], deviceType [%s]", 
            m_mapType, m_param.reqId, m_param.resourceId.getString(), m_param.deviceType.getString());
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        if (NULL == myAccessStack) {
            NCLOGD("NCNetworkAddRequestMapReq::run, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        myAccessStack->addRequestMap(m_mapType, m_param.reqId, m_param);
    }


    NCNetworkPopRequestMapReq::NCNetworkPopRequestMapReq(const UINT32 mapType, const UINT32 reqType, const NCString& deviceType, const UINT32 result)
    : m_mapType(mapType)
    , m_reqType(reqType)
    , m_deviceType(deviceType)
    , m_result(result)
    {

    }

    NCNetworkPopRequestMapReq::~NCNetworkPopRequestMapReq()
    {

    }

    VOID
    NCNetworkPopRequestMapReq::run()
    {
        NCLOGD("NCNetworkPopRequestMapReq::run, Start");
        doPopRequest();
    }

    VOID
    NCNetworkPopRequestMapReq::doPopRequest()
    {
        NCLOGD("NCNetworkPopRequestMapReq::doPopRequest, Start");
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        if (NULL == myAccessStack) {
            NCLOGD("NCNetworkPopRequestMapReq::doPopRequest, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        AccessParameter tempParam;
        while (myAccessStack->popRequestMap(m_mapType, m_reqType, m_deviceType, tempParam)) {
            NCLOGD("NCNetworkPopRequestMapReq::doPopRequest, Pop request, reqId[%d], resourceId [%s], reqType [%d]", 
                tempParam.reqId, tempParam.resourceId.getString(), tempParam.reqType);
            tempParam.result = m_result;
            myAccessStack->rmRequestMap(m_mapType, tempParam.reqId);
            NCNetworkReplyRequestMapReq replyReq(tempParam);
            replyReq.doReplyRequest();
        }
    }

    NCNetworkPopAllRequestMapReq::NCNetworkPopAllRequestMapReq(const UINT32 mapType, const UINT32 popRequestFlag)
    : m_mapType(mapType)
    , m_popRequestFlag(popRequestFlag)
    {

    }

    NCNetworkPopAllRequestMapReq::~NCNetworkPopAllRequestMapReq()
    {

    }

    VOID
    NCNetworkPopAllRequestMapReq::run()
    {
        NCLOGD("NCNetworkPopAllRequestMapReq::run, Start");
        doPopAllRequest();
    }

    VOID
    NCNetworkPopAllRequestMapReq::doPopAllRequest()
    {
        NCLOGD("NCNetworkPopAllRequestMapReq::doPopAllRequest, Start, mapType [%d], flag [%d]", m_mapType, m_popRequestFlag);
        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        if (NULL == myAccessStack) {
            NCLOGD("NCNetworkPopAllRequestMapReq::doPopAllRequest, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        AccessParameter tempParam;
        while (myAccessStack->popRequestMap(m_mapType, tempParam)) {
            NCLOGD("NCNetworkPopAllRequestMapReq::doPopAllRequest, Pop request, reqId[%d], resourceId [%s], reqType [%d]", 
                tempParam.reqId, tempParam.resourceId.getString(), tempParam.reqType);
            myAccessStack->rmRequestMap(m_mapType, tempParam.reqId);
            switch (tempParam.reqType) {
            case RequestType_Access:
            {
                NCLOGD("NCNetworkPopAllRequestMapReq::doPopAllRequest, RequestType_Access");
                if (NetworkErrCode_Success == tempParam.result) {
                    if (PopResultFlag_Interrupt == m_popRequestFlag) {
                        tempParam.result = NetworkErrCode_InterruptByDisconnect;
                        NCNetworkReplyRequestMapReq replyReq(tempParam);
                        replyReq.doReplyRequest();
                        break;
                    }
                    else if (PopResultFlag_Timeout == m_popRequestFlag) {
                        tempParam.result = NetworkErrCode_Timeout;
                        NCNetworkReplyRequestMapReq replyReq(tempParam);
                        replyReq.doReplyRequest();
                        break;
                    }
                    else if (PopResultFlag_Failed == m_popRequestFlag) {
                        tempParam.result = NetworkErrCode_Fail;
                        NCNetworkReplyRequestMapReq replyReq(tempParam);
                        replyReq.doReplyRequest();
                        break;
                    }
                    else {
                        NCNetworkReplyRequestMapReq replyReq(tempParam);
                        replyReq.doReplyRequest();
                        break;
                    }
                }
                else {
                    NCNetworkReplyRequestMapReq replyReq(tempParam);
                    replyReq.doReplyRequest();
                    break;
                }
            }
            case RequestType_Release:
            {
                NCLOGD("NCNetworkPopAllRequestMapReq::doPopAllRequest, RequestType_Release");
                NCNetworkReplyRequestMapReq replyReq(tempParam);
                replyReq.doReplyRequest();
                break;
            }
            case RequestType_Disconnect:
            {
                NCLOGD("NCNetworkPopAllRequestMapReq::doPopAllRequest, RequestType_Disconnect");
                NCNetworkReplyRequestMapReq replyReq(tempParam);
                replyReq.doReplyRequest();
                break;
            }
            default:
            {
                NCLOGD("NCNetworkPopAllRequestMapReq::doPopAllRequest, Default");
                return;
            }
            }
        }
    }

    NCNetworkReplyRequestMapReq::NCNetworkReplyRequestMapReq(const AccessParameter& param)
    : m_param(param)
    {

    }

    NCNetworkReplyRequestMapReq::~NCNetworkReplyRequestMapReq()
    {

    }

    VOID
    NCNetworkReplyRequestMapReq::run()
    {
        NCLOGD("NCNetworkReplyRequestMapReq::run, Start");
        doReplyRequest();
    }

    VOID
    NCNetworkReplyRequestMapReq::doReplyRequest()
    {
        NCLOGD("NCNetworkReplyRequestMapReq::doReplyRequest, Start");
        NCNetworkPluginManager* myPluginManager = NCNetworkManager::Instance()->getNetworkPluginManager();
        if (NULL == myPluginManager) {
            NCLOGD("NCNetworkReplyRequestMapReq::doReplyRequest, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;

        }

        NCNetworkNotify notify;
        UINT32 reqType = m_param.reqType;
        switch (reqType) {
        case RequestType_Access:
        {
            NCLOGD("NCNetworkReplyRequestMapReq::doReplyRequest, RequestType_Access");
            if (m_param.remoteFlag) {
                myPluginManager->notifyRemoteRequestCallback(m_param.deviceType, m_param.result);
            }
            else {
                if (m_param.reqIdFlag) {
                    notify.notifyRequestAccessResult(m_param.resourceId, m_param.result, m_param.wifiApFlag, m_param.reqId);
                }
                else {
                    notify.notifyRequestAccessResult(m_param.resourceId, m_param.result, m_param.wifiApFlag, 0);
                }
            }

            break;
        }
        case RequestType_Release:
        {
            NCLOGD("NCNetworkReplyRequestMapReq::doReplyRequest, RequestType_Release");
            if (m_param.reqIdFlag) {
                notify.notifyReleaseAccessResult(m_param.resourceId, m_param.result, m_param.reqId);
            }
            else {
                notify.notifyReleaseAccessResult(m_param.resourceId, m_param.result, 0);
            }

            break;
        }
        case RequestType_Disconnect:
        {
            NCLOGD("NCNetworkReplyRequestMapReq::doReplyRequest, RequestType_Disconnect");
            if (m_param.remoteFlag) {
                myPluginManager->notifyRemoteDisconnectCallback(m_param.deviceType);
            }
            else {
                if (m_param.reqIdFlag) {
                    notify.notifyDisconnectResult(m_param.deviceType, m_param.result, m_param.reqId);
                }
                else {
                    notify.notifyDisconnectResult(m_param.deviceType, m_param.result, 0);
                }
                
            }

            break;
        }
        default:
        {
            NCLOGD("NCNetworkReplyRequestMapReq::doReplyRequest, reqType error, [%d]", reqType);
            break;
        }
        }
    }

    NCNetworkCheckTimeoutRequestMapReq::NCNetworkCheckTimeoutRequestMapReq(const UINT32 mapType)
    : m_mapType(mapType)
    {

    }

    NCNetworkCheckTimeoutRequestMapReq::~NCNetworkCheckTimeoutRequestMapReq()
    {

    }

    VOID
    NCNetworkCheckTimeoutRequestMapReq::run()
    {
        NCLOGD("NCNetworkCheckTimeoutRequestMapReq::run, Start, mapType [%d]", m_mapType);
        if (RequestMapType_Init != m_mapType) {
            NCLOGD("NCNetworkCheckTimeoutRequestMapReq::run, Parameters error");
            return;
        }

        NCNetworkAccessStack* myAccessStack = NCNetworkManager::Instance()->getAccessStack();
        if (NULL == myAccessStack) {
            NCLOGD("NCNetworkCheckTimeoutRequestMapReq::run, Get pointer failed");
            NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
            return;
        }

        NC_BOOL flag = NC_FALSE;
        NCLOGD("NCNetworkCheckTimeoutRequestMapReq::run, flag [%d]", flag);

        AccessParameter tempParam;
        while (myAccessStack->checkRequestTimeout(m_mapType, tempParam)) {
            NCLOGD("NCNetworkCheckTimeoutRequestMapReq::run, Missed callback for seqId[%d], reqType [%d], resourceId [%s], deviceType [%s]", 
                tempParam.reqId, tempParam.reqType, tempParam.resourceId.getString(), tempParam.deviceType.getString());
            flag = NC_TRUE;
            myAccessStack->rmRequestMap(m_mapType, tempParam.reqId);

            tempParam.result = NetworkErrCode_Timeout;
            NCNetworkReplyRequestMapReq replyReq(tempParam);
            replyReq.doReplyRequest();
        }


        NCNetworkHandler handler;
        if (flag) {
            NCErrorLog(NetworkErrCode_Timeout, handler.deviceTypeToNumber(tempParam.deviceType));
        }
    }

} /* namespace nutshell */
/* EOF */
