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
 * @file NCNetworkPlugin.h
 * @brief Declaration file of class NCNetworkPlugin.
 *
 * This file includes the declaration of class NCNetworkPlugin, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKPLUGIN_H
#define NCNETWORKPLUGIN_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCRefBase.h>
#include <ncore/NCGlobalAPI.h>
#include "networkmanager/NCNetworkDef.h"

namespace nutshell
{
    /// network plug-in callback
    class NCNetworkPluginCallback;

    /**
     * This is the base-class of network plug-in.
     *
     * All network plug-in must inherit this class.
    */
    class NCNetworkPlugin : public NCRefBase
    {
    public:
        NCNetworkPlugin() {}
        virtual ~NCNetworkPlugin() {}

        /**
         * Initialize plug-in.
         *
         * @param [IN] cb : The pointer of callback
         *
         * @return None
         *
         * @attention Synchronous I/F.
         */
        virtual VOID onInit(NCNetworkPluginCallback* cb) = 0;


        /**
         * Notify onAwake
         *
         * @param None
         * @return None
         *
         * @attention Synchronous I/F.
         */
        virtual VOID onStart() {}

        /**
         * This interface will be deleted later.
         */
        virtual VOID requestAccess(const AccessMode& mode) {}

        /**
         * Request network access.
         *
         * @param [in] mode : The struct that includes resourceID, dial type and communicate priority
         * @param [in] reqId : The sequence ID that used in callbacl result.
         * 
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID requestAccess(const AccessMode& mode, const UINT32 reqId) {}

        /**
         * This interface will be deleted later
         */
        virtual VOID releaseAccess() {}

        /**
         * Release network access.
         *
         * @param [in] reqId : The sequence ID that used in callbacl result.
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID releaseAccess(const UINT32 reqId) {}

        /**
         * Check plugin is releasing or not.
         *
         * @param None.
         *
         * @return NC_TRUE indicates the plugin is releasing. NC_FALSE indicates that is not releasing.
         *
         * @attention Synchronous I/F.
         */
        virtual NC_BOOL isReleasing()
        { 
            return NC_TRUE; 
        }

        /**
         * This interface will be deleted later
         */
        virtual VOID updateAccess(const NCString& resourceId) {}

        /**
         * Update network access.
         *
         * @param [IN] resourceId : The resource Identify
         * @param [IN] seqId : The sequence ID
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID updateAccess(const NCString& resourceId, const UINT32 seqId) {}

        /**
         * Notify DCM/DUN plugin to send callback message to DCU/MEU by avcLan.
         *
         * @param [IN] deviceType : The type of network device
         * 
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID notifyRemoteDisconnectCallback(const NCString& deviceType) {}

        /**
         * Notify DCM/DUN plugin to send request callback to DCU/MEU by avcLan.
         *
         * @param [IN] deviceType : The type of network device
         * @param [IN] result : The result of remote request access
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
         virtual VOID notifyRemoteRequestCallback(const NCString& deviceType, const UINT32 result) {}

private:
        NC_DISABLE_COPY(NCNetworkPlugin);
    };

} /* namespace nutshell */

#define NCNETWORKPLUGIN_CREATE_FUNC_NAME "nc_create_networkplugin"
#define NCNETWORKPLUGIN_CREATE(OBJ) \
    extern "C" NUTSHELL_API nutshell::NCNetworkPlugin* nc_create_networkplugin() \
    { \
        return new OBJ(); \
    }
extern"C" nutshell::NCNetworkPlugin* nc_create_networkplugin();

#endif /* NCNETWORKPLUGIN_H */
/* EOF */
