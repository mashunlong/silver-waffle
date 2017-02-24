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
#include <ncore/NCError.h>
#include <NHAvcProtocolDef.h>
#include <evipservice/NEAvcFilter.h>
#include <evipservice/NEAvcUnit.h>
#include <NDAvcProtocolData_generated.h>
#include <NDAvcNetworkManager_generated.h>
#include <system/NCEnvironment.h>
#include "NCNetworkStatus.h"
#include "NCNetworkHandler.h"
#include "NCNetworkManager.h"
#include "NCNetworkNotify.h"
#include "NCNetworkAccessStack.h"
#include "NCNetworkReqConfirm.h"
#include "NCNetworkPluginManager.h"
#include "NCNetworkThread.h"
#include "NCNetworkEventReq.h"
#include "NCNetworkAvcUnit.h"

namespace nutshell
{
#define NC_LOG_TAG "NCNetworkManager"

/// Define the global array for communication device type and their reference number
#define DeviceNumber 2
char* DeviceArray[DeviceNumber] = { "dcm", "dsrc" };
UINT32 DeviceReference[DeviceNumber] = { 33554432, 67108864 };

#define ModeNumber 5
char* Mode[5] = { "btDun", "btPan", "wifiSta", "dcm", "dsrc" };
UINT32 ModeReference[5] = { 64, 128, 65536, 33554432, 67108864 };

    static const UINT32 s_extAddrList[] = {
        NHAVC_UINIT_NETWORK_MANAGER2,
        -1U,
    };

    static const NEAvcFilter::Item s_extFilterList[] = {
        { NHAVC_UINIT_NETWORK_MANAGER1, NHAVC_UINIT_NETWORK_MANAGER2, NHNetworkManagerOpcCode_NotifyCommStatusChange },
        { -1U, -1U, -1U },
    };

    VOID
    NCNetworkAvcUnit::start(const NCRunnableLooper&  looper)
    {
        NCLOGD("NCNetworkAvcUnit::start - Start");
        // start avc unit
        if (NULL != m_avcUnit) {
            NCLOGD("NCNetworkAvcUnit::start - Start avc unit");
            m_avcUnit->start(s_extAddrList, s_extFilterList, looper, NEAvcUnit::Channel_Ext);
        }
        NCLOGD("NCNetworkAvcUnit::start - End");
    }

