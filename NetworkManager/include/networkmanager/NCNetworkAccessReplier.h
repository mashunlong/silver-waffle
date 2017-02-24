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
 * @file NCNetworkAccessReplier.h
 * @brief Declaration file of class NCNetworkAccessReplier.
 *
 * This file includes the declaration of class NCNetworkAccessReplier, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKACCESSREPLIER_H
#define NCNETWORKACCESSREPLIER_H

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
     * @brief The replier class of NCNetworkAccess.
     *
     * This class should be inherited and implemented by user.
     */
    class NUTSHELL_API NCNetworkAccessReplier : public NEReplyReceiver
    {
    public:

        /**
         * Constructor
         */
        NCNetworkAccessReplier() {}

        /**
         * Destructor
         */
        virtual ~NCNetworkAccessReplier() {}

        /**
         * @brief According to the request result, user need to do something.
         *
         * @param [in] resourceId : The resource identify.
         * @param [in] result : The result of request network access.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyRequest(const NCString& resourceId, UINT32 result) = 0;

        /**
         * This interface will be deleated later.
         */
        virtual VOID replyRequestResult(const NCString& deviceType, UINT32 result) {}

        /**
         * @brief Reply the result of request network access with sequence ID.
         *
         * @param [in] seqId : The sequence ID for this reply.
         * @param [in] resourceId : The resource identify.
         * @param [in] result : The result of request network access.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyRequestWithSequenceId(const UINT32 seqId, const NCString& resourceId, const UINT32 result) {}

        /**
         * @brief According to the disconnect result, user need to do something.
         *
         * @param [in] deviceType : The device type.
         * @param [in] result : The result of disconnect network
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyDisconnect(const NCString& deviceType, UINT32 result) {}

        /**
         * @brief Reply the result of disconnect network with sequence ID.
         *
         * @param [in] seqId : The sequence ID
         * @param [in] deviceType : The device type.
         * @param [in] result : The result of disconnect network
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyDisconnectWithSeqId(const UINT32 seqId, const NCString& deviceType, const UINT32 result) {}

        /**
         * @brief According to the release result, user need to do something.
         *
         * @param [in] resourceId : The resource identify.
         * @param [in] result : The result of release network access.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyRelease(const NCString& resourceId, UINT32 result) = 0;

        /**
         * @brief Reply the result of release network access with sequence ID.
         *
         * @param [in] seqId : The sequence ID for this reply.
         * @param [in] resourceId : The resource identify.
         * @param [in] result : The result of release network access.
         *
         * @return NC_NOERROR indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID replyReleaseWithSequenceId(const UINT32 seqId, const NCString& resourceId, const UINT32 result) {}

    private:
        NC_DISABLE_COPY(NCNetworkAccessReplier);
    };

} /* namespace nutshell */

#endif /* NCNETWORKACCESSREPLIER_H */
/* EOF */
