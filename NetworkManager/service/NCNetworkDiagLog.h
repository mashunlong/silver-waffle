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
 * @file  NCNetworkDiagLog.h
 * @brief Class of NCNetworkDiagLog
 */
#ifndef NCNETWORKDIAGLOG_H
#define NCNETWORKDIAGLOG_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <ni_hallib.h>
#include <ni_hallib_diag.h>
#include "NCNetworkManagerDef.h"

namespace nutshell
{
    /// Diag log info
    struct NCNetworkDiagLogInfo
    {
        UINT8 deviceType;
        UINT8 failedType;
        UINT8 optionLength;
        CHAR option[21];
    };

    /**
     * @brief Class of NCNetworkDiagLog
     *
     * This class is used to record error log by diag.
     */
    class NCNetworkDiagLog
    {
    public:
        NCNetworkDiagLog();
        virtual ~NCNetworkDiagLog();

        NC_BOOL startDiagLog();
        VOID stopDiagLog();
        NC_BOOL writeDiagLog(const UINT8 deviceType, const UINT8 failedType, const NCString& option);

    private:
        NC_BOOL m_isDeviceOpen;
        ni_hallib_diag_t m_device;
        PVOID m_handle;
        NCNetworkDiagLogInfo m_logInfo;

        NCNetworkDiagLog(const NCNetworkDiagLog&);
        NCNetworkDiagLog& operator=(const NCNetworkDiagLog&);
    };

} // end of nutshell

#endif /* NCNETWORKDIAGLOG_H */
/* EOF */
