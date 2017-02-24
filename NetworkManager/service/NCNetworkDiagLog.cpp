/**
 * Copyright @ 2014 - 2015 Suntec Software(Shanghai) Co., Ltd.
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
 * @file NCNetworkDiagLog.cpp
 * @brief implements of NCNetworkDiagLog
 */

#include <ncore/NCLog.h>
#include "NCNetworkDiagLog.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{

    NCNetworkDiagLog::NCNetworkDiagLog()
    : m_isDeviceOpen(NC_FALSE)
    , m_device()
    , m_handle(NULL)
    {

    }

    NCNetworkDiagLog::~NCNetworkDiagLog()
    {

    }

    NC_BOOL
    NCNetworkDiagLog::startDiagLog()
    {
        NCLOGD("NCNetworkDiagLog::startDiagLog, Start");
        int rst = ni_hallib_device_open("diag", &m_device, sizeof(m_device));
        if (NIHALLIB_ERROR_NONE != rst) {
            NCLOGD("NCNetworkDiagLog::startDiagLog, Open diag so failed");
            return NC_FALSE;
        }

        m_isDeviceOpen = NC_TRUE;

        m_handle = m_device.start();
        if (NULL == m_handle) {
            ni_hallib_device_close(&m_device);
            m_isDeviceOpen = NC_FALSE;
            NCLOGD("NCNetworkDiagLog::startDiagLog, device start failed");
            return NC_FALSE;
        }

        return NC_TRUE;
    }

    VOID
    NCNetworkDiagLog::stopDiagLog()
    {
        NCLOGD("NCNetworkDiagLog::stopDiagLog, Start");
        if (!m_isDeviceOpen) {
            return;
        }

        if (NULL != m_handle) {
            m_device.stop(m_handle);
        }

        ni_hallib_device_close(&m_device);
    }


    NC_BOOL
    NCNetworkDiagLog::writeDiagLog(const UINT8 deviceType, const UINT8 failedType, const NCString& option)
    {
        NCLOGD("NCNetworkDiagLog::writeDiagLog, Start, deviceType [%d], failedType [%d], optionLength [%d], option[%s]", 
            deviceType, failedType, strlen(option.getString()), option.getString());
        if (NULL == m_handle || !m_isDeviceOpen) {
            NCLOGD("NCNetworkDiagLog::writeDiagLog, Handler is null || Device is not opened");
            return NC_FALSE;
        }

        NCNetworkDiagLogInfo diagInfo;
        memset(&diagInfo, 0, sizeof(NCNetworkDiagLogInfo));
        diagInfo.deviceType = deviceType;
        diagInfo.failedType = failedType;
        diagInfo.optionLength = (UINT8)strlen(option.getString());
        strncpy(diagInfo.option, option.getString(), sizeof(diagInfo.option) - 1);

        if (!m_device.write(m_handle, DIAGRECORDID_NET, sizeof(NCNetworkDiagLogInfo), &diagInfo)) {
            NCLOGD("NCNetworkDiagLog::writeDiagLog, Write failed");
            return NC_FALSE;
        }

        return NC_TRUE;
    }

} // end of nutshell
/* EOF */
