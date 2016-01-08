LOCAL_PATH:= $(call my-dir)

###
### Wrapper for TI gralloc
###

include $(CLEAR_VARS)

LOCAL_MODULE := gralloc.$(TARGET_DEVICE)

LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib/hw/
LOCAL_SRC_FILES := wrapper.c

LOCAL_CFLAGS := -DREAL_GRALLOC=\"gralloc.omap$(TARGET_BOARD_OMAP_CPU).so\"

# To find out your correct BOARD_GRALLOC_HMI_SIZE run:
# arm-linux-androideabi-nm -S -D $OUT/system/vendor/lib/hw/gralloc.omap4.so | grep HMI | cut -d' ' -f 2
LOCAL_CFLAGS += -DGRALLOC_HMI_SIZE=$(BOARD_GRALLOC_HMI_SIZE)

LOCAL_SHARED_LIBRARIES := liblog libcutils libdl
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
