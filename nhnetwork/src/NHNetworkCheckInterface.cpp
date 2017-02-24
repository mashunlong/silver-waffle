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
#include "NHNetworkCheckInterface.h"

#define NC_LOG_TAG "NHNetworkHandler"
#define NHNETWORK_PROC_NET_DEV "/proc/net/dev"

namespace nutshell
{

    NHNetworkCheckInterface::NHNetworkCheckInterface()
    {

    }

    NHNetworkCheckInterface::~NHNetworkCheckInterface()
    {

    }

    NC_BOOL
    NHNetworkCheckInterface::interfaceExist(const NCString& interfaceName)
    {
        NCLOGD("NHNetworkCheckInterface::interfaceExist, interface name [%s]", interfaceName.getString());
        FILE* fp;
        NCCHAR buf[1024] = { 0 };

        fp = fopen(NHNETWORK_PROC_NET_DEV, "r");
        if (NULL == fp) {
            NCLOGD("NHNetworkCheckInterface::interfaceExist, open file failed");
            return NC_FALSE;
        }

        // Drop header lines
        fgets(buf, sizeof(buf) -1, fp);
        fgets(buf, sizeof(buf) -1, fp);

        while (NULL != fgets(buf, sizeof(buf) -1, fp)) {
            NCString name = getName(buf);
            if (name == interfaceName) {
                fclose(fp);
                return NC_TRUE;
            }
        }

        fclose(fp);
        return NC_FALSE;
    }

    NCCHAR*
    NHNetworkCheckInterface::getName(NCCHAR* buf)
    {
        while (*buf && isspace(*buf)) {
            ++buf;
        }

        NCCHAR* find = strrchr(buf, ':');
        *find = '\0';

        return buf;
    }

} /* namespace nutshell */
/* EOF */