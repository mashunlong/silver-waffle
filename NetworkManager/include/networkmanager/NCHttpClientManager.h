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
 * @file NCHttpClientManager.h
 * @brief Declaration file of class NCHttpClientManager.
 *
 * This file includes the declaration of class NCHttpClientManager, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCHTTPCLIENTMANAGER_H
#define NCHTTPCLIENTMANAGER_H

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
     * @brief The proxy class of HttpClient manager
     *
     * This class provides interfaces of HttpClient network access permission
     */
    class NUTSHELL_API NCHttpClientManager : public NCBinderServiceProxy, public NEServiceProxy
    {
    public:

        /**
         * Constructor
         */
        NCHttpClientManager();

        /**
         * Destructor
         */
        virtual ~NCHttpClientManager();

        /**
         * @brief Request HttpClient network access permission.
         *
         * @param None.
         *
         * @return NetworkErrCode_Success indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 getAccessPermission();

        /**
         * @brief Release HttpClient network access permission.
         *
         * @param None.
         *
         * @return NetworkErrCode_Success indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 releaseAccessPermission();

        /**
         * @brief Set log flag
         *
         * @param log switch.
         *
         * @return NetworkErrCode_Success indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 setLogFileFlag(NC_BOOL flag);

        /**
         * @brief Get log flag
         *
         * @param log status.
         *
         * @return NetworkErrCode_Success indicates success, other value indicates failure.
         *
         * @attention Synchronous I/F.
         */
        INT32 getLogFileFlag(NC_BOOL &flag);

    private:
        NCString getSelfTaskName();

        NC_DISABLE_COPY(NCHttpClientManager);
    };

} /* namespace nutshell */

#endif /* NCHTTPCLIENTMANAGER_H */
/* EOF */
