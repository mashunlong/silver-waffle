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
 * @file NCNetworkTimer.h
 * @brief Declaration file of class NCNetworkTimer.
 *
 * This file includes the declaration of class NCNetworkTimer, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKTIMER_H
#define NCNETWORKTIMER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTimer.h>

namespace nutshell
{

    /**
     * @brief Class of NCNetworkTimer.
     *
     * This class is used to time for NetworkManger
     */
    class NCNetworkTimer : public NCTimer
    {
    public:
        NCNetworkTimer(const UINT32 type, const LONG msec, NC_BOOL iter);
        virtual ~NCNetworkTimer();

        VOID OnTimer();

    private:
        UINT32 m_type;

        NCNetworkTimer(const NCNetworkTimer&);
        NCNetworkTimer& operator=(const NCNetworkTimer&);
    };

} /* namespace nutshell */

#endif /* NCNETWORKTIMER_H */
/* EOF */
