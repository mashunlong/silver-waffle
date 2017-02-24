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
/**
 * @file NHNetworkTimerThread.h
 * @brief Declaration file of class NHNetworkTimerThread.
 *
 * This file includes the declaration of class NHNetworkTimerThread, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */
#ifndef NHNETWORKTIMERTHREAD_H
#define NHNETWORKTIMERTHREAD_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif


#include <vector>
#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCSyncObj.h>
#include <ncore/NCThread.h>

namespace nutshell
{
    class NHNetworkTimer;
    /**
     * The timer thread for NHNetworkTimerThread
     *
     * All the request related timer will be posted to this thread.
     *
     */
    class NHNetworkTimerThread: public NCThread
    {
    public:
        NHNetworkTimerThread();
        virtual ~NHNetworkTimerThread();

        VOID startTimer(const UINT32 type);
        VOID stopTimer(const UINT32 type);

        virtual VOID run();

    private:
        UINT32 pop();

        std::vector<UINT32> m_timerList;
        NHNetworkTimer* m_dhclientTimer;

        NCSyncObj m_syncObj;

        NHNetworkTimerThread(const NHNetworkTimerThread&);
        NHNetworkTimerThread& operator=(const NHNetworkTimerThread&);
    };

} /* namespace nutshell */

#endif /* NHNETWORKTIMERTHREAD_H */
/* EOF */
