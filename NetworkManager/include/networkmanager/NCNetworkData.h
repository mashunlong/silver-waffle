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
 * @file NCNetworkData.h
 * @brief Declaration file of class NCNetworkData.
 *
 * This file includes the declaration of class NCNetworkData, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKDATA_H
#define NCNETWORKDATA_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCGlobalAPI.h>
#include <ncservice/NCBinderServiceProxy.h>
#include <evip/NEServiceProxy.h>

namespace nutshell
{

    /**
     * @brief The proxy class of NetworkManager.
     *
     * This class is used to get information which are related to network device.
     */
    class NUTSHELL_API NCNetworkData : public NCBinderServiceProxy, public NEServiceProxy
    {
    public:

        /**
         * Constructor
         */
        NCNetworkData();

        /**
         * Destructor
         */
        virtual ~NCNetworkData();

        /**
         * @brief Get information of network device.
         *
         * @param [in] deviceType : The type of network device.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 deviceInfo(const NCString& deviceType);

        /**
         * @brief When wifi AP mode is started, get the connected client info.
         *
         * @param [in] mac : The hardware address.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 connectedClientInfo(const NCString& mac);

        /**
         * @brief Get the name of current available network device.
         *
         * @param None.
         *
         * @return Return the current use network device type.
         *
         * @attention Synchronous I/F.
         */
        NCString currentUseNetwork();

        /**
         * @brief Get the status of network device, connected or disconnected
         *
         * @param [in] deviceType : The type of network device.
         *
         * @return Return the current status of network device, NC_TRUE indicates connected, NC_FALSE indicates failure.
         *
         * @attention Synchronous I/F.
         */
        NC_BOOL getDeviceStatus(const NCString& deviceType);

        /**
         * @brief Return the specified network device signal status. NetworkManager get the information 
         *        from environment variables, which are controlled by others.
         *
         * @param [in] deviceType : The type of network device.
         *
         * @return NC_TRUE indicates the network device has signal. NC_FALSE indicates that has no signal.
         *
         * @attention Synchronous I/F. 
         */
        NC_BOOL networkSignal(const NCString& deviceType);

        /**
         * @brief Bind service and register notify ID.
         *
         * @param None.
         *
         * @return NC_TRUE indicates success, NC_FALSE indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_BOOL bindService();

        /**
         * @brief Unkind service and unregister notify ID.
         *
         * @param None.
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID unbindService();

    private:
        /**
         * Receive message from service.
         */
        virtual VOID onReceiveMessage(NEMessage &message);

        NC_DISABLE_COPY(NCNetworkData);
    };

} /* namespace nutshell */

#endif /* NCNETWORKDATA_H */
/* EOF */
