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
 * @file TestNCNetworkData.h
 * @brief Declaration file of class TestNCNetworkData.
 *
 * This file includes the declaration of class TestNCNetworkData, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef TESTNCNETWORKDATA_H
#define TESTNCNETWORKDATA_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCRefBase.h>
#include "networkmanager/NCNetworkData.h"


namespace nutshell
{
    /**
     * @brief This class is used to test the interfaces in NCNetworkData.h
     *
     * This class tests the interfaces in NCNetworkData.h by google test.
     */
    class TestNCNetworkData : public virtual NCRefBase
    {
    public:
        TestNCNetworkData();
        virtual ~TestNCNetworkData();

        VOID onInit();
        VOID onDeinit();
        
        static TestNCNetworkData* Instance();
        NCNetworkData* proxy();
        
    private:
        static TestNCNetworkData* instance;
        ncsp<NCNetworkData>::sp m_proxy;

        TestNCNetworkData(const TestNCNetworkData&);
        TestNCNetworkData& operator=(const TestNCNetworkData&);
    };

} /* namespace nutshell */

#endif /* TESTNCNETWORKDATA_H */
/* EOF */
