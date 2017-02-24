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

#include <ncore/NCLog.h>
#include "NCNetworkManager.h"
#include "NCNetworkStatus.h"
#include "NCNetworkUsbGadgetnetReplier.h"

#define NC_LOG_TAG "NCNetworkManager"

namespace nutshell
{

    NCNetworkUsbGadgetnetReplier::NCNetworkUsbGadgetnetReplier()
    {

    }

    NCNetworkUsbGadgetnetReplier::~NCNetworkUsbGadgetnetReplier()
    {

    }

    VOID
    NCNetworkUsbGadgetnetReplier::onConnected(const NCString& id)
    {
        NCLOGD("NCNetworkUsbGadgetnetReplier::onConnected, Start, id [%s]", id.getString());
        NCNetworkManager::Instance()->getNetworkStatus()->setMirrorLinkStatus(NC_TRUE);

    }

    VOID
    NCNetworkUsbGadgetnetReplier::onDisconnected(const NCString& id)
    {
        NCLOGD("NCNetworkUsbGadgetnetReplier::onDisconnected, Start, id [%s]", id.getString());
        NCNetworkManager::Instance()->getNetworkStatus()->setMirrorLinkStatus(NC_FALSE);
    }

} /* namespace nutshell */
/* EOF */