    VOID
    NCNetworkAvcUnit::onRecvExtData(NCString &filter, NCData &data)
    {
        NCLOGD("NCNetworkAvcUnit::onRecvExtData,Start");
        flatbuffers::Verifier verifier(reinterpret_cast<uint8_t*>(data.getData()), data.getSize());
        if (!verifier.VerifyBuffer<extavc::NDAvcProtocolData>()) {
            NCLOGD("NCNetworkAvcUnit::onRecvExtData - Verify NDAvcProtocolData failed");
            return;
        }

        const extavc::NDAvcProtocolData* protocolData = flatbuffers::GetRoot<extavc::NDAvcProtocolData>(data.getData());
        if (NULL == protocolData) {
            NCLOGD("NCNetworkAvcUnit::onRecvExtData - GetRoot NDAvcProtocolData failed");
            return;
        }

        UINT32 src = protocolData->head()->logicalSrc();
        UINT32 dst = protocolData->head()->logicalDst();
        UINT32 opc = protocolData->head()->opc();

        if (NHAVC_UINIT_NETWORK_MANAGER2 == dst  && NHNetworkManagerOpcCode_NotifyCommStatusChange == opc) {
            if ((NULL == protocolData->data()->Data()) || (0 == protocolData->data()->Length())) {
                NCLOGD("NCNetworkAvcUnit::onRecvExtData - NDAvcProtocolData data error");
                return;
            }

            flatbuffers::Verifier verifierNDNotifyCommStatusChange(protocolData->data()->Data(), protocolData->data()->Length());
            if (!verifierNDNotifyCommStatusChange.VerifyBuffer<extavc::NDNotifyCommStatusChange>()) {
                NCLOGD("NCNetworkAvcUnit::onRecvExtData - Verify NDNotifyCommStatusChange failed");
                return;
            }

            const extavc::NDNotifyCommStatusChange* statusChange = flatbuffers::GetRoot<extavc::NDNotifyCommStatusChange>(protocolData->data()->Data());
            if (NULL == statusChange) {
                NCLOGD("NCNetworkAvcUnit::onRecvExtData - Getroot NDNotifyCommStatusChange failed");
                return;
            }

            NCNetworkStatus* myStatus = NCNetworkManager::Instance()->getNetworkStatus();
            NCNetworkThread* myWorkThread = NCNetworkManager::Instance()->getNetworkThread();
            if (NULL == myStatus || NULL == myWorkThread) {
                NCLOGD("NCNetworkAvcUnit::onRecvExtData, Get pointer failed");
                NCErrorLog(NetworkErrCode_PointerIsNULL, 0);
                return;
            }

            UINT32 commDevice = statusChange->commDevice();
            updateCommDevice(commDevice);
            NCLOGD("NCNetworkAvcUnit::onRecvExtData - commDevice [%d], commMode [%d]", commDevice, statusChange->routeStatus());

            INT8 wifiGStation = statusChange->wifiSTAType();
            INT8 mirrorLinkStatus = statusChange->mirrorlinkStatus();
            NCLOGD("NCNetworkAvcUnit::onRecvExtData - wifiGStation [%x], mirrorLinkStatus [%x]", wifiGStation, mirrorLinkStatus);

            INT8 carrierInfo[NCNETWORK_BTDUN_CARRIER_INFO_LENGTH] = { 0 };
            for (INT i = 0; i < NCNETWORK_BTDUN_CARRIER_INFO_LENGTH; ++i) {
                carrierInfo[i] = statusChange->carrierInfo()->Get(i);
            }

            NCCHAR charCarrierInfo[NCNETWORK_BTDUN_CARRIER_INFO_LENGTH + 1] = { 0 };
            memcpy(charCarrierInfo, carrierInfo, sizeof(charCarrierInfo) - 1);
            NCLOGD("NCNetworkAvcUnit::onRecvExtData - carrierInfo [%s]", charCarrierInfo);

            NCNetworkNotify notify;
            NCNetworkHandler handler;
            NCNetworkReqConfirm reqConfirm;
            NCString currNetwork = myStatus->currentNetwork();

            switch (statusChange->routeStatus()) {
            case extavc::NDNetworkCommMode_Undefine:
            {
                if (myWorkThread->lockReqState(RequestType_ClearDefaultNetwork)) {
                    NCString clearType = reqConfirm.getDeviceType(RequestType_ClearDefaultNetwork);
                    UINT32 lastRequest = reqConfirm.getLastRequest(RequestType_ClearDefaultNetwork);
                    NC_BOOL remoteFlag = reqConfirm.getRemoteFlag(RequestType_ClearDefaultNetwork);
                    NCLOGD("NCNetworkAvcUnit::onRecvExtData, RequestType_ClearDefaultNetwork, Start");
                    NCLOGD("NCNetworkAvcUnit::onRecvExtData, Clear type [%s], Last request [%d], remote flag[%d]", clearType.getString(), lastRequest, remoteFlag);
                    if (DCM == clearType) {
                        myStatus->setDialState(clearType, DialState_Disconnected, DialType_None);
                        NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Init);
                        popAllReq.doPopAllRequest();
                        reqConfirm.confirm();
                        myWorkThread->unlockReqState(RequestType_ClearDefaultNetwork);
                        break;
                    }
                    
                    myWorkThread->unlockReqState(RequestType_ClearDefaultNetwork);
                }

                if (WIFI_STA == currNetwork || BT_DUN == currNetwork) {
                    NCNetworkManager::Instance()->postRequest(new NCNetworkClearDefaultNetworkReq(RequestType_Init, currNetwork, NC_FALSE));
                    break;
                }

                break;
            }
            case extavc::NDNetworkCommMode_BTDUN:
            {
                if (NCString("") != currNetwork || getFlag()) {
                    break;
                }
                else {
                    setFlag(NC_TRUE);
                }
                
                DeviceRouteDnsInfo routeInfo;
                handler.getUsbRouteDnsInfo(BT_DUN, routeInfo);

                NCNetworkManager::Instance()->postRequest(new NCNetworkSetDefaultNetworkReq(RequestType_Init, routeInfo));
                break;
            }
            case extavc::NDNetworkCommMode_WFSTA:
            {
                if (NCString("") != currNetwork || getFlag()) {
                    break;
                }
                else {
                    setFlag(NC_TRUE);
                }

                DeviceRouteDnsInfo routeInfo;
                handler.getUsbRouteDnsInfo(WIFI_STA, routeInfo);

                // Request wifi from meu
                if (myWorkThread->lockReqState(RequestType_Access)) {
                    NCString accessType = reqConfirm.getDeviceType(RequestType_Access);
                    if (WIFI_STA == accessType) {
                        NCLOGD("NCNetworkAvcUnit::onRecvExtData, WIFI_STA, Start set default route");
                        NCNetworkManager::Instance()->insertRequest(new NCNetworkSetDefaultNetworkReq(RequestType_Access, routeInfo));
                        reqConfirm.confirm();
                        myWorkThread->unlockReqState(RequestType_Access);
                        break;
                    }

                    myWorkThread->unlockReqState(RequestType_Access);
                }

                // Request wifi from dcu
                NCNetworkManager::Instance()->postRequest(new NCNetworkSetDefaultNetworkReq(RequestType_Init, routeInfo));
                break;
            }
            case extavc::NDNetworkCommMode_DCM:
            {
                if (myWorkThread->lockReqState(RequestType_SetDefaultNetwork)) {
                    NCLOGD("NCNetworkAvcUnit::onRecvExtData, Confirm DCM, Reply NotifyRequestAccessResult");
                    NCString setType = reqConfirm.getDeviceType(RequestType_SetDefaultNetwork);
                    UINT32 lastRequest = reqConfirm.getLastRequest(RequestType_SetDefaultNetwork);
                    NCLOGD("NCNetworkAvcUnit::onRecvExtData, Confirm DCM, Set type [%s], last request [%d]", setType.getString(), lastRequest);

                    if (DCM == setType && RequestType_Access == lastRequest) {
                        myStatus->setDialState(setType, DialState_Connected, DialType_None);
                        NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Init);
                        popAllReq.doPopAllRequest();
                        reqConfirm.confirm();
                    }

                    myWorkThread->unlockReqState(RequestType_SetDefaultNetwork);
                }

                break;
            }
            default:
            {
                break;
            }
            }
        }
    }

    VOID
    NCNetworkAvcUnit::notifyCommStatusChange()
    {
        NCAutoSync lock(m_syncDcuMeuNotify);
        NCLOGD("NCNetworkAvcUnit::notifyCommStatusChange, Start");
        flatbuffers::FlatBufferBuilder fbb1;
        extavc::NDNotifyCommStatusChangeBuilder oob(fbb1);
        UINT32 commDevice = getCommDevice();
        UINT32 commMode = getCommMode();
        INT8 wifiSTAType = getWifiGStationStatus();
        INT8 mirrorlinkStatus = getMirrorLinkStatus();

        std::vector<INT8> tmpVecType;
        getDunCarrierInfo(tmpVecType);

        std::vector<INT8> vecDns1;
        std::vector<INT8> vecDns2;
        getDcmDnsInfo(vecDns1, vecDns2);
        
        oob.add_commDevice(commDevice);
        oob.add_routeStatus(commMode);
        oob.add_wifiSTAType(wifiSTAType);
        oob.add_mirrorlinkStatus(mirrorlinkStatus);
        oob.add_carrierInfo(fbb1.CreateVector(tmpVecType));
        oob.add_dns1(fbb1.CreateVector(vecDns1));
        oob.add_dns2(fbb1.CreateVector(vecDns2));

        fbb1.Finish(oob.Finish());
        std::string network_data((char*)fbb1.GetBufferPointer(), fbb1.GetSize());

        // construct NDAvcProtocolData
        flatbuffers::FlatBufferBuilder fbb;
        extavc::NDAvcProtocolDataBuilder pdb(fbb);
        pdb.add_head(extavc::CreateNDAvcDataHeader(fbb,
            NHNetworkManagerOpcCode_NotifyCommStatusChange,
            NHAVC_UINIT_NETWORK_MANAGER2, NHAVC_UINIT_NETWORK_MANAGER1, extavc::NDAvcCommandType_PointToPoint, 0, 0));

        pdb.add_data(fbb.CreateString(network_data.c_str(), network_data.size()));
        fbb.Finish(pdb.Finish());

        NCData outData((char*)fbb.GetBufferPointer(), fbb.GetSize());
        if (0 != m_avcUnit->sendExtData(outData)) {
            NCLOGD("NCNetworkAvcUnit::notifyCommStatusChange - Send ext data failed");
        }
    }

    UINT32
    NCNetworkAvcUnit::getCommDevice()
    {
        UINT32 commDevice = 0;
        NCNetworkStatus* status = NCNetworkManager::Instance()->getNetworkStatus();
        for (UINT32 i = 0; i < DeviceNumber; i++) {
            NCString deviceType = DeviceArray[i];
            if (status->status(deviceType)) {
                commDevice = commDevice | DeviceReference[i];
            }
        }

        NCLOGD("NCNetworkAvcUnit::getCommDevice, Comm device [%d]", commDevice);
        return commDevice;
    }

    UINT32
    NCNetworkAvcUnit::getCommMode()
    {
        UINT32 commMode = 0;
        NCNetworkStatus* status = NCNetworkManager::Instance()->getNetworkStatus();
        NCString route = status->currentNetwork();
        for (UINT32 i = 0; i < ModeNumber; i++) {
            if (route == NCString(Mode[i])) {
                commMode = ModeReference[i];
                break;
            }
        }

        NCLOGD("NCNetworkAvcUnit::getCommDevice, Comm mode [%d]", commMode);
        return commMode;
    }

    INT8
    NCNetworkAvcUnit::getWifiGStationStatus()
    {
        return 0x00;
    }

    INT8
    NCNetworkAvcUnit::getMirrorLinkStatus()
    {
        return 0x00;
    }

    VOID
    NCNetworkAvcUnit::getDunCarrierInfo(std::vector<INT8>& info)
    {
        for (INT i = 0; i < 16; ++i) {
            info.push_back(0x00);
        }
    }

    VOID
    NCNetworkAvcUnit::getDcmDnsInfo(std::vector<INT8>& dns1, std::vector<INT8>& dns2)
    {
        NCLOGD("NCNetworkAvcUnit::getDcmDnsInfo, Start");
        NCString tempDns1, tempDns2;
        if (ModeReference[3] != getCommMode()) {
            tempDns1 = "0.0.0.0";
            tempDns2 = "0.0.0.0";
        }
        else {
            NCEnvironment env;
            NCString country;
            env.getProductCountry(country);
            NCLOGD("NCNetworkAvcUnit::getDcmDnsInfo, country [%s]", country.getString());

            if (NCString("America") == country) {
                NCNetworkManager::Instance()->getNetworkStatus()->dcmDnsInfo(tempDns1, tempDns2);
                if (NCString("") == tempDns1) {
                    tempDns1 = "0.0.0.0";
                }

                if (NCString("") == tempDns2) {
                    tempDns2 = "0.0.0.0";
                } 
            }
            else {
                tempDns1 = "0.0.0.0";
                tempDns2 = "0.0.0.0";
            }
        }


        // Convert dns from string to array
        NCCHAR arrayDns1[4] = { 0 };
        NCCHAR arrayDns2[4] = { 0 };
        NCNetworkHandler handler;
        handler.convertIpStringToArray(tempDns1, arrayDns1);
        handler.convertIpStringToArray(tempDns2, arrayDns2);

        INT8 intArrayDns1[4] = { 0x00 };
        INT8 intArrayDns2[4] = { 0x00 };
        memcpy(intArrayDns1, arrayDns1, sizeof(intArrayDns1));
        memcpy(intArrayDns2, arrayDns2, sizeof(intArrayDns2));

        for (INT i = 0; i < 4; ++i) {
            dns1.push_back(intArrayDns1[i]);
            dns2.push_back(intArrayDns2[i]);
        }
    }

    VOID
    NCNetworkAvcUnit::updateCommDevice(const UINT32 commDevice)
    {
        NCLOGD("NCNetworkAvcUnit::updateCommDevice, Start, commDevice [%d]", commDevice);
        NCNetworkStatus* status = NCNetworkManager::Instance()->getNetworkStatus();
        
        char* device[14] = { "btHfp", "btAvp", "btPbap", "btOpp", "btMap", "btSpp", "btDun",
                "btPan", "btSdp", "btDip", "wifiSta", "wifiP2p", "wifiAp", "usb" };
        UINT32 reference[14] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 65536, 131072, 262144, 16777216 };

        for (UINT32 i = 0; i < 14; i++) {
            NCString deviceType = device[i];
            if (commDevice & reference[i]) {
                status->setStatus(deviceType, NC_TRUE);
            }
            else {
                status->setStatus(deviceType, NC_FALSE);
            }
        }
    }

} /* namespace nutshell */
/* EOF */
