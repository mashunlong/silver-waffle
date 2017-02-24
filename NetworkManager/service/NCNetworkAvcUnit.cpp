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
#include <evipservice/NEAvcUnit.h>
#include <ncore/NCAutoSync.h>
#include <NDAvcNetworkManager_generated.h>
#include "NCNetworkStatus.h"
#include "NCNetworkDeviceType.h"
#include "NCNetworkManagerDef.h"
#include "NCNetworkHandler.h"
#include "NCNetworkManager.h"
#include "NCNetworkAvcUnit.h"

namespace nutshell
{
#define NC_LOG_TAG "NCNetworkManager"

    NCNetworkAvcUnit::NCNetworkAvcUnit()
    : m_avcUnit(NULL)
    {
        m_avcUnit = new NEAvcUnit(this);
        m_flag = NC_FALSE;
    }

    NCNetworkAvcUnit::~NCNetworkAvcUnit()
    {
        if (NULL != m_avcUnit) {
            delete m_avcUnit;
            m_avcUnit = NULL;
        }
    }

    VOID
    NCNetworkAvcUnit::stop()
    {
        NCLOGD("NCNetworkAvcUnit::stop - Start");
        // stop avc unit
        if (NULL != m_avcUnit) {
            m_avcUnit->stop(NEAvcUnit::Channel_Ext);
        }
        NCLOGD("NCNetworkAvcUnit::stop - End");
    }

    VOID
    NCNetworkAvcUnit::onBindExtChannel()
    {
        NCLOGD("NCNetworkAvcUnit::onBindExtChannel, Start");
        NCNetworkManager::Instance()->startTimer(TimerType_DcuMeuSync);
    }

    VOID
    NCNetworkAvcUnit::onUnbindExtChannel()
    {
        NCLOGD("NCNetworkAvcUnit::onUnbindExtChannel - Start");
        NCNetworkManager::Instance()->stopTimer(TimerType_DcuMeuSync);
    }

        
    VOID
    NCNetworkAvcUnit::setFlag(NC_BOOL flag)
    {
        NCAutoSync lock(m_syncFlag);
        m_flag = flag;
    }

    NC_BOOL
    NCNetworkAvcUnit::getFlag()
    {
        NCAutoSync lock(m_syncFlag);
        return m_flag;
    }


} /* namespace nutshell */
/* EOF */
