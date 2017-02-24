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
 * @file NCNetworkDataReplier.h
 * @brief Declaration file of class NCNetworkDataReplier.
 *
 * This file includes the declaration of class NCNetworkDataReplier, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKDATAREPLIER_H
#define NCNETWORKDATAREPLIER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCGlobalAPI.h>
#include <evip/NEReplyReceiver.h>

namespace nutshell
{

    /**
     * @brief The replier class of NCNetworkData.
     *
     * This class should be inherited and implemented by user.
     */
    class NUTSHELL_API NCNetworkDataReplier : public NEReplyReceiver
    {
    public:

        /// Struct that is used to store network device information
        struct NetworkDeviceInfo
        {
            NCString ip;              ///< IP address
            NCString mac;             ///< Hardware address
            UINT32 prefixLength;      ///< Prefix length of mask
            NCString gateway;         ///< IP address of gateway
            NCString dns1;            ///< The ip of domain name server
            NCString dns2;            ///< The ip of domain name server
            NCString serverId;        ///< The server identifier
        };

        /// Store the connected client info
        struct ConnectedClientInfo
        {
            NCString ip;             ///< The IP address that is assigned
            NCString name;           ///< The connected client name
        };

        /**
         * Constructor
         */
        NCNetworkDataReplier() {}

        /**
         * Destructor
         */
        virtual ~NCNetworkDataReplier() {}

        /**
         * @brief According to the obtained network device info, user should do something.
         *
         * @param [in] device : The network device name.
         * @param [in] info : The network device info, wihch includes ip, mac, prefixLength, etc.
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyDeviceInfo(const NCString& device, const NetworkDeviceInfo& info) = 0;

        /**
         * @brief According to the obtained connected client info, user should do something.
         *
         * @param [in] mac : The hardware address of connected client.
         * @param [in] info : The connected client info, which include ip and host name.
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyConnectedClientInfo(const NCString& mac, const ConnectedClientInfo& info) = 0;

    private:
        NC_DISABLE_COPY(NCNetworkDataReplier);
    };

} /* namespace nutshell */

#endif /* NCNETWORKDATAREPLIER_H */
/* EOF */
