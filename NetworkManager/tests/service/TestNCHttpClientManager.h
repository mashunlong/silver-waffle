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
 * @file TestNCHttpClientManager.h
 * @brief Declaration file of class TestNCHttpClientManager.
 *
 * This file includes the declaration of class TestNCHttpClientManager, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef TESTNCHTTPCLIENTMANAGER_H
#define TESTNCHTTPCLIENTMANAGER_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCRefBase.h>
#include "networkmanager/NCHttpClientManager.h"


namespace nutshell
{
    /**
     * @brief This class is used to test the interfaces in NCNetworkAccess.h
     *
     * This class tests the interfaces in NCNetworkAccess by google test.
     */
    class TestNCHttpClientManager : public virtual NCRefBase
    {
    public:
        TestNCHttpClientManager();
        virtual ~TestNCHttpClientManager();

        VOID onInit();
        VOID onDeinit();

        static TestNCHttpClientManager* Instance();
        ncsp<NCHttpClientManager>::sp proxy();

    private:
        static TestNCHttpClientManager* instance;

        TestNCHttpClientManager(const TestNCHttpClientManager&);
        TestNCHttpClientManager& operator=(const TestNCHttpClientManager&);
    };

} /* namespace nutshell */

#endif /* TESTNCHTTPCLIENTMANAGER_H */
/* EOF */
