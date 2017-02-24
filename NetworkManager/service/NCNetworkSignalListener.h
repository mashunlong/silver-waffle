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
 * @file NCNetworkSignalListener.h
 * @brief Declaration file of class NCNetworkSignalListener.
 *
 * This file includes the declaration of class NCNetworkSignalListener, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKSIGNALLISTENER_H
#define NCNETWORKSIGNALLISTENER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <ncore/NCString.h>
#include <contentresolver/NCContentListener.h>

namespace nutshell
{
    class NCNetworkSignal;
    /**
     * @brief Class of NCNetworkSignalListener.
     *
     * This class is used to receive the changed of network environment info.
     */
    class NCNetworkSignalListener : public NCContentListener
    {
    public:
        NCNetworkSignalListener(const NCString& name);
        virtual ~NCNetworkSignalListener();

        // Receive message when network device signal changed 
        virtual VOID onNotifyUpdate(NCUri &uri, NCVariant &variant);

    private:
        NCNetworkSignal* m_signal;
        
        NCNetworkSignalListener(const NCNetworkSignalListener&);
        NCNetworkSignalListener& operator=(const NCNetworkSignalListener&);
    };

} /* namespace nutshell */

#endif /* NCNETWORKSIGNALLISTENER_H */
/* EOF */
