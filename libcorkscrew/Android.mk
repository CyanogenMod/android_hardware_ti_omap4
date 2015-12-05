LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := corkscrew.c
LOCAL_SHARED_LIBRARIES:= libc
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_MODULE := libcorkscrew
LOCAL_MODULE_TAGS := optional

ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge 20 || echo 1),)
	include $(BUILD_SHARED_LIBRARY)
endif
