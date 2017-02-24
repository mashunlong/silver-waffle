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
#include "nhnetwork/NHNetworkDefine.h"
#include "NHNetworkTimer.h"
#include "NHNetworkTimerThread.h"

#define NC_LOG_TAG "NHNetworkHandler"
#define NHNETWORK_HANDLER_TIMER_THREAD "network.handler.timer"
#define NHNETWORK_DHCLIENT_TIMER 1*1000

namespace nutshell
{
    NHNetworkTimerThread::NHNetworkTimerThread()
    : m_dhclientTimer(NULL)
    {
        m_dhclientTimer = new NHNetworkTimer(NHNETWORK_DHCLIENT_TIMER, NC_TRUE, TimerType_Dhclient);
    }

    NHNetworkTimerThread::~NHNetworkTimerThread()
    {
        if (NULL != m_dhclientTimer) {
            delete m_dhclientTimer;
            m_dhclientTimer = NULL;
        }
    }

    VOID
    NHNetworkTimerThread::startTimer(const UINT32 type)
    {
        NCLOGD("NHNetworkTimerThread::startTimer, type [%d]", type);
        if (0 == type) {
            NCLOGD("NHNetworkTimerThread::startTimer, timer type is failed");
            return;
        }

        m_syncObj.syncStart();
        startThread(NHNETWORK_HANDLER_TIMER_THREAD);
        m_timerList.push_back(type);
        m_syncObj.syncEnd();
        notify();
    }

    VOID
    NHNetworkTimerThread::stopTimer(const UINT32 type)
    {
        NCLOGD("NHNetworkTimerThread::stopTimer, Start, type [%d]", type);
        switch (type) {
        case TimerType_Dhclient:
        {
            m_dhclientTimer->stop();
            break;
        }
        default:
            break;
        }
    }

    VOID
    NHNetworkTimerThread::run()
    {
        NCLOGD("NHNetworkTimerThread::run, Start");
        while (!checkQuit()) {
            UINT32 type = pop();
            if (0 != type) {
                NCLOGD("NHNetworkTimerThread::run, type [%d]", type);
                switch (type) {
                case TimerType_Dhclient:
                {
                    if (m_dhclientTimer->isActive()) {
                        m_dhclientTimer->stop();
                    }
                    m_dhclientTimer->start();
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

    UINT32
    NHNetworkTimerThread::pop()
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

} /* namespace nutshell */
/* EOF */