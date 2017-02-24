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
 * @file NCHttpClients.h
 * @brief Declaration file of class NCHttpClients.
 *
 * This file includes the declaration of class NCHttpClients, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCHTTPCLIENTS_H
#define NCHTTPCLIENTS_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <vector>
#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCParcel.h>
#include <ncore/NCAutoSync.h>

namespace nutshell
{

#define NCNETWORK_HTTPCLIENT_MAXACCESS_PERTASK  30
#define NCNETWORK_HTTPCLIENT_MAXACCESS_TOTAL  300
#define NCNETWORK_HTTPCLIENT_ACCESSCHECK_NUM  (NCNETWORK_HTTPCLIENT_MAXACCESS_TOTAL - 20)

#define NCNETWORK_HTTPCLIENT_LOGFLAG_FILEPATH    ("/var/system/lighttpd.flg")

    /**
     * @brief The service class of httpclient
     *
     * This class provides interfaces of httpclient
     */
    class NCHttpClients
    {
    public:

        typedef struct
        {
            NCString TaskName;
            INT32 pID;
            std::vector<INT32> tIDList;
        }HttpClientAccessNode;

        /**
         * Constructor
         */
        NCHttpClients();

        /**
         * Destructor
         */
        virtual ~NCHttpClients();

        /**
         * @brief Parse the request from NCHttpClientManager
         *
         * @param [in] code : The function ID.
         * @param [in] data : The data contains the required parameters.
         * @param [in] reply: The pointer is used to store the returned result.
         * @param [in] flags: The flags is usually 0.
         *
         * @return None.
         */
        VOID onCommand(UINT32 code, const NCParcel& data, NCParcel* reply, UINT32 flags);

        /**
         * @brief get service info
         *
         * @return Service Info.
         */
        NCString dumpService();

    private:
        /**
         * @brief Request access permission of socket.
         *
         * @param [in] taskName : The name of request moudle task.
         * @param [in] pID : The PID of request moudle task.
         * @param [in] tID:  The tID of request moudle task.
         *
         * @return NetworkErrCode_Success indicates success, other value indicates failure.
         *
         */
        INT32 getAccessPermission(const NCString& taskName, const INT32 pID, const INT32 tID);

        /**
         * @brief Release access permission of socket.
         *
         * @param [in] pID : The PID of request moudle task.
         * @param [in] tID:  The tID of request moudle task.
         *
         * @return NetworkErrCode_Success indicates success, other value indicates failure.
         *
         */
        INT32 releaseAccessPermission(const INT32 pID, const INT32 tID);

        /**
         * @brief Set log flag
         *
         * @param log switch.
         *
         * @return NetworkErrCode_Success indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 setLogFileFlag(INT32 flag);

        /**
         * @brief Get log flag
         *
         * @param log status.
         *
         * @return NetworkErrCode_Success indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 getLogFileFlag(INT32 &flag);

        INT32 getPIDList(std::vector<INT32> &pIDList);
        NC_BOOL isPIDActive(const std::vector<INT32>& pIDList, INT32 PID);

    private:
        NCSyncObj m_syncLock;
        std::vector<HttpClientAccessNode> mAccessList;
        UINT32 mTotalAccessNum;

        NC_DISABLE_COPY(NCHttpClients);
    };

} /* namespace nutshell */

#endif /* NCHTTPCLIENTS_H */
/* EOF */
