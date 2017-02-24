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

#include <ncore/NCLog.h>
#include <ncore/NCAutoSync.h>
#include "NCNetworkDeviceType.h"

namespace nutshell
{
    NCNetworkDeviceType::NCNetworkDeviceType()
    {
        initDeviceTypeMap();
    }

    NCNetworkDeviceType::~NCNetworkDeviceType()
    {

    }

    VOID
    NCNetworkDeviceType::initDeviceTypeMap()
    {

    }

    VOID
    NCNetworkDeviceType::add(const NCString& deviceName, const NCString& deviceType)
    {
        NCAutoSync lock(m_syncObj);
        std::map<NCString, NCString>::iterator initIter = m_typeMap.begin();
        while (initIter != m_typeMap.end()) {
            if (deviceType == initIter->second) {
                m_typeMap.erase(initIter);
                break;
            }
            ++initIter;
        }
        
        std::map<NCString, NCString>::iterator iter = m_typeMap.find(deviceName);
        if (iter != m_typeMap.end()) {
            iter->second = deviceType;
        }
        else {
            m_typeMap.insert(std::map<NCString, NCString>::value_type(deviceName, deviceType));
        }

    }

    VOID
    NCNetworkDeviceType::remove(const NCString& deviceName)
    {
        NCAutoSync lock(m_syncObj);
        std::map<NCString, NCString>::iterator iter = m_typeMap.find(deviceName);
        if (iter != m_typeMap.end()) {
            m_typeMap.erase(iter);
        }
    }

    NC_BOOL
    NCNetworkDeviceType::getType(const NCString& deviceName, NCString& deviceType)
    {
        NCAutoSync lock(m_syncObj);
        std::map<NCString, NCString>::iterator iter = m_typeMap.find(deviceName);
        if (iter != m_typeMap.end()) {
            deviceType = iter->second;
            return NC_TRUE;
        }
        else {
            return NC_FALSE;
        }
    }

    NC_BOOL
    NCNetworkDeviceType::getName(const NCString& deviceType, NCString& deviceName)
    {
        NCAutoSync lock(m_syncObj);
        std::map<NCString, NCString>::iterator iter = m_typeMap.begin();
        while (iter != m_typeMap.end()) {
            if (deviceType == iter->second) {
                deviceName = iter->first;
                return NC_TRUE;
            }
            ++iter;
        }

        return NC_FALSE;
    }


} /* namespace nutshell */
/* EOF */
