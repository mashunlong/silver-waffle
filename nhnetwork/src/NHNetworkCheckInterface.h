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
 * @file NHNetworkCheckInterface.h
 * @brief Declaration file of class NHNetworkCheckInterface.
 *
 * This file includes the declaration of class NHNetworkCheckInterface, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NHNETWORKCHECKINTERFACE_H
#define NHNETWORKCHECKINTERFACE_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <ncore/NCTypesDefine.h>
#include <ncore/NCString.h>

namespace nutshell
{

    /**
     * @brief NHNetworkCheckInterface
     *
     * This class is used to check the status of network interfaces
     */
    class NHNetworkCheckInterface
    {
    public:
        NHNetworkCheckInterface();
        virtual ~NHNetworkCheckInterface();

        NC_BOOL interfaceExist(const NCString& interfaceName);

    private:
        NCCHAR* getName(NCCHAR* buf);

        NHNetworkCheckInterface(const NHNetworkCheckInterface&);
        NHNetworkCheckInterface& operator=(const NHNetworkCheckInterface&);
    };

} /* namespace nutshell */

#endif /* NHNETWORKCHECKINTERFACE_H */
/* EOF */
