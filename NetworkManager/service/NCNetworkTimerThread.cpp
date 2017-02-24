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
#include "NCNetworkTimer.h"
#include "NCNetworkTimerThread.h"

#define NC_LOG_TAG "NCNetworkManager"
#define NC_AVCUNIT_TIMER 5*1000
#define AVCLAN_IP_TIMER 1*1000
#define FIREWALL_CONFIG_TIMER 2*1000
#define REQUEST_MAP_TIMER 60*1000

#define WIFI_AP_TIME 1*1000

namespace nutshell
{
    NCNetworkTimerThread::NCNetworkTimerThread()
    : m_dcuMeuTimer(NULL)
    , m_avcLanTimer(NULL)
    , m_wifiApTimer(NULL)
    , m_requestMapTimer(NULL)
    {
        m_dcuMeuTimer = new NCNetworkTimer(TimerType_DcuMeuSync, NC_AVCUNIT_TIMER, NC_TRUE);
        m_avcLanTimer = new NCNetworkTimer(TimerType_AvcLanIp, AVCLAN_IP_TIMER, NC_FALSE);
        m_wifiApTimer = new NCNetworkTimer(TimerType_WifiAp, WIFI_AP_TIME, NC_TRUE);
        m_requestMapTimer = new NCNetworkTimer(TimerType_RequestMap, REQUEST_MAP_TIMER, NC_TRUE);
    }

    NCNetworkTimerThread::~NCNetworkTimerThread()
    {
        if (NULL != m_dcuMeuTimer) {
            delete m_dcuMeuTimer;
            m_dcuMeuTimer = NULL;
        }

        if (NULL != m_avcLanTimer) {
            delete m_avcLanTimer;
            m_avcLanTimer = NULL;
        }

        if (NULL != m_wifiApTimer) {
            delete m_wifiApTimer;
            m_wifiApTimer = NULL;
        }

        if (NULL != m_requestMapTimer) {
            delete m_requestMapTimer;
            m_requestMapTimer = NULL;
        }
    }

    VOID
    NCNetworkTimerThread::startTimer(const UINT32 timerType)
    {
        NCLOGD("NCNetworkTimerThread::startTimer, timerType [%d]", timerType);
        if (0 == timerType) {
            NCLOGD("NCNetworkTimerThread::startTimer, timer type is failed");
            return;
        }

        m_syncObj.syncStart();
        startThread(NCNETWORK_TIMER_THREAD);
        m_timerList.push_back(timerType);
        m_syncObj.syncEnd();
        notify();
    }

    VOID
    NCNetworkTimerThread::stopTimer(const UINT32 timerType)
    {
        NCLOGD("NCNetworkTimerThread::stopTimer, Start, timer type [%d]", timerType);
        switch (timerType) {
        case TimerType_DcuMeuSync:
        {
            m_dcuMeuTimer->stop();
            break;
        }
        case TimerType_AvcLanIp:
        {
            m_avcLanTimer->stop();
            break;
        }
        case TimerType_WifiAp:
        {
            m_wifiApTimer->stop();
            break;
        }
        case TimerType_RequestMap:
        {
            m_requestMapTimer->stop();
            break;
        }
        default:
            break;
        }
    }

    UINT32
    NCNetworkTimerThread::pop()
    {
        UINT32 cur = 0;
        m_syncObj.syncStart();
        if (!m_timerList.empty()) {
            std::vector<UINT32>::iterator it = m_timerList.begin();
            cur = (*it);
            it = m_timerList.erase(it);
        }
        m_syncObj.syncEnd();
        return cur;
    }

    VOID
    NCNetworkTimerThread::run()
    {
        NCLOGD("NCNetworkTimerThread::run, Start");
        while (!checkQuit()) {
            UINT32 timerType = pop();
            if (0 != timerType) {
                NCLOGD("NCNetworkTimerThread::run, timerType [%d]", timerType);
                switch (timerType) {
                case TimerType_DcuMeuSync:
                {
                    if (m_dcuMeuTimer->isActive()) {
                        m_dcuMeuTimer->stop();
                    }
                    m_dcuMeuTimer->start();
                    break;
                }
                case TimerType_AvcLanIp:
                {
                    if (m_avcLanTimer->isActive()) {
                        m_avcLanTimer->stop();
                    }
                    m_avcLanTimer->start();
                    break;
                }
                case TimerType_WifiAp:
                {
                    if (m_wifiApTimer->isActive()) {
                        m_wifiApTimer->stop();
                    }
                    m_wifiApTimer->start();
                    break;
                }
                case TimerType_RequestMap:
                {
                    if (m_requestMapTimer->isActive()) {
                        m_requestMapTimer->stop();
                    }
                    m_requestMapTimer->start();
                    break;
                }
                default:
                    break;
                }

                continue;
            }

            wait();
        }
    }

} /* namespace nutshell */
/* EOF */