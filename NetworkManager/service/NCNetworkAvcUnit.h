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
 * @file NCNetworkAvcUnit.h
 * @brief Declaration file of class NCNetworkAvcUnit.
 *
 * This file includes the declaration of class NCNetworkAvcUnit, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */

#ifndef NCNETWORKAVCUNIT_H
#define NCNETWORKAVCUNIT_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <map>
#include <vector>
#include <ncore/NCGlobalAPI.h>
#include <ncore/NCTypesDefine.h>
#include <ncore/NCString.h>
#include <ncore/NCSyncObj.h>
#include <nceventsys/NCRunnableLooper.h>
#include <evipservice/NEAvcUnitIF.h>

namespace nutshell
{
    class NEAvcUnit;

    /**
     * Class NCNetworkAvcUnit
     *
     * Ext.AVC-LAN Unit Class
     */
    class NCNetworkAvcUnit : public NEAvcUnitIF
    {
    public:
        NCNetworkAvcUnit();
        virtual ~NCNetworkAvcUnit();

        VOID start(const NCRunnableLooper&  looper);
        VOID stop();

        // Override form NEAvcUnitIF
        virtual VOID onRecvExtData(NCString &filter, NCData &data);
        virtual VOID onBindExtChannel();
        virtual VOID onUnbindExtChannel();

        /// Notify DCU/MEU the available communication device and default route
        VOID notifyCommStatusChange();
        
        VOID setFlag(NC_BOOL flag);
        NC_BOOL getFlag();

    private:
        UINT32 getCommDevice();
        UINT32 getCommMode();
        INT8 getWifiGStationStatus();
        INT8 getMirrorLinkStatus();
        VOID getDunCarrierInfo(std::vector<INT8>& info);
        VOID getDcmDnsInfo(std::vector<INT8>& dns1, std::vector<INT8>& dns2);

        VOID updateCommDevice(const UINT32 commDevice);

        NEAvcUnit* m_avcUnit;
        
        NC_BOOL m_flag;
        NCSyncObj m_syncFlag;

        NCSyncObj m_syncDcuMeuNotify;

        NCNetworkAvcUnit(const NCNetworkAvcUnit &);
        NCNetworkAvcUnit &operator=(const NCNetworkAvcUnit &);
    };

} /* namespace nutshell */

#endif /* NCNETWORKAVCUNIT_H */
/* EOF */
