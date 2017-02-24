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
#include <nhinterface/NINetworkHandler.h>
#include <NDAvcProtocolData_generated.h>
#include <NDAvcNetworkManager_generated.h>
#include "NCNetworkStatus.h"
#include "NCNetworkHandler.h"
#include "NCNetworkManager.h"
#include "NCNetworkNotify.h"
#include "NCNetworkReqConfirm.h"
#include "NCNetworkAccessStack.h"
#include "NCNetworkPluginManager.h"
#include "NCNetworkThread.h"
#include "NCNetworkEventReq.h"
#include "NCNetworkAvcUnit.h"

namespace nutshell
{
#define NC_LOG_TAG "NCNetworkManager"

/// Define the global array for communication device type and their reference number
#define DeviceNumber 14
char* DeviceArray[DeviceNumber] = { "btHfp", "btAvp", "btPbap", "btOpp", "btMap", "btSpp", "btDun", "btPan", "btSdp", 
"btDip", "wifiSta", "wifiP2p", "wifiAp", "usb" };
UINT32 DeviceReference[DeviceNumber] = {  1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 65536, 131072, 262144, 16777216 };

#define ModeNumber 5
char* Mode[5] = { "btDun", "btPan", "wifiSta", "dcm", "dsrc" };
UINT32 ModeReference[5] = { 64, 128, 65536, 33554432, 67108864 };

    static const UINT32 s_extAddrList[] = {
        NHAVC_UINIT_NETWORK_MANAGER1,
        -1U,
    };

    static const NEAvcFilter::Item s_extFilterList[] = {
        { NHAVC_UINIT_NETWORK_MANAGER2, NHAVC_UINIT_NETWORK_MANAGER1, NHNetworkManagerOpcCode_NotifyCommStatusChange },
        { -1U, -1U, -1U },
    };

    VOID
    NCNetworkAvcUnit::start(const NCRunnableLooper&  looper)
    {
        NCLOGD("NCNetworkAvcUnit::start - Start");
        // start avc unit
        if (NULL != m_avcUnit) {
            m_avcUnit->start(s_extAddrList, s_extFilterList, looper, NEAvcUnit::Channel_Ext);
        }
    }

