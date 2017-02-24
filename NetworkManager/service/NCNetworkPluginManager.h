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
 * @file NCNetworkPluginManager.h
 * @brief Declaration file of class NCNetworkPluginManager.
 *
 * This file includes the declaration of class NCNetworkPluginManager, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKPLUGINMANAGER_H
#define NCNETWORKPLUGINMANAGER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <map>
#include <ncore/NCString.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCRefBase.h>
#include <NCNetworkManagerDef.h>

namespace nutshell
{
    /// network plug-in
    class NCNetworkPlugin;
    /// network plug-in callback
    class NCNetworkPluginCallback;

    /**
     * Network Plug-in Manager
     *
     * This class is used to manage network plug-in
     */
    class NCNetworkPluginManager : public NCRefBase
    {
    public:
        NCNetworkPluginManager(ncsp<NCNetworkPluginCallback>::sp cb);
        virtual ~NCNetworkPluginManager();

        /**
         * Load plug-in.
         *
         * @param None
         *
         * @return None
         *
         * @attention Synchronous I/F.
         */
        VOID onInit();

        /**
         * Notify onAwake
         *
         * @param None
         * @return None
         *
         * @attention Synchronous I/F.
         */
        VOID onStart();

        /**
         * Request network access.
         *
         * @param [IN] param : struct includes resourceId, dial type, etc. 
         *
         * @return NC_TRUE indicates success, NC_FALSE indicates fail
         *
         * @attention Synchronous I/F.
         */
        NC_BOOL requestNetworkAccess(const AccessParameter& param);
        /**
         * Release network access.
         *
         * @param [IN] device : The network device type.
         * @param [IN] seqId : The sequence ID
         *
         * @return NC_TRUE indicates success, NC_FALSE indicates fail.
         *
         * @attention Synchronous I/F.
         */
        NC_BOOL releaseNetworkAccess(const NCString& device, const UINT32 seqId);

        /**
         * Update network access.
         *
         * @param [IN] deviceType : The network device type.
         * @param [IN] resourceId : The resource Identify
         * @param [IN] seqId : The sequence ID
         *
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        virtual VOID updateNetworkAccess(const NCString& deviceType, const NCString& resourceId, const UINT32 seqId);

        /**
         * Check plugin is releasing or not.
         *
         * @param [IN] deviceType : The network device type.
         *
         * @return NC_TRUE indicates the plugin is releasing. NC_FALSE indicates that is not releasing.
         *
         * @attention Synchronous I/F.
         */
         NC_BOOL isReleasing(const NCString& deviceType);

        /**
         * Notify DCM/DUN plugin to send callback message to DCU/MEU by avcLan.
         *
         * @param [IN] deviceType : The type of network device
         * 
         * @return None.
         *
         * @attention Synchronous I/F.
         */
        VOID notifyRemoteDisconnectCallback(const NCString& deviceType);

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
         VOID notifyRemoteRequestCallback(const NCString& deviceType, const UINT32 result);

    private:
        /// Load plug-in
        VOID loadPlugin(const NCString& pluginId, const NCString& path);
        /// Unload plug-in
        VOID unloadPlugin();
        /// Read the plug-in configure
        NC_BOOL readPluginConfigFile();

        /// Struct to store plug-in
        struct PluginItem
        {
            void* handler;                  ///< The handler of plug-in
            NCNetworkPlugin* instance;      ///< The instance of plug-in

            PluginItem()
            : handler(NULL)
            , instance(NULL)
            {
            }
        };

        std::map<NCString, PluginItem> m_pluginMap;
        std::map<NCString, NCString> m_pluginConfigMap;
        ncsp<NCNetworkPluginCallback>::sp m_cb;

        NC_DISABLE_COPY(NCNetworkPluginManager)
    };

} /* namespace nutshell */

#endif /* NCNETWORKPLUGINMANAGER_H */
/* EOF */
