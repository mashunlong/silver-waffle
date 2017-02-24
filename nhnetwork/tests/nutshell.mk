# Copyright @ 2013 - 2015 Suntec Software(Shanghai) Co., Ltd.
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
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_MODULE:= gtstlibnhnetworkhandler

LOCAL_NICO_IFNAME := NINetworkHandler

LOCAL_SRC_FILES := TestNHNetworkHandler.cpp \
                   TestNHNetworkReply.cpp \
                   TestNHNetworkReq.cpp 

LOCAL_C_INCLUDES := \
                    $(LOCAL_PATH) \
                    $(TARGET_OUT_HEADERS)/nistub \
                    system/core/include \
                    externals/icu4c/common/ \
                    externals/googletest/include/ \
                    system/handler/interface/include \
                    system/handler/nhbase/include \
                    system/handler/nhnetwork/include \
                    system/handler/nhnetwork/ \
                    system/core/include \
                    system/handler/nhnetwork/src \
                    externals/flatbuffers/include \

LOCAL_SHARED_LIBRARIES := libcutils \
                          libncore \
                          libgtest \
                          libbinder \
                          libnhinterface \
                          libninetworkhandler \
                          libnhnetworkhandler \
                          libnhbase \
                          libnetutils \

LOCAL_STATIC_LIBRARIES := libgtest_main libdebugger

LOCAL_ADDITIONAL_DEPENDENCIES := libninetworkhandler

LOCAL_CFLAGS += -std=gnu++11

LOCAL_LDFLAGS := -lpthread -lrt -ldl

include $(BUILD_EXECUTABLE)

# EOF