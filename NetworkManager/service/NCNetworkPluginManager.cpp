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

#include <dlfcn.h>
#include <ncore/NCLog.h>
#include <ncore/NCIniFile.h>
#include <ncore/NCIniFileParser.h>
#include <ncore/NCTimeDefine.h>
#include "networkmanager/NCNetworkDef.h"
#include "networkplugin/NCNetworkPlugin.h"
#include "networkplugin/NCNetworkPluginCallback.h"
#include "NCNetworkNotify.h"
#include "NCNetworkPluginManager.h"

#define NC_LOG_TAG "NCNetworkManager"

#define NCNETWORK_PLUGIN_CONFIG_FILE "/etc/networkmanager/plugin.ini"
#define NCNETWORK_PLUGIN_CONFIG_SECTION "LibPath"

namespace nutshell
{
    NCNetworkPluginManager::NCNetworkPluginManager(ncsp<NCNetworkPluginCallback>::sp cb)
    : m_pluginMap()
    , m_pluginConfigMap()
    , m_cb(cb)
    {

    }

    NCNetworkPluginManager::~NCNetworkPluginManager()
    {
        NCLOGD("NCNetworkPluginManager::~NCNetworkPluginManager - Start");
        unloadPlugin();
        NCLOGD("NCNetworkPluginManager::~NCNetworkPluginManager - End");
    }

    VOID
    NCNetworkPluginManager::onInit()
    {
        NCLOGD("NCNetworkPluginManager::onInit - Start");
        NC_BOOL ret = readPluginConfigFile();
        if (!ret) {
            NCLOGE("NCNetworkPluginManager::onInit - Read plug-in configure file failed!");
            return;
        }
        std::map<NCString, NCString>::iterator configIter = m_pluginConfigMap.begin();
        for (; configIter != m_pluginConfigMap.end(); ++configIter) {
            NCString pluginId = configIter->first;
            NCString path = configIter->second;
            loadPlugin(pluginId, path);
        }

        std::map<NCString, PluginItem>::iterator pluginIter = m_pluginMap.begin();
        while (pluginIter != m_pluginMap.end()) {
            PluginItem item = pluginIter->second;
            item.instance->onInit(m_cb.get());
            ++pluginIter;
        }
        NCLOGD("NCNetworkPluginManager::onInit - End");
    }

    VOID
    NCNetworkPluginManager::onStart()
    {
        NCLOGD("NCNetworkPluginManager::onStart - Start");
        std::map<NCString, PluginItem>::iterator pluginIter = m_pluginMap.begin();
        while (pluginIter != m_pluginMap.end()) {
            PluginItem item = pluginIter->second;
            item.instance->onStart();
            ++pluginIter;
        }
    }

    VOID
    NCNetworkPluginManager::loadPlugin(const NCString& pluginId, const NCString& path)
    {
        NCLOGD("NCNetworkPluginManager::loadPlugin- Start, pluginId = [%s], path = [%s]", pluginId.getString(), path.getString());
        void* handler = dlopen(path.getString(), RTLD_LAZY);
        if (NULL == handler) {
            NCLOGE("NCNetworkPluginManager::loadPlugin - Failed to do dlopen(%s)", dlerror());
            return;
        }
        NCNetworkPlugin*(*pCreateFunc)();
        pCreateFunc = (NCNetworkPlugin*(*)())dlsym(handler, NCNETWORKPLUGIN_CREATE_FUNC_NAME);
        if (NULL == pCreateFunc) {
            NCLOGE("NCNetworkPluginManager::loadPlugin - Failed to do dlsym(%s)", dlerror());
            dlclose(handler);
            return;
        }

        PluginItem item;
        item.handler = handler;
        item.instance = pCreateFunc();
        m_pluginMap.insert(std::map<NCString, PluginItem>::value_type(pluginId, item));
        NCLOGD("NCNetworkPluginManager::loadPlugin - End");
    }

    VOID
    NCNetworkPluginManager::unloadPlugin()
    {
        NCLOGD("NCNetworkPluginManager::unloadPlugin - Start");
        std::map<NCString, PluginItem>::iterator pluginIter = m_pluginMap.begin();
        while (pluginIter != m_pluginMap.end()) {
            PluginItem item = pluginIter->second;
            if (NULL != item.instance) {
                delete item.instance;
                item.instance = NULL;
            }
            dlclose(item.handler);
            pluginIter = m_pluginMap.erase(pluginIter);
        }
        NCLOGD("NCNetworkPluginManager::unloadPlugin - End");
    }

