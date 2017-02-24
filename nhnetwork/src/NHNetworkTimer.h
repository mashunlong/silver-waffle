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
 * @file NHNetworkTimer.h
 * @brief Declaration file of class NHNetworkTimer.
 *
 * This file includes the declaration of class NHNetworkTimer, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NHNETWORKTIMER_H
#define NHNETWORKTIMER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCTimer.h>

namespace nutshell
{

    /**
     * @brief Class of NHNetworkTimer.
     *
     * This class is used to time for network handler
     */
    class NHNetworkTimer : public NCTimer
    {
    public:
        NHNetworkTimer(const LONG msec, const NC_BOOL iter, const UINT32 type);
        virtual ~NHNetworkTimer();

        VOID OnTimer();

    private:
        VOID readDhclientProperty(NCString& deviceName, DhclientInfo& info);

        UINT32 m_type;

        NHNetworkTimer(const NHNetworkTimer&);
        NHNetworkTimer& operator=(const NHNetworkTimer&);
    };

} /* namespace nutshell */

#endif /* NHNETWORKTIMER_H */
/* EOF */
