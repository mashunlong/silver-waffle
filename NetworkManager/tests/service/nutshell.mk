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

# gtstlibncnetworkmanager

include $(CLEAR_VARS)
LOCAL_MODULE := socket_client

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES := system/core/include \
                    externals/googletest/include \
                    externals/googlemock/include \
                    externals/protobuf/src \
                    externals/icu4c/common \
                    externals/flatbuffers/include \
                    platform/base/AppFramework/include \
                    platform/base/ContentProvider/include \
                    platform/service/NetworkManager/include \
                    platform/service/NetworkManager/service \
                    system/handler/interface/include/ \
                    system/handler/nhbase/include/ \
                    system/handler/avcprotocol/include


LOCAL_SRC_FILES := testmain.cpp


LOCAL_LDFLAGS := -lpthread -lrt -ldl

LOCAL_CFLAGS += -std=gnu++11

LOCAL_STATIC_LIBRARIES := libdebugger \
                          libnetworkmanagerproto

LOCAL_ADDITIONAL_DEPENDENCIES := libavcprotocolmanage

LOCAL_SHARED_LIBRARIES := libgtest \
                          libncore \
                          libncapp \
                          libutils \
                          libcutils \
                          libevip \
                          libprotobuf \
                          libbinder \
                          libevipservice \
                          libncservice \
                          libnceventsys \
                          libncprocess \
                          libnhinterface \
                          libnccontentresolver \
                          libncdbhelper \
                          libncnetworkmanagerservice \
                          libncnetworkmanager

include $(BUILD_EXECUTABLE)

#build test package
include $(call all-makefiles-under,$(LOCAL_PATH))
# EOF