    NC_BOOL
    NCNetworkPluginManager::requestNetworkAccess(const AccessParameter& param)
    {
        NCLOGD("NCNetworkPluginManager::requestNetworkAccess, Start");
        NCLOGD("deviceType [%s], resourceId [%s] dialType [%d], commPriority [%d], reqId [%d]", param.deviceType.getString(), 
            param.resourceId.getString(), param.dealType, param.commPriority, param.reqId);

        std::map<NCString, PluginItem>::iterator pluginIter = m_pluginMap.find(param.deviceType);
        if (pluginIter == m_pluginMap.end()) {
            NCLOGE("NCNetworkPluginManager::requestNetworkAccess - Failed find the specified plug-in!");
            return NC_FALSE;
        }

        PluginItem item;
        item = pluginIter->second;

        AccessMode mode;
        mode.type = (DialType)param.dealType;
        mode.priority = (CommunicatePriority)param.commPriority;
        mode.resourceId = param.resourceId;
        NCLOGD("NCNetworkPluginManager::requestNetworkAccess, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        item.instance->requestAccess(mode, param.reqId);
        return NC_TRUE;
    }

    NC_BOOL
    NCNetworkPluginManager::releaseNetworkAccess(const NCString& device, const UINT32 seqId)
    {
        NCLOGD("NCNetworkPluginManager::releasectNetworkAccess, Start, device [%s], seqId [%d]", device.getString(), seqId);
        std::map<NCString, PluginItem>::iterator pluginIter = m_pluginMap.find(device);
        if (pluginIter == m_pluginMap.end()) {
            NCLOGE("NCNetworkPluginManager::releasectNetworkAccess - Failed find the specified plug-in!");
            return NC_FALSE;
        }
        PluginItem item;
        item = pluginIter->second;
        NCLOGD("NCNetworkPluginManager::releaseNetworkAccess, performance, [TC: %d TT: %d]", GetTickCount(), GetThreadTime());
        item.instance->releaseAccess(seqId);
        return NC_TRUE;
    }

    VOID
    NCNetworkPluginManager::updateNetworkAccess(const NCString& deviceType, const NCString& resourceId, const UINT32 seqId)
    {
        NCLOGD("NCNetworkPluginManager::updateNetworkAccess, Start, deviceType [%s], resourceId [%s], seqId [%d]", deviceType.getString(), 
            resourceId.getString(), seqId);
        std::map<NCString, PluginItem>::iterator pluginIter = m_pluginMap.find(deviceType);
        if (pluginIter == m_pluginMap.end()) {
            NCLOGE("NCNetworkPluginManager::updateNetworkAccess - Failed find plug-in!");
            return;
        }

        PluginItem item;
        item = pluginIter->second;
        item.instance->updateAccess(resourceId, seqId);
    }

    NC_BOOL
    NCNetworkPluginManager::isReleasing(const NCString& deviceType)
    {
        NCLOGD("NCNetworkPluginManager::isReleasing - Start, deviceType [%s]", deviceType.getString());
        std::map<NCString, PluginItem>::iterator pluginIter = m_pluginMap.find(deviceType);
        if (pluginIter == m_pluginMap.end()) {
            NCLOGE("NCNetworkPluginManager::isReleasing, Bug: Find specified plug-in failed");
            return NC_TRUE;
        }

        PluginItem item;
        item = pluginIter->second;
        return item.instance->isReleasing();
    }

    VOID
    NCNetworkPluginManager::notifyRemoteDisconnectCallback(const NCString& deviceType)
    {
        NCLOGD("NCNetworkPluginManager::notifyRemoteDisconnectCallback, Start, deviceType [%s]", deviceType.getString());
        std::map<NCString, PluginItem>::iterator pluginIter = m_pluginMap.find(deviceType);
        if (pluginIter == m_pluginMap.end()) {
            NCLOGE("NCNetworkPluginManager::notifyRemoteDisconnectCallback - Failed find plug-in!");
            return;
        }

        PluginItem item;
        item = pluginIter->second;
        item.instance->notifyRemoteDisconnectCallback(deviceType);
    }

    VOID
    NCNetworkPluginManager::notifyRemoteRequestCallback(const NCString& deviceType, const UINT32 result)
    {
        NCLOGD("NCNetworkPluginManager::notifyRemoteRequestCallback, Start, deviceType [%s], result [%d]", deviceType.getString(), result);
        std::map<NCString, PluginItem>::iterator pluginIter = m_pluginMap.find(deviceType);
        if (pluginIter == m_pluginMap.end()) {
            NCLOGE("NCNetworkPluginManager::notifyRemoteRequestCallback - Failed find plug-in!");
            return;
        }

        PluginItem item;
        item = pluginIter->second;
        item.instance->notifyRemoteRequestCallback(deviceType, result);
    }

    NC_BOOL
    NCNetworkPluginManager::readPluginConfigFile()
    {
        NCLOGD("NCNetworkPluginManager::readPluginConfigFile - Start");
        NCIniFile iniFileOperator;
        NC_BOOL ret = iniFileOperator.open(NCNETWORK_PLUGIN_CONFIG_FILE, NCINI_OM_R);
        if (!ret) {
            NCLOGE("NCNetworkPluginManager::readPluginConfigFile - Open plug-in configure file failed!");
            return ret;
        }

        NCIniIterator iter = iniFileOperator.iterator(NCNETWORK_PLUGIN_CONFIG_SECTION);
        for (; !iter.end(); ++iter) {
            NCIniKey key;
            if (iter.current(key)) {
                NCString keyName = key.name();
                NCString keyValue = key.value();
                NCLOGD("NCNetworkPluginManager::readPluginConfigFile - LibPath: [%s] = [%s]", keyName.getString(), keyValue.getString());
                m_pluginConfigMap.insert(std::map<NCString, NCString>::value_type(keyName, keyValue));
            }
        }

        ret = iniFileOperator.close();
        if (!ret) {
            NCLOGE("NCNetworkPluginManager::readPluginConfigFile - Close plug-in configure file failed!");
            return ret;
        }

        return ret;
    }

} /* namespace nutshell */
/* EOF */
