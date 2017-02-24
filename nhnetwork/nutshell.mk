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

## Make stub.a Library
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_NICO_IFNAME := NINetworkHandler
LOCAL_MODULE := libninetworkhandler

LOCAL_MODULE_TAGS := optional

LOCAL_EXPORT_SYMBOLS := true

LOCAL_C_INCLUDES += system/handler/nhbase/include
LOCAL_SHARED_LIBRARIES := libnhbase libncore libbinder libcutils libutils libncservice

LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT)/lib
include $(BUILD_SHARED_LIBRARY)

#Make Handler Library
include $(CLEAR_VARS)
LOCAL_MODULE    := libnhnetworkhandler

LOCAL_NICO_IFNAME := NINetworkHandler

LOCAL_C_INCLUDES += \
                $(LOCAL_PATH) \
                system/handler/nhbase/include/ \
                system/handler/nhnetwork/include/ \
                system/handler/nhnetwork/src/ \
                externals/flatbuffers/include \

LOCAL_SRC_FILES := src/NHNetworkHandler.cpp \
                src/NHNetworkReq.cpp \
                src/NHNetworkReply.cpp \
                src/NHNetworkDhcp.cpp \
                src/NHNetworkDhcpInfo.cpp \
                src/NHNetworkCheckInterface.cpp \
                src/NHNetworkTimer.cpp \
                src/NHNetworkTimerThread.cpp
                
LOCAL_CFLAGS += -std=gnu++11

LOCAL_LDLIBS := -lrt -lpthread -ldl
LOCAL_SHARED_LIBRARIES := \
                libnhbase \
                libncore \
                liblog \
                libbinder \
                libcutils \
                libutils \
                libncservice \
                libnceventsys \
                libnetutils \



ifneq ($(filter emulator,$(BOARD_VENDOR)),)
LOCAL_CFLAGS += -DNCNETWORK_TARGET_PRODUCT_EMULATOR
endif

ifneq ($(filter DCU,$(TARGET_BOARD_TYPE)),)
LOCAL_CFLAGS += -DNCNETWORK_TARGET_PRODUCT_DCU
endif

ifneq ($(filter MEU,$(TARGET_BOARD_TYPE)),)
LOCAL_CFLAGS += -DNCNETWORK_TARGET_PRODUCT_MEU
endif

LOCAL_MODULE_TAGS := optional
LOCAL_EXPORT_SYMBOLS := true
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT)/lib

include $(BUILD_SHARED_LIBRARY)

####include all testcase
include $(CLEAR_VARS)
include $(LOCAL_PATH)/tests/nutshell.mk

# EOF
