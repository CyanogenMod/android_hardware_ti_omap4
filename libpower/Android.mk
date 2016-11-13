#
# Copyright 2016 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := power.omap4
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SRC_FILES := power.c
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -Wall -Werror $(ANDROID_API_CFLAGS)

ifneq ($(BOARD_DOUBLE_TAP_TO_WAKE_PATH),)
LOCAL_CFLAGS += -DDOUBLE_TAP_TO_WAKE_PATH=$(BOARD_DOUBLE_TAP_TO_WAKE_PATH)
endif

include $(BUILD_SHARED_LIBRARY)
