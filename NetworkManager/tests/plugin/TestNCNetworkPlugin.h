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
 * @file TestNCNetworkPlugin.h
 * @brief Declaration file of class TestNCNetworkPlugin.
 *
 * This file includes the declaration of class TestNCNetworkPlugin, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef TESTNCNETWORKPLUGIN_H
#define TESTNCNETWORKPLUGIN_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <networkplugin/NCNetworkPlugin.h>
#include <networkplugin/NCNetworkPluginCallback.h>

namespace nutshell
{
    /**
     * This class is used to test the network plugin.
     *
     * This class inherited NCNetworkPlugin, and realized the virtual function.
    */
    class TestNCNetworkPlugin : public NCNetworkPlugin
    {
    public:
        TestNCNetworkPlugin();
        virtual ~TestNCNetworkPlugin();

        virtual VOID onInit(NCNetworkPluginCallback* cb);
        virtual VOID requestAccess(const AccessMode& mode);
        virtual VOID releaseAccess();

    private:
        NCNetworkPluginCallback* m_cb;
    };

} /* namespace nutshell */

#endif /* TESTNCNETWORKPLUGIN_H */
/* EOF */
