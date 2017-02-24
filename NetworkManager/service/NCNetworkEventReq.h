/**
 * Copyright @ 2013 - 2015 Suntec Software(Shanghai) Co., Ltd.
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
 * @file NCNetworkEventReq.h
 * @brief class define of NCNetworkEventReq.
 */

#ifndef NCNETWORKEVENTREQ_H
#define NCNETWORKEVENTREQ_H

#ifndef __cplusplus
#   error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <nceventsys/NCRunnable.h>

namespace nutshell
{
    /**
     * @brief This class is used to check network request.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkCheckRequstReq : public NCRunnable
    {
    public:

        NCNetworkCheckRequstReq(const AccessParameter& param);
        virtual ~NCNetworkCheckRequstReq();

        VOID run();

    private:
        AccessParameter m_param;
        
        NCNetworkCheckRequstReq(const NCNetworkCheckRequstReq&);
        NCNetworkCheckRequstReq& operator=(const NCNetworkCheckRequstReq&);
    };

    /**
     * @brief This class is used to check network release.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkCheckReleaseReq : public NCRunnable
    {
    public:

        NCNetworkCheckReleaseReq(const AccessParameter& param);
        virtual ~NCNetworkCheckReleaseReq();

        VOID run();

    private:
        AccessParameter m_param;
        
        NCNetworkCheckReleaseReq(const NCNetworkCheckReleaseReq&);
        NCNetworkCheckReleaseReq& operator=(const NCNetworkCheckReleaseReq&);
    };

    /**
     * @brief This class is used to check network disconnect.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkCheckDisconnectReq : public NCRunnable
    {
    public:

        NCNetworkCheckDisconnectReq(const AccessParameter& param);
        virtual ~NCNetworkCheckDisconnectReq();

        VOID run();

    private:
        AccessParameter m_param;
        
        NCNetworkCheckDisconnectReq(const NCNetworkCheckDisconnectReq&);
        NCNetworkCheckDisconnectReq& operator=(const NCNetworkCheckDisconnectReq&);
    };

    /**
     * @brief This class is used to process request callback result.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkRequestCallbackReq : public NCRunnable
    {
    public:

        NCNetworkRequestCallbackReq(const RequestPluginResult& info);
        virtual ~NCNetworkRequestCallbackReq();

        VOID run();

    private:
        UINT32 m_reqId;
        UINT32 m_result;
        NCString m_deviceType;
        NCString m_deviceName;
        NCString m_gateway;
        NCString m_dns1;
        NCString m_dns2;

        NCNetworkRequestCallbackReq(const NCNetworkRequestCallbackReq&);
        NCNetworkRequestCallbackReq& operator=(const NCNetworkRequestCallbackReq&);
    };

    /**
     * @brief This class is used to process update callback result.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkReleaseCallbackReq : public NCRunnable
    {
    public:

        NCNetworkReleaseCallbackReq(const UINT32 result, const UINT32 seqId);
        virtual ~NCNetworkReleaseCallbackReq();

        VOID run();

    private:
        UINT32 m_result;
        UINT32 m_seqId;

        NCNetworkReleaseCallbackReq(const NCNetworkReleaseCallbackReq&);
        NCNetworkReleaseCallbackReq& operator=(const NCNetworkReleaseCallbackReq&);
    };

    /**
     * @brief This class is used to process release callback result.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkDisconnectCallbackReq : public NCRunnable
    {
    public:
        NCNetworkDisconnectCallbackReq(const UINT32 result, const NCString& deviceType, const UINT32 seqId);
        virtual ~NCNetworkDisconnectCallbackReq();

        VOID run();

    private:
        UINT32 m_result;
        NCString m_deviceType;
        UINT32 m_seqId;

        NCNetworkDisconnectCallbackReq(const NCNetworkDisconnectCallbackReq&);
        NCNetworkDisconnectCallbackReq& operator=(const NCNetworkDisconnectCallbackReq&);
    };

    /**
     * @brief This class is used to request access complete.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkCheckRequestCompleteReq : public NCRunnable
    {
    public:
        NCNetworkCheckRequestCompleteReq();
        virtual ~NCNetworkCheckRequestCompleteReq();

        VOID run();
        VOID doCheckRequestComplete();

    private:
        NCNetworkCheckRequestCompleteReq(const NCNetworkCheckRequestCompleteReq&);
        NCNetworkCheckRequestCompleteReq& operator=(const NCNetworkCheckRequestCompleteReq&);
    };

    /**
     * @brief This class is used to add request into map
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkAddRequestMapReq : public NCRunnable
    {
    public:
        NCNetworkAddRequestMapReq(const UINT32 mapType, const AccessParameter& param);
        virtual ~NCNetworkAddRequestMapReq();

        VOID run();

    private:
        UINT32 m_mapType;
        AccessParameter m_param;
        
        NCNetworkAddRequestMapReq(const NCNetworkAddRequestMapReq&);
        NCNetworkAddRequestMapReq& operator=(const NCNetworkAddRequestMapReq&);
    };

    /**
     * @brief This class is used to pop specify request from map and reply it.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkPopRequestMapReq : public NCRunnable
    {
    public:
        NCNetworkPopRequestMapReq(const UINT32 mapType, const UINT32 reqType, const NCString& deviceType, const UINT32 result);
        virtual ~NCNetworkPopRequestMapReq();

        VOID run();
        VOID doPopRequest();

    private:
        UINT32 m_mapType;
        UINT32 m_reqType;
        NCString m_deviceType;
        UINT32 m_result;

        NCNetworkPopRequestMapReq(const NCNetworkPopRequestMapReq&);
        NCNetworkPopRequestMapReq& operator=(const NCNetworkPopRequestMapReq&);
    };

    /**
     * @brief This class is used to pop all request from map and reply it.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkPopAllRequestMapReq : public NCRunnable
    {
    public:
        NCNetworkPopAllRequestMapReq(const UINT32 mapType, const UINT32 popRequestFlag);
        virtual ~NCNetworkPopAllRequestMapReq();

        VOID run();
        VOID doPopAllRequest();

    private:
        UINT32 m_mapType;
        UINT32 m_popRequestFlag;

        NCNetworkPopAllRequestMapReq(const NCNetworkPopAllRequestMapReq&);
        NCNetworkPopAllRequestMapReq& operator=(const NCNetworkPopAllRequestMapReq&);
    };

    /**
     * @brief This class is used to check request timeout in map.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkCheckTimeoutRequestMapReq : public NCRunnable
    {
    public:
        NCNetworkCheckTimeoutRequestMapReq(const UINT32 mapType);
        virtual ~NCNetworkCheckTimeoutRequestMapReq();

        VOID run();

    private:
        UINT32 m_mapType;
        
        NCNetworkCheckTimeoutRequestMapReq(const NCNetworkCheckTimeoutRequestMapReq&);
        NCNetworkCheckTimeoutRequestMapReq& operator=(const NCNetworkCheckTimeoutRequestMapReq&);
    };

    /**
     * @brief This class is used to reply request.
     *
     * This class will be posted in NCRunnableThread, which will call the run() function.
     */
    class NCNetworkReplyRequestMapReq : public NCRunnable
    {
    public:
        NCNetworkReplyRequestMapReq(const AccessParameter& param);
        virtual ~NCNetworkReplyRequestMapReq();

        VOID run();
        VOID doReplyRequest();

    private:
        AccessParameter m_param;
        
        NCNetworkReplyRequestMapReq(const NCNetworkReplyRequestMapReq&);
        NCNetworkReplyRequestMapReq& operator=(const NCNetworkReplyRequestMapReq&);
    };
}

#endif /* NCNETWORKEVENTREQ_H */
/* EOF */