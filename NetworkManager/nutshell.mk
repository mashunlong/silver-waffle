# Copyright @ 2013 - 2014 Suntec Software(Shanghai) Co., Ltd.
# All Rights Reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are NOT permitted except as agreed by
# Suntec Software(Shanghai) Co., Ltd.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

LOCAL_PATH:= $(call my-dir)

# libncnetworkmanager
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libncnetworkmanager

LOCAL_SRC_FILES := service/NCNetworkDhcp.cpp \
                   service/NCNetworkAccess.cpp \
                   service/NCNetworkData.cpp \
                   service/NCHttpClientManager.cpp

LOCAL_C_INCLUDES := \
                    $(LOCAL_PATH)/include \
                    system/core/include/ \
                    externals/protobuf/src \
                    system/handler/nhbase/include/ \
                    platform/base/AppFramework/include/

LOCAL_SHARED_LIBRARIES := libncore \
                          liblog \
                          libevip \
                          libutils \
                          libbinder \
                          libncservice \
                          libprotobuf

LOCAL_STATIC_LIBRARIES := libnetworkmanagerproto

LOCAL_LDFLAGS := -lpthread -lrt -ldl

LOCAL_EXPORT_SYMBOLS := true

include $(BUILD_SHARED_LIBRARY)


# libncnetworkmanagerservice
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libncnetworkmanagerservice

LOCAL_SRC_FILES := service/NCNetworkManager.cpp \
                   service/NCNetworkEventHandler.cpp \
                   service/NCNetworkReq.cpp \
                   service/NCNetworkNotify.cpp \
                   service/NCNetworkPluginCallbackImpl.cpp \
                   service/NCNetworkPluginManager.cpp \
                   service/NCNetworkDeviceType.cpp \
                   service/NCNetworkStatus.cpp \
                   service/NCNetworkAccessStack.cpp \
                   service/NCNetworkSignal.cpp \
                   service/NCNetworkSignalListener.cpp \
                   service/NCNetworkHandler.cpp \
                   service/NCNetworkThread.cpp \
                   service/NCNetworkReqConfirm.cpp \
                   service/NCHttpClients.cpp \
                   service/NCNetworkTimer.cpp \
                   service/NCNetworkTimerThread.cpp \
                   service/NCNetworkDiagLog.cpp \
                   service/NCNetworkUsbGadgetnetReplier.cpp \
                   service/NCNetworkEventReq.cpp

LOCAL_C_INCLUDES :=  \
                    $(LOCAL_PATH)/include/ \
                    $(LOCAL_PATH)/service/ \
                    system/core/include/ \
                    externals/protobuf/src \
                    externals/flatbuffers/include \
                    system/handler/nhbase/include/ \
                    system/handler/interface/include/ \
                    system/handler/avcprotocol/include \
                    platform/base/AppFramework/include \
                    platform/base/ContentProvider/include \
                    platform/base/DBHelper/include \
                    platform/service/SystemServer/include \
                    platform/service/ConnectManager/include \
                    system/hallib/interface/include/
                    

LOCAL_SHARED_LIBRARIES :=  libncore \
                           liblog \
                           libutils \
                           libcutils \
                           libbinder \
                           libevipservice \
                           libncservice \
                           libnceventsys \
                           libnhinterface \
                           libprotobuf \
                           libnccontentresolver \
                           libncdbhelper \
                           libncsystem \
                           libncconnectmanager \
                           libnihallib \
                           libevip

LOCAL_STATIC_LIBRARIES := libnetworkmanagerproto

LOCAL_ADDITIONAL_DEPENDENCIES := libavcprotocoldummy

LOCAL_CFLAGS += -std=gnu++11

ifneq ($(filter emulator,$(BOARD_VENDOR)),)
LOCAL_CFLAGS += -DNCNETWORK_TARGET_PRODUCT_EMULATOR
endif

ifneq ($(filter DCU,$(TARGET_BOARD_TYPE)),)
LOCAL_SRC_FILES += service/NCNetworkAvcUnit.cpp \
                   service/NCNetworkAvcUnitDcu.cpp
                   
LOCAL_CFLAGS += -DNCNETWORK_TARGET_PRODUCT_DCU
endif

ifneq ($(filter MEU,$(TARGET_BOARD_TYPE)),)
LOCAL_SRC_FILES += service/NCNetworkAvcUnit.cpp \
                   service/NCNetworkAvcUnitMeu.cpp
                   
LOCAL_CFLAGS += -DNCNETWORK_TARGET_PRODUCT_MEU
endif

LOCAL_LDFLAGS := -lpthread -lrt -ldl

include $(BUILD_SHARED_LIBRARY)

#build test package
include platform/service/NetworkManager/tests/service/nutshell.mk
# EOF
