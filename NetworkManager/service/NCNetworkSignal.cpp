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
#include <system/NCEnvironment.h>
#include <NDNetworkManager.pb.h>
#include "networkmanager/NCNetworkDef.h"
#include "NCNetworkManagerDef.h"
#include "NCNetworkManager.h"
#include "NCNetworkStatus.h"
#include "NCNetworkNotify.h"
#include "NCNetworkSignal.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{
    NCNetworkSignal::NCNetworkSignal()
    : m_wifiSignal(NC_FALSE)
    , m_dcmSignal(NC_FALSE)
    , m_btphoneSignal(NC_FALSE)
    {

    }

    NCNetworkSignal::~NCNetworkSignal()
    {

    }


    NC_BOOL
    NCNetworkSignal::signal(const NCString& deviceType)
    {
        NCLOGD("NCNetworkSignal::signal - Start, deviceType = [%s]", deviceType.getString());
        NCEnvironment env;
        if (WIFI_STA == deviceType) {
            NCString uriString = WIFI_STRENGTH_KEY;
            NCString value;
            if (env.get(uriString, value)) {
                return checkSignalStatus(deviceType, value);
            }
            else {
                return NC_FALSE;
            }
        }
        else if (DCM == deviceType) {
            NCString uriString = DCM_STRTHGTH_KEY;
            NCString value;
            if (env.get(uriString, value)) {
                return checkSignalStatus(deviceType, value);
            }
            else {
                return NC_FALSE;
            }
        }
        else if (BT_PHONE == deviceType) {
            NCString uriString = BT_STRTHGTH_KEY;
            NCString value;
            if (env.get(uriString, value)) {
                return checkSignalStatus(deviceType, value);
            }
            else {
                return NC_FALSE;
            }
        }
        else {
            NCLOGD("NCNetworkSignal::signal - DeviceType [%s] is not found");
            return NC_FALSE;
        }
    }

    VOID
    NCNetworkSignal::setSignalStatus(const NCString& deviceType, const NCString& value)
    {
        NCLOGD("NCNetworkSignal::setSignalStatus - deviceType [%s], value [%s]", deviceType.getString(), value.getString());
        NC_BOOL status = checkSignalStatus(deviceType, value);
        NCNetworkNotify notify;
        if (WIFI_STA == deviceType) {
            if (!m_wifiSignal && status) {
                m_wifiSignal = status;
                notify.broadcastNetworkStatus(NCNETWORK_EVENT_HAVESIGNAL, deviceType);
            }
            else if (m_wifiSignal && !status) {
                m_wifiSignal = status;
                notify.broadcastNetworkStatus(NCNETWORK_EVENT_NOSIGNAL, deviceType);
            }
            else {
            }
        }
        else if (DCM == deviceType) {
            if (!m_dcmSignal && status) {
                m_dcmSignal = status;
                notify.broadcastNetworkStatus(NCNETWORK_EVENT_HAVESIGNAL, deviceType);
            }
            else if (m_dcmSignal && !status) {
                m_dcmSignal = status;
                notify.broadcastNetworkStatus(NCNETWORK_EVENT_NOSIGNAL, deviceType);
            }
            else {
            }
        }
        else if (BT_PHONE == deviceType) {
            if (!m_btphoneSignal && status) {
                m_btphoneSignal = status;
                notify.broadcastNetworkStatus(NCNETWORK_EVENT_HAVESIGNAL, deviceType);
            }
            else if (m_btphoneSignal && !status) {
                m_btphoneSignal = status;
                notify.broadcastNetworkStatus(NCNETWORK_EVENT_NOSIGNAL, deviceType);
            }
            else {
            }
        }
        else {
            NCLOGD("NCNetworkSignal::setSignalStatus deviceType [%s] is not defined", deviceType.getString());
        }
    }

    VOID
    NCNetworkSignal::setDcmDevStatus(const NCString& value)
    {
        NCLOGD("NCNetworkSignal::setDcmDevStatus - Start, value = [%s]", value.getString());
        if (NCString("inited") == value) {
            NCNetworkManager::Instance()->getNetworkStatus()->setStatus(DCM, NC_TRUE);
        }
        else if (NCString("uninited") == value) {
            NCNetworkManager::Instance()->getNetworkStatus()->setStatus(DCM, NC_FALSE);
        }
        else {
        }
    }

    VOID
    NCNetworkSignal::setBtDunDevStatus(const NCString& value)
    {
        NCLOGD("NCNetworkSignal::setBtDunDevStatus - Start, value = [%s]", value.getString());
        if (NCString("connected") == value) {
            NCNetworkManager::Instance()->getNetworkStatus()->setStatus(BT_DUN, NC_TRUE);
        }
        else if (NCString("disconnected") == value) {
            NCNetworkManager::Instance()->getNetworkStatus()->setStatus(BT_DUN, NC_FALSE);
        }
        else {
        }
    }

    VOID
    NCNetworkSignal::updateNetworkStatus(const NCString& deviceType, const NCString& value)
    {
        NCLOGD("NCNetworkSignal::updateNetworkStatus, deviceType [%s], value[%s]", deviceType.getString(), value.getString());
        NCEnvironment env;
        NCString statusKey = INTERNET_STATUS_KEY;
        NCString typeKey = INTERNET_TYPE_KEY;
        NCString dcmKey = DCM_STATUS_KEY;

        if (NCString("disconnected") == value) {
            env.update(statusKey, value);
            NCString typeValue = "none";
            env.update(typeKey, typeValue);
            if (DCM == deviceType) {
                env.update(dcmKey, "disconnected");
            }
        }
        else if (NCString("connected") == value) {
            env.update(statusKey, value);
            if (DCM == deviceType) {
                env.update(typeKey, "dcm");
                env.update(dcmKey, "connected");
            }
            else if (BT_DUN == deviceType) {
                env.update(typeKey, "btdun");
            }
            else if (WIFI_STA == deviceType) {
                env.update(typeKey, "wifi");
            }
            else {
                NCLOGD("NCNetworkSignal::updateNetworkStatus, deviceType is not defined");
            }
        }
        else {
            NCLOGD("NCNetworkSignal::updateNetworkStatus, status value is not defined");
        }
    }

    NC_BOOL
    NCNetworkSignal::checkSignalStatus(const NCString& deviceType, const NCString& value)
    {
        NCLOGD("NCNetworkSignal::checkSignalStatus - Start, deviceType = [%s], value = [%s]", deviceType.getString(), value.getString());
        if (WIFI_STA == deviceType) {
            if ("0" == value) {
                NCLOGD("NCNetworkSignal::checkSignalStatus - WIFI has no signal");
                return NC_FALSE;
            }
            else if ("1" == value || "2" == value || "3" == value || "4" == value || "5" == value) {
                NCLOGD("NCNetworkSignal::checkSignalStatus - WIFI has signal, signal strength = [%s]", value.getString());
                return NC_TRUE;
            }
            else {
                NCLOGD("NCNetworkSignal::checkSignalStatus - WIFI value [%s] is unknown", value.getString());
                return NC_FALSE;
            }
        }
        else if (DCM == deviceType) {
            if ("0" == value) {
                NCLOGD("NCNetworkSignal::checkSignalStatus - DCM has no signal");
                return NC_FALSE;
            }
            else if ("1" == value || "2" == value || "3" == value || "4" == value || "5" == value) {
                NCLOGD("NCNetworkSignal::checkSignalStatus - DCM has signal, signal strength = [%s]", value.getString());
                return NC_TRUE;
            }
            else {
                NCLOGD("NCNetworkSignal::checkSignalStatus - DCM value [%s] is unknown", value.getString());
                return NC_FALSE;
            }
        }
        else if (BT_PHONE == deviceType) {
            if ("disconnected" == value) {
                NCLOGD("NCNetworkSignal::checkSignalStatus - BT has no signal");
                return NC_FALSE;
            }
            else if ("bad" == value || "good" == value) {
                NCLOGD("NCNetworkSignal::checkSignalStatus - BT has signal, signal strength = [%s]", value.getString());
                return NC_TRUE;
            }
            else {
                NCLOGD("NCNetworkSignal::checkSignalStatus - BT value [%s] is unknown", value.getString());
                return NC_FALSE;
            }
            
        }
        else {
            NCLOGD("NCNetworkSignal::checkSignalStatus - DeviceType [%s] is not found");
            return NC_FALSE;
        }
    }

} /* namespace nutshell */
/* EOF */
