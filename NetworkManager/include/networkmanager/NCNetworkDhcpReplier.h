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
 * @file NCNetworkDhcpReplier.h
 * @brief Declaration file of class NCNetworkDhcpReplier.
 *
 * This file includes the declaration of class NCNetworkDhcpReplier, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKDHCPREPLIER_H
#define NCNETWORKDHCPREPLIER_H

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
     * @brief The replier class of NCNetworkDhcp.
     *
     * This class should be inherited and implemented by user.
     */
    class NUTSHELL_API NCNetworkDhcpReplier : public NEReplyReceiver
    {
    public:

        /**
         * Constructor
         */
        NCNetworkDhcpReplier() {}

        /**
         * Destructor
         */
        virtual ~NCNetworkDhcpReplier() {}

        /**
         * @brief According to the request IP result, user should do something.
         *
         * @param [in] deviceType : The device type.
         * @param [in] result : The result of request IP via DHCP protocol.
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyRequestIp(const NCString& deviceType, UINT32 result) = 0;

        /**
         * @brief According to the stop request IP result, user should do something.
         *
         * @param [in] deviceType : The network device type.
         * @param [in] result : The result of stop request IP.
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyStopRequestIp(const NCString& deviceType, UINT32 result) = 0;

        /**
         * @brief According to the release IP result, user should do something.
         *
         * @param [in] deviceType : The network device type.
         * @param [in] result : The result of release IP.
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyReleaseIp(const NCString& deviceType, UINT32 result) = 0;


        /**
         * @brief According to the start DHCP server result, user should do something.
         *
         * @param [in] deviceType : The network device type.
         * @param [in] result : The result of start DHCP server.
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyStartDhcpServer(const NCString& deviceType, UINT32 result) = 0;

        /**
         * @brief According to the stop DHCP server result, user should do something.
         *
         * @param [in] deviceType : The network device type.
         * @param [in] result : The result of stop DHCP server.
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyStopDhcpServer(const NCString& deviceType, UINT32 result) = 0;

    private:
        NC_DISABLE_COPY(NCNetworkDhcpReplier);
    };

} /* namespace nutshell */

#endif /* NCNETWORKDHCPREPLIER_H */
/* EOF */
