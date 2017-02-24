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
#include "NCNetworkManagerDef.h"
#include "NCNetworkSignal.h"
#include "NCNetworkManager.h"
#include "NCNetworkSignalListener.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{

    NCNetworkSignalListener::NCNetworkSignalListener(const NCString& name)
    : NCContentListener(name)
    , m_signal(new NCNetworkSignal())
    {

    }

    NCNetworkSignalListener::~NCNetworkSignalListener()
    {
        if (m_signal != NULL) {
            delete m_signal;
            m_signal = NULL;
        }
    }

    VOID
    NCNetworkSignalListener::onNotifyUpdate(NCUri &uri, NCVariant &variant)
    {
        NCString ncuri = uri.toString();
        std::string strValue = variant.toString();
        NCString uriPrefix = NCString(SYSTEM_INFO_URI) + NCString("?");

        if (uriPrefix + NCString(WIFI_STRENGTH_KEY) == ncuri) {
            if (NULL != m_signal) {
                m_signal->setSignalStatus(WIFI_STA, NCString(strValue.c_str()));
            }
        }
        else if (uriPrefix + NCString(DCM_STRTHGTH_KEY) == ncuri) {
            if (NULL != m_signal) {
                m_signal->setSignalStatus(DCM, strValue.c_str());
            }
        }
        else if (uriPrefix + NCString(BT_STRTHGTH_KEY) == ncuri) {
            if (NULL != m_signal) {
                m_signal->setSignalStatus(BT_PHONE, strValue.c_str());
            }
        }
        else if (uriPrefix + NCString(DCM_DEV_Key) == ncuri) {
            if (NULL != m_signal) {
                m_signal->setDcmDevStatus(strValue.c_str());
            }
        }
        else if (uriPrefix + NCString(BTDUN_DEV_KEY) == ncuri) {
            if (NULL != m_signal) {
                m_signal->setBtDunDevStatus(strValue.c_str());
            }
        }
        else {
            
        }
    }

} /* namespace nutshell */
/* EOF */