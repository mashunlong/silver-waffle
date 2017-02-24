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
#include <ncore/NCAutoSync.h>
#include <ncore/NCTimeDefine.h>
#include "NCNetworkManager.h"
#include "NCNetworkHandler.h"
#include "NCNetworkAccessStack.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{

    NCNetworkAccessStack::NCNetworkAccessStack()
    : m_timerFlag(NC_FALSE)
    {

    }

    NCNetworkAccessStack::~NCNetworkAccessStack()
    {

    }

    VOID
    NCNetworkAccessStack::addRequestMap(const UINT32 mapType, const UINT32 seqId, const AccessParameter& param)
    {
        NCLOGD("NCNetworkAccessStack::addRequestMap, Start, mapType [%d], seqId [%d]", mapType, seqId);
        if (RequestMapType_Init == mapType) {
            NCNetworkHandler handler;
            if (m_initMap.empty() && (!handler.isCarryNetworkDevice(param.deviceType))) {
                NCLOGD("NCNetworkAccessStack::addRequestMap, Init map is empty, start timer");
                NCNetworkManager::Instance()->startTimer(TimerType_RequestMap);
                m_timerFlag = NC_TRUE;
            }

            std::map<UINT32, AccessParameter>::iterator iter = m_initMap.find(seqId);
            if (iter != m_initMap.end()) {
                m_initMap.erase(iter);
            }

            AccessParameter tempParam = param;
            tempParam.tc = GetTickCount();
            m_initMap.insert(std::map<UINT32, AccessParameter>::value_type(seqId, tempParam));
        }
        else if (RequestMapType_Callback == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_callbackMap.find(seqId);
            if (iter != m_callbackMap.end()) {
                m_callbackMap.erase(iter);
            }
            
            m_callbackMap.insert(std::map<UINT32, AccessParameter>::value_type(seqId, param));
        }
        else {
            NCLOGD("NCNetworkAccessStack::addRequestMap, Map type error");
        }
    }

    NC_BOOL
    NCNetworkAccessStack::getRequestMap(const UINT32 mapType, const UINT32 seqId, AccessParameter& param)
    {
        NCLOGD("NCNetworkAccessStack::getRequestMap, mapType [%d], seqId [%d]", mapType, seqId);
        if (RequestMapType_Init == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_initMap.find(seqId);
            if (iter != m_initMap.end()) {
                param = iter->second;
                return NC_TRUE;
            }
        }
        else if (RequestMapType_Callback == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_callbackMap.find(seqId);
            if (iter != m_callbackMap.end()) {
                param = iter->second;
                return NC_TRUE;
            }
        }

        return NC_FALSE;
    }

    VOID
    NCNetworkAccessStack::rmRequestMap(const UINT32 mapType, const UINT32 seqId)
    {
        NCLOGD("NCNetworkAccessStack::rmRequestMap, Start, mapType [%d], seqId [%d]", mapType, seqId);
        if (RequestMapType_Init == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_initMap.find(seqId);
            if (iter != m_initMap.end()) {
                m_initMap.erase(iter);
            }

            NCNetworkHandler handler;
            if (m_initMap.empty() && (m_timerFlag)) {
                NCLOGD("NCNetworkAccessStack::rmRequestMap, Init map is empty, stop timer");
                NCNetworkManager::Instance()->stopTimer(TimerType_RequestMap);
                m_timerFlag = NC_FALSE;
            }
        }
        else if (RequestMapType_Callback == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_callbackMap.find(seqId);
            if (iter != m_callbackMap.end()) {
                m_callbackMap.erase(iter);
            }
        }
    }

    NC_BOOL
    NCNetworkAccessStack::isRequestMapEmpty(const UINT32 mapType)
    {
        NCLOGD("NCNetworkAccessStack::isRequestMapEmpty, Start, mapType [%d]", mapType);
        if (RequestMapType_Init == mapType) {
            return m_initMap.empty();
        }
        else if (RequestMapType_Callback == mapType) {
            return m_callbackMap.empty();
        }
        else {
            NCLOGD("NCNetworkAccessStack::isRequestMapEmpty, Parameters error");
            return NC_FALSE;
        }
    }

    NC_BOOL
    NCNetworkAccessStack::popRequestMap(const UINT32 mapType, AccessParameter& param)
    {
        NCLOGD("NCNetworkAccessStack::popRequestMap, Start, mapType [%d]", mapType);
        if (RequestMapType_Init == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_initMap.begin();
            if (iter != m_initMap.end()) {
                param = iter->second;
                return NC_TRUE;
            }
        }
        else if (RequestMapType_Callback == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_callbackMap.begin();
            if (iter != m_callbackMap.end()) {
                param = iter->second;
                return NC_TRUE;
            }
        }

        return NC_FALSE;
    }

    NC_BOOL
    NCNetworkAccessStack::popRequestMap(const UINT32 mapType, const UINT32 reqType, const NCString& deviceType, AccessParameter& param)
    {
        NCLOGD("NCNetworkAccessStack::popRequestMap, Start, mapType [%d], reqType [%d], deviceType [%s]", 
            mapType, reqType, deviceType.getString());
        if (RequestMapType_Init == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_initMap.begin();
            while (iter != m_initMap.end()) {
                param = iter->second;
                if (reqType == param.reqType && deviceType == param.deviceType) {
                    return NC_TRUE;
                }
                ++iter;
            }
        }
        else if (RequestMapType_Callback == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_callbackMap.begin();
            while (iter != m_callbackMap.end()) {
                param = iter->second;
                if (reqType == param.reqType && deviceType == param.deviceType) {
                    return NC_TRUE;
                }
                ++iter;
            }
        }

        return NC_FALSE;
    }

    NC_BOOL
    NCNetworkAccessStack::checkRequstType(const UINT32 mapType, const UINT32 reqType)
    {
        NCLOGD("NCNetworkAccessStack::checkRequstType, Start, mapType [%d], reqType [%d]", mapType, reqType);
        if (RequestMapType_Init == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_initMap.begin();
            AccessParameter param;
            while (iter != m_initMap.end()) {
                param = iter->second;
                if (reqType == param.reqType) {
                    return NC_TRUE;
                }
                ++iter;
            }
        }
        else if (RequestMapType_Callback == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_callbackMap.begin();
            AccessParameter param;
            while (iter != m_callbackMap.end()) {
                param = iter->second;
                if (reqType == param.reqType) {
                    return NC_TRUE;
                }
                ++iter;
            }
        }

        return NC_FALSE;
    }

    NC_BOOL
    NCNetworkAccessStack::checkRequestSuccess(const UINT32 mapType, const UINT32 reqType)
    {
        NCLOGD("NCNetworkAccessStack::checkRequestSuccess, Start, mapType [%d], reqType [%d]", mapType, reqType);
        if (RequestMapType_Init == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_initMap.begin();
            AccessParameter param;
            while (iter != m_initMap.end()) {
                param = iter->second;
                if (reqType == param.reqType && param.result == NetworkErrCode_Success) {
                    return NC_TRUE;
                }
                ++iter;
            }
        }
        else if (RequestMapType_Callback == mapType) {
            std::map<UINT32, AccessParameter>::iterator iter = m_callbackMap.begin();
            AccessParameter param;
            while (iter != m_callbackMap.end()) {
                param = iter->second;
                if (reqType == param.reqType && param.result == NetworkErrCode_Success) {
                    return NC_TRUE;
                }
                ++iter;
            }
        }

        return NC_FALSE;
    }

    NC_BOOL
    NCNetworkAccessStack::checkRequestTimeout(const UINT32 mapType, AccessParameter& param)
    {
        NCLOGD("NCNetworkAccessStack::checkRequestTimeout, Start, mapType [%d]", mapType);
        if (RequestMapType_Init != mapType) {
            NCLOGD("NCNetworkAccessStack::checkRequestTimeout, Parameters error");
            return NC_FALSE;
        }

        DWORD tc = GetTickCount();
        NCLOGD("NCNetworkAccessStack::checkRequestTimeout, Current tick count [%d]", tc);

        std::map<UINT32, AccessParameter>::iterator iter = m_initMap.begin();
        while (iter != m_initMap.end()) {
            param = iter->second;
            DWORD timeLag = tc - param.tc;
            if (timeLag > 600 * 1000) {
                return NC_TRUE;
            }

            ++iter;
        }

        return NC_FALSE;
    }

    VOID
    NCNetworkAccessStack::updateCurrRouteInfo(const DeviceRouteDnsInfo& info)
    {
        NCLOGD("NCNetworkAccessStack::updateCurrRouteInfo, Start");
        NCAutoSync lock(m_syncCurrRouteInfo);
        m_currRouteInfo.deviceType = info.deviceType;
        m_currRouteInfo.deviceName = info.deviceName;
        m_currRouteInfo.gateway = info.gateway;
        m_currRouteInfo.dns1 = info.dns1;
        m_currRouteInfo.dns2 = info.dns2;
    }

    VOID
    NCNetworkAccessStack::getCurrRouteInfo(DeviceRouteDnsInfo& info)
    {
        NCLOGD("NCNetworkAccessStack::getCurrRouteInfo, Start");
        NCAutoSync lock(m_syncCurrRouteInfo);
        info.deviceType = m_currRouteInfo.deviceType;
        info.deviceName = m_currRouteInfo.deviceName;
        info.gateway = m_currRouteInfo.gateway;
        info.dns1 = m_currRouteInfo.dns1;
        info.dns2 = m_currRouteInfo.dns2;
    }

    VOID
    NCNetworkAccessStack::addResourceId(const NCString& resourceId, const NCString& deviceType)
    {
        NCLOGD("NCNetworkAccessStack::addResourceId, resourceId [%s], deviceType [%s]", resourceId.getString(), deviceType.getString());
        NCAutoSync lock(m_syncResourceIdMap);
        std::map<NCString, NCString>::iterator iter = m_resourceIdMap.find(resourceId);
        if (iter != m_resourceIdMap.end()) {
            iter->second = deviceType;
        }
        else {
            m_resourceIdMap.insert(std::map<NCString, NCString>::value_type(resourceId, deviceType));
        }
    }

    VOID
    NCNetworkAccessStack::getResourceId(const NCString& resourceId, NCString& deviceType)
    {
        NCLOGD("NCNetworkAccessStack::getResourceId, resourceId [%s]", resourceId.getString());
        NCAutoSync lock(m_syncResourceIdMap);
        std::map<NCString, NCString>::iterator iter = m_resourceIdMap.find(resourceId);
        if (iter != m_resourceIdMap.end()) {
            deviceType = iter->second;
        }
    }

    NC_BOOL
    NCNetworkAccessStack::findResourceId(const NCString& resourceId)
    {
        NCLOGD("NCNetworkAccessStack::findResourceId, resourceId [%s]", resourceId.getString());
        NCAutoSync lock(m_syncResourceIdMap);
        std::map<NCString, NCString>::iterator iter = m_resourceIdMap.find(resourceId);
        if (iter != m_resourceIdMap.end()) {
            return NC_TRUE;
        }

        return NC_FALSE;
    }

} /* namespace nutshell */
/* EOF */
