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
 * @file TestNCNetworkAccess.h
 * @brief Declaration file of class TestNCNetworkAccess.
 *
 * This file includes the declaration of class TestNCNetworkAccess, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef TESTNCNETWORKACCESS_H
#define TESTNCNETWORKACCESS_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCRefBase.h>
#include "networkmanager/NCNetworkAccess.h"


namespace nutshell
{
    /**
     * @brief This class is used to test the interfaces in NCNetworkAccess.h
     *
     * This class tests the interfaces in NCNetworkAccess by google test.
     */
    class TestNCNetworkAccess : public virtual NCRefBase
    {
    public:
        TestNCNetworkAccess();
        virtual ~TestNCNetworkAccess();

        VOID onInit();
        VOID onDeinit();

        static TestNCNetworkAccess* Instance();
        NCNetworkAccess* proxy();
        
    private:
        static TestNCNetworkAccess* instance;
        ncsp<NCNetworkAccess>::sp m_proxy;

        TestNCNetworkAccess(const TestNCNetworkAccess&);
        TestNCNetworkAccess& operator=(const TestNCNetworkAccess&);
    };

} /* namespace nutshell */

#endif /* TESTNCNETWORKACCESS_H */
/* EOF */