    VOID
    NCNetworkAvcUnit::onRecvExtData(NCString &filter, NCData &data)
    {
        NCLOGD("NCNetworkAvcUnit::onRecvExtData, Start");
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
        if (NHAVC_UINIT_NETWORK_MANAGER1 == dst  && NHNetworkManagerOpcCode_NotifyCommStatusChange == opc) {
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

            NCString currNetwork = myStatus->currentNetwork();
            NCLOGD("NCNetworkAvcUnit::onRecvExtData, commDevice [%d], commMode [%d]", commDevice, statusChange->routeStatus());

            NCNetworkHandler handler;
            NCNetworkNotify notify;
            NCNetworkReqConfirm reqConfirm;
            switch (statusChange->routeStatus()) {
            case extavc::NDNetworkCommMode_Undefine:
            {
                if (myWorkThread->lockReqState(RequestType_ClearDefaultNetwork)) {
                    NCLOGD("NCNetworkAvcUnit::onRecvExtData, RequestType_ClearDefaultNetwork");
                    NCString clearType = reqConfirm.getDeviceType(RequestType_ClearDefaultNetwork);
                    UINT32 lastRequest = reqConfirm.getLastRequest(RequestType_ClearDefaultNetwork);
                    NC_BOOL remoteFlag = reqConfirm.getRemoteFlag(RequestType_ClearDefaultNetwork);
                    NCLOGD("NCNetworkAvcUnit::onRecvExtData, Clear type [%s], Last request [%d], remote flag[%d]", clearType.getString(), lastRequest, remoteFlag);

                    if (BT_DUN == clearType || WIFI_STA == clearType) {
                        myStatus->setDialState(clearType, DialState_Disconnected, DialType_None);
                        NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Init);
                        popAllReq.doPopAllRequest();
                        reqConfirm.confirm();
                    }

                    myWorkThread->unlockReqState(RequestType_ClearDefaultNetwork);
                    break;
                }

                if (myWorkThread->lockReqState(RequestType_ReleaseIp)) {
                    NCString releaseType = reqConfirm.getDeviceType(RequestType_ReleaseIp);
                    NCLOGD("NCNetworkAvcUnit::onRecvExtData, RequestType_ReleaseIp, Start, releaseType [%s]", releaseType.getString());
                    if (WIFI_STA == releaseType) {
                        myStatus->setDialState(WIFI_STA, DialState_Disconnected, DialType_None);
                        notify.notifyReleaseIpResult(WIFI_STA, NetworkErrCode_Success);
                        reqConfirm.confirm();
                        myWorkThread->unlockReqState(RequestType_ReleaseIp);
                        break;
                    }

                    myWorkThread->unlockReqState(RequestType_ReleaseIp);
                }

                if (DCM == currNetwork) {
                    NCNetworkManager::Instance()->postRequest(new NCNetworkClearDefaultNetworkReq(RequestType_Init, DCM, NC_FALSE));
                    break;
                }
                
                break;
            }
            case extavc::NDNetworkCommMode_DCM:
            {
                if (NCString("") != currNetwork || getFlag()) {
                    break;
                }
                setFlag(NC_TRUE);

                INT8 arrayDns1[4] = { 0 };
                INT8 arrayDns2[4] = { 0 };
                for (INT i = 0; i < 4; ++i) {
                    arrayDns1[i] = statusChange->dns1()->Get(i);
                    arrayDns2[i] = statusChange->dns2()->Get(i);
                }

                NCCHAR charArrayDns1[4] = { 0 };
                NCCHAR charArrayDns2[4] = { 0 };
                memcpy(charArrayDns1, arrayDns1, sizeof(charArrayDns1));
                memcpy(charArrayDns2, arrayDns2, sizeof(charArrayDns2));

                NCString dns1;
                NCString dns2;
                handler.convertIpArrayToString(charArrayDns1, dns1);
                handler.convertIpArrayToString(charArrayDns2, dns2);

                DeviceRouteDnsInfo routeInfo;
                handler.getUsbRouteDnsInfo(DCM, routeInfo);

                NCLOGD("NCNetworkAvcUnit::onRecvExtData, DCM, Start config dcm route");
                NCNetworkManager::Instance()->postRequest(new NCNetworkUpdateDhcpdConfReq(dns1, dns2));
                NCNetworkManager::Instance()->postRequest(new NCNetworkSetDefaultNetworkReq(RequestType_Init, routeInfo));
                break;
            }
            case extavc::NDNetworkCommMode_BTDUN:
            {
                if (myWorkThread->lockReqState(RequestType_SetDefaultNetwork)) {
                    NCLOGD("NCNetworkAvcUnit::onRecvExtData, Confirm BT DUN, Reply NotifyRequestAccessResult");
                    NCString setType = reqConfirm.getDeviceType(RequestType_SetDefaultNetwork);
                    if (BT_DUN == setType) {
                        myStatus->setDialState(BT_DUN, DialState_Connected, DialType_None);
                        NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Init);
                        popAllReq.doPopAllRequest();
                        reqConfirm.confirm();
                    }

                    myWorkThread->unlockReqState(RequestType_SetDefaultNetwork);
                }
                break;
            }
            case extavc::NDNetworkCommMode_WFSTA:
            {
                if (myWorkThread->lockReqState(RequestType_SetDefaultNetwork)) {
                    NCLOGD("NCNetworkAvcUnit::onRecvExtData, Confirm WiFi Sta, Reply Request IP/Request access wifi");
                    NCString setType = reqConfirm.getDeviceType(RequestType_SetDefaultNetwork);
                    UINT32 lastRequest = reqConfirm.getLastRequest(RequestType_SetDefaultNetwork);
                    if (WIFI_STA == setType) {
                        myStatus->setDialState(WIFI_STA, DialState_Connected, DialType_None);
                        if (RequestType_RequestIp == lastRequest) {
                            notify.notifyRequestIpResult(WIFI_STA, NetworkErrCode_Success);
                            reqConfirm.confirm();
                        }
                        else if (RequestType_Access == lastRequest) {
                            NCNetworkPopAllRequestMapReq popAllReq(RequestMapType_Callback, PopResultFlag_Init);
                            popAllReq.doPopAllRequest();
                            reqConfirm.confirm();
                        }
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
        INT8 wifiGStationStatus = getWifiGStationStatus();
        INT8 mirrorlinkStatus = getMirrorLinkStatus();

        std::vector<INT8> carrierInfo;
        getDunCarrierInfo(carrierInfo);

        std::vector<INT8> vecDns1;
        std::vector<INT8> vecDns2;
        getDcmDnsInfo(vecDns1, vecDns2);

        oob.add_commDevice(commDevice);
        oob.add_routeStatus(commMode);
        oob.add_wifiSTAType(wifiGStationStatus);
        oob.add_mirrorlinkStatus(mirrorlinkStatus);
        oob.add_carrierInfo(fbb1.CreateVector(carrierInfo));
        oob.add_dns1(fbb1.CreateVector(vecDns1));
        oob.add_dns2(fbb1.CreateVector(vecDns2));

        fbb1.Finish(oob.Finish());
        std::string network_data((char*)fbb1.GetBufferPointer(), fbb1.GetSize());
        
        // construct NDAvcProtocolData
        flatbuffers::FlatBufferBuilder fbb2;
        extavc::NDAvcProtocolDataBuilder pdb(fbb2);
        pdb.add_head(extavc::CreateNDAvcDataHeader(fbb2,
            NHNetworkManagerOpcCode_NotifyCommStatusChange,
            NHAVC_UINIT_NETWORK_MANAGER1, NHAVC_UINIT_NETWORK_MANAGER2, extavc::NDAvcCommandType_PointToPoint, 0, 0));

        pdb.add_data(fbb2.CreateString(network_data.c_str(), network_data.size()));
        fbb2.Finish(pdb.Finish());

        NCData outData((char*)fbb2.GetBufferPointer(), fbb2.GetSize());
        if (0 != m_avcUnit->sendExtData(outData)) {
            NCLOGD("NCNetworkAvcUnit::notifyCommStatusChange, Send ext data failed");
        }
    }

    UINT32
    NCNetworkAvcUnit::getCommDevice()
    {
        UINT32 commDevice = 0;

        NCNetworkStatus* networkStatus = NCNetworkManager::Instance()->getNetworkStatus();
        for (UINT32 i = 0; i < DeviceNumber; i++) {
            NCString deviceType = DeviceArray[i];
            if (networkStatus->status(deviceType)) {
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

        NCNetworkStatus* networkStatus = NCNetworkManager::Instance()->getNetworkStatus();
        NCString route = networkStatus->currentNetwork();
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
        NCLOGD("NCNetworkAvcUnit::getWifiGStationStatus, Start");
        INT8 wifiGStation = 0x00;
        if (WIFI_STA == NCNetworkManager::Instance()->getNetworkStatus()->currentNetwork()) {
            wifiGStation = 0x02;
        }

        NCLOGD("NCNetworkAvcUnit::getWifiGStationStatus, Wifi Gstation [%x]", wifiGStation);
        return wifiGStation;
    }

    INT8
    NCNetworkAvcUnit::getMirrorLinkStatus()
    {
        NCLOGD("NCNetworkAvcUnit::getMirrorLinkStatus, Start");
        INT8 mirrorLinkState = 0x00;
        if (NCNetworkManager::Instance()->getNetworkStatus()->getMirrorLinkStatus()) {
            mirrorLinkState = 0x01;
        }

        NCLOGD("NCNetworkAvcUnit::getMirrorLinkStatus, Mirrorlink state [%x]", mirrorLinkState);
        return mirrorLinkState;
    }

    VOID
    NCNetworkAvcUnit::getDunCarrierInfo(std::vector<INT8>& info)
    {
        NCLOGD("NCNetworkAvcUnit::getDunCarrierInfo, Start");
        NCString tempInfo;
        NCNetworkManager::Instance()->getNetworkStatus()->getDunCarrierInfo(tempInfo);
        NCLOGD("NCNetworkAvcUnit::getDunCarrierInfo, tempInfo [%s]", tempInfo.getString());

        INT8 carrierInfo[NCNETWORK_BTDUN_CARRIER_INFO_LENGTH] = { 0x00 };
        NCCHAR tempArray[NCNETWORK_BTDUN_CARRIER_INFO_LENGTH + 1] = { 0 };
        INT len = snprintf(tempArray, sizeof(tempArray), tempInfo.getString());
        NCLOGD("NCNetworkAvcUnit::getDunCarrierInfo, len [%d], temp array [%s], strlen [%d]", len, tempArray, strlen(tempArray));
        if (len > 0) {
            memcpy(carrierInfo, tempArray, strlen(tempArray));
        }

        for (INT i = 0; i < NCNETWORK_BTDUN_CARRIER_INFO_LENGTH; ++i) {
            info.push_back(carrierInfo[i]);
        }
    }

    VOID
    NCNetworkAvcUnit::getDcmDnsInfo(std::vector<INT8>& dns1, std::vector<INT8>& dns2)
    {
        NCLOGD("NCNetworkAvcUnit::getDcmDnsInfo, Start");
        for (INT i = 0; i < 4; ++i) {
            dns1.push_back(0x00);
            dns2.push_back(0x00);
        }
    }

    VOID
    NCNetworkAvcUnit::updateCommDevice(const UINT32 commDevice)
    {
        char* device[2] = { "dcm", "dsrc" };
        UINT32 reference[2] = { 33554432, 67108864 };
        NCNetworkStatus* networkStatus = NCNetworkManager::Instance()->getNetworkStatus();
        for (UINT32 i = 0; i < 2; i++) {
            NCString deviceType = device[i];
            if (commDevice & reference[i]) {
                networkStatus->setStatus(deviceType, NC_TRUE);
            }
            else {
                networkStatus->setStatus(deviceType, NC_FALSE);
            }
        }
    }

} /* namespace nutshell */
/* EOF */
