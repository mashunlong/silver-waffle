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
 * @file NCNetworkSignal.h
 * @brief Declaration file of class NCNetworkSignal.
 *
 * This file includes the declaration of class NCNetworkSignal, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKSIGNAL_H
#define NCNETWORKSIGNAL_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <ncore/NCString.h>

namespace nutshell
{

    /**
     * @brief Class of NCNetworkStatus.
     *
     * This class is used to query or update the system information. The URI is content://system/info
     */
    class NCNetworkSignal
    {
    public:
        /**
         * Constructor
         */
        NCNetworkSignal();

        /**
         * Destructor
         */
        virtual ~NCNetworkSignal();

        NC_BOOL signal(const NCString& deviceType);
        VOID setSignalStatus(const NCString& deviceType, const NCString& value);

        VOID setDcmDevStatus(const NCString& value);
        VOID setBtDunDevStatus(const NCString& value);

        VOID updateNetworkStatus(const NCString& deviceType, const NCString& value);

    private:
        NC_BOOL checkSignalStatus(const NCString& deviceType, const NCString& value);

        NC_BOOL m_wifiSignal;
        NC_BOOL m_dcmSignal;
        NC_BOOL m_btphoneSignal;

        NC_DISABLE_COPY(NCNetworkSignal);
    };

} /* namespace nutshell */

#endif /* NCNETWORKSIGNAL_H */
/* EOF */
