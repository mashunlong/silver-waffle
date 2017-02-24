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
 * @file NHNetworkDhcp.h
 * @brief Declaration file of class NHNetworkDhcp.
 *
 * This file includes the declaration of class NHNetworkDhcp, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NHNETWORKDHCP_H
#define NHNETWORKDHCP_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <ncore/NCString.h>

namespace nutshell
{

    /**
     * @brief This class is used to manage DHCP service.
     *
     * This class can start or stop DHCP client and DHCP server.
     */
    class NHNetworkDhcp
    {
    public:
        NHNetworkDhcp();
        virtual ~NHNetworkDhcp();

        UINT32 startClient(const NCString& deviceName);
        UINT32 stopClient(const NCString& deviceName);
        UINT32 releaseIp(const NCString& deviceName);

        UINT32 startServer(const NCString& deviceName);
        UINT32 stopServer(const NCString& deviceName);

    private:
        NC_BOOL checkCancelFlag();

        NC_BOOL readDhclientPid(const NCString& deviceName, LONG& pid);
        VOID rmDhclientPid(const NCString& deviceName);

        NC_DISABLE_COPY(NHNetworkDhcp);
    };

} /* namespace nutshell */

#endif /* NHNETWORKDHCP_H */
/* EOF */
