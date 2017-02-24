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
 * @file NCNetworkTimerThread.h
 * @brief Declaration file of class NCNetworkTimerThread.
 *
 * This file includes the declaration of class NCNetworkTimerThread, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */
#ifndef NCNETWORKTIMERTHREAD_H
#define NCNETWORKTIMERTHREAD_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <vector>
#include <ncore/NCSyncObj.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCThread.h>

namespace nutshell
{
    class NCNetworkTimer;
    /**
     * The timer thread for NetworkManager
     *
     * All the request related timer will be posted to this thread.
     *
     */
    class NCNetworkTimerThread: public NCThread
    {
    public:
        NCNetworkTimerThread();
        virtual ~NCNetworkTimerThread();

        VOID startTimer(const UINT32 timerType);
        VOID stopTimer(const UINT32 timerType);

        UINT32 pop();
        virtual VOID run();

    private:
        std::vector<UINT32> m_timerList;
        NCSyncObj m_syncObj;
        NCNetworkTimer* m_dcuMeuTimer;
        NCNetworkTimer* m_avcLanTimer;
        NCNetworkTimer* m_wifiApTimer;
        NCNetworkTimer* m_requestMapTimer;
        
        NCNetworkTimerThread(const NCNetworkTimerThread&);
        NCNetworkTimerThread& operator=(const NCNetworkTimerThread&);
    };

} /* namespace nutshell */

#endif /* NCNETWORKTIMERTHREAD_H */
/* EOF */
