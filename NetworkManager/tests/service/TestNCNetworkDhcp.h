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
 * @file TestNCNetworkDhcp.h
 * @brief Declaration file of class TestNCNetworkDhcp.
 *
 * This file includes the declaration of class TestNCNetworkDhcp, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef TESTNCNETWORKDHCP_H
#define TESTNCNETWORKDHCP_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCRefBase.h>
#include "networkmanager/NCNetworkDhcp.h"

namespace nutshell
{
    /**
     * @brief This class is used to test the interfaces in NCNetworkDhcp.h
     *
     * This class tests the interfaces in NCNetworkDhcp.h by google test.
     */
    class TestNCNetworkDhcp : public virtual NCRefBase
    {
    public:
        TestNCNetworkDhcp();
        virtual ~TestNCNetworkDhcp();

        VOID onInit();
        VOID onDeinit();
        
        static TestNCNetworkDhcp* Instance();
        NCNetworkDhcp* proxy();
        
    protected:
        static TestNCNetworkDhcp* instance;
        ncsp<NCNetworkDhcp>::sp m_proxy;

        TestNCNetworkDhcp(const TestNCNetworkDhcp&);
        TestNCNetworkDhcp& operator=(const TestNCNetworkDhcp&);
        
    };

} /* namespace nutshell */

#endif /* TESTNCNETWORKDHCP_H */
/* EOF */
