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

#include <ncore/NCLog.h>
#include "TestNCNetworkPlugin.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{
    TestNCNetworkPlugin::TestNCNetworkPlugin()
    : m_cb(NULL)
    {
    }

    TestNCNetworkPlugin::~TestNCNetworkPlugin()
    {
    }

    VOID
    TestNCNetworkPlugin::onInit(NCNetworkPluginCallback* cb)
    {
        NCLOGD("TestNCNetworkPlugin::onInit - Start");
        m_cb = cb;
        NCLOGD("TestNCNetworkPlugin::onInit - End");
    }

    VOID
    TestNCNetworkPlugin::requestAccess(const AccessMode& mode)
    {
        NCLOGD("TestNCNetworkPlugin::requestAccess, - Start");
    }

    VOID
    TestNCNetworkPlugin::releaseAccess()
    {
        NCLOGD("TestNCNetworkPlugin::releaseAccess - Start ");
        m_cb->notifyReleaseAccessResult(NC_NOERROR, "eth0");
    }

    NCNETWORKPLUGIN_CREATE(TestNCNetworkPlugin)
} /* namespace nutshell */
/* EOF */

