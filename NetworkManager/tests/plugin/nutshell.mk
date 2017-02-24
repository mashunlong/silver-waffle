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

# libncnetworkplugin
include $(CLEAR_VARS)

LOCAL_MODULE := libncnetworkplugintest

LOCAL_C_FLAGS := -O2

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := TestNCNetworkPlugin.cpp

LOCAL_C_INCLUDES := platform/service/NetworkManager/include \
                    platform/base/AppFramework/include

LOCAL_LDFLAGS := -lpthread  -lrt

LOCAL_SHARED_LIBRARIES := libncore libutils

include $(BUILD_SHARED_LIBRARY)

# EOF