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
 * @file NCNetworkEventHandler.h
 * @brief Declaration file of class NCNetworkEventHandler.
 *
 * This file includes the declaration of class NCNetworkEventHandler, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKEVENTHANDLER_H
#define NCNETWORKEVENTHANDLER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <nceventsys/NCEventHandler.h>
#include <networkmanager/NCNetworkDataReplier.h>

namespace nutshell
{
    class NCNetworkManager;
    /**
     * @brief This class is used to receive and handle event.
     *
     * This class can receive event from network handler and services.
     */
    class NCNetworkEventHandler : public NCEventHandler
    {
    public:
        NCNetworkEventHandler(const NCRunnableLooper& looper);
        virtual ~NCNetworkEventHandler();

        VOID handleEvent(const NCEvent& event);

    private:
        VOID doSetStaticIpMaskMtu(const NCString& deviceName, const UINT32 result);
        VOID doDeviceInfo(const NCString& deviceName, const NCNetworkDataReplier::NetworkDeviceInfo& info, const UINT32 result);
        VOID doDhcpEvent(const NCString& deviceName, const NCString& gateway, const NCString& dns1, const NCString& dns2, const UINT32 result);
        VOID doReleaseIpEvent(const NCString& deviceName, const UINT32 result);
        VOID doAddDefaultRoute(const NCString& deviceName, const UINT32 result);
        VOID doRmDefaultRoute(const NCString& deviceName, const UINT32 result);

        VOID doStartDhcpServer(const NCString& deviceName, const UINT32 result);
        VOID doStopDhcpServer(const NCString& deviceName, const UINT32 result);
        VOID doRestartDhcpServer(const NCString& deviceName, const UINT32 result);
        VOID doAddRoute(const NCString& deviceName, const UINT32 result);

        NC_DISABLE_COPY(NCNetworkEventHandler);
    };

    typedef ncsp<NCNetworkEventHandler>::sp NCNetworkEventHandlerStrongHolder;
    typedef ncsp<NCNetworkEventHandler>::wp NCNetworkEventHandlerWeakHolder;

} /* namespace nutshell */

#endif /* NCNETWORKEVENTHANDLER_H */
/* EOF */
