/**
 * Copyright @ 2013 - 2014 Suntec Software(Shanghai) Co., Ltd.
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
 * @file NCNetworkDeviceType.h
 * @brief Declaration file of class NCNetworkDeviceType.
 *
 * This file includes the declaration of class NCNetworkDeviceType, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKDEVICETYPE_H
#define NCNETWORKDEVICETYPE_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <map>
#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCSyncObj.h>

namespace nutshell
{

    /**
     * @brief Class of NCNetworkDeviceType.
     *
     * This class is used to store the correspondence between device type and device name.
     */
    class NCNetworkDeviceType
    {
    public:
        NCNetworkDeviceType();
        virtual ~NCNetworkDeviceType();

        VOID add(const NCString& deviceName, const NCString& deviceType);
        VOID remove(const NCString& deviceName);
        NC_BOOL getType(const NCString& deviceName, NCString& deviceType);
        NC_BOOL getName(const NCString& deviceType, NCString& deviceName);

    private:
        VOID initDeviceTypeMap();

        std::map<NCString, NCString> m_typeMap;
        NCSyncObj m_syncObj;

        NC_DISABLE_COPY(NCNetworkDeviceType);

    };

} /* namespace nutshell */

#endif /* NCNETWORKDEVICETYPE_H */
/* EOF */
