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
 * @file NCNetworkUsbGadgetnetReplier.h
 * @brief Declaration file of class NCNetworkUsbGadgetnetReplier.
 *
 * This file includes the declaration of class NCNetworkUsbGadgetnetReplier, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKTIMER_H
#define NCNETWORKTIMER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <connectmanager/NCUsbGadgetnetReplier.h>

namespace nutshell
{

    /**
     * @brief Class of NCNetworkUsbGadgetnetReplier.
     *
     * This class is used to receive MirrorLink connect status from ConnectManager.
     */
    class NCNetworkUsbGadgetnetReplier : public NCUsbGadgetnetReplier
    {
    public:
        NCNetworkUsbGadgetnetReplier();
        virtual ~NCNetworkUsbGadgetnetReplier();

        virtual VOID onConnected(const NCString& id);
        virtual VOID onDisconnected(const NCString& id);

    private:
        NCNetworkUsbGadgetnetReplier(const NCNetworkUsbGadgetnetReplier&);
        NCNetworkUsbGadgetnetReplier& operator=(const NCNetworkUsbGadgetnetReplier&);
    };

} /* namespace nutshell */

#endif /* NCNETWORKTIMER_H */
/* EOF */
