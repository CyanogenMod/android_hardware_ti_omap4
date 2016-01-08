/*
 * Copyright (c) 2016 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "ti_gralloc_wrapper"
/* #define LOG_NDEBUG 0 */

#include <errno.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#include <cutils/log.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>
#include "../hwc/hal_public.h"

/* These bits should be removed from usage */
#define GRALLOC_REMOVE_USAGE_MASK       (GRALLOC_USAGE_CURSOR)

/* Padded module structure */
typedef struct IMG_gralloc_module_t
{
    gralloc_module_t base;
    /* According to nm the size of the symbol is 448 bytes */
    unsigned char pad[GRALLOC_HMI_SIZE];
} IMG_gralloc_module_t;

static void *dso_handle = NULL;

/* Function pointers to HAL */
int (*real_gralloc_alloc)(struct alloc_device_t* dev, int w, int h, int format, int usage, buffer_handle_t* handle, int* stride) = NULL;

/* Pointers to data structures from HAL */
static IMG_gralloc_module_t *real_gralloc_module = NULL;
static alloc_device_t *real_gralloc_device = NULL;

int wrapper_lock(struct gralloc_module_t const* module, buffer_handle_t handle, int usage, int l, int t, int w, int h, void** vaddr)
{
    usage &= ~(GRALLOC_REMOVE_USAGE_MASK);

    return real_gralloc_module->base.lock(module, handle, usage, l, t, w, h, vaddr);
}

int wrapper_alloc(struct alloc_device_t* dev, int w, int h, int format, int usage, buffer_handle_t* handle, int* stride)
{
    usage &= ~(GRALLOC_REMOVE_USAGE_MASK);

    return real_gralloc_alloc(dev, w, h, format, usage, handle, stride);
}

static int wrapper_open(const hw_module_t* module, const char* name, hw_device_t** device);

static struct hw_module_methods_t wrapper_module_methods = {
    .open = wrapper_open,
};

IMG_gralloc_module_t HAL_MODULE_INFO_SYM = {
    .base = {
        .common = {
            .tag = HARDWARE_MODULE_TAG,
            .version_major = 0,
            .version_minor = 1,
            .id = GRALLOC_HARDWARE_MODULE_ID,
            .name = "TI gralloc wrapper",
            .author = "The CyanogenMod Project (Michael Gernoth)",
            .methods = &wrapper_module_methods
        },
    },
};

static int wrapper_open(const hw_module_t* module, const char* name, hw_device_t** device)
{
    int ret;

    if (!real_gralloc_module) {
        return -EINVAL;
    }

    ((IMG_gralloc_module_t*)module)->base.common.dso = dso_handle;

    ret = real_gralloc_module->base.common.methods->open(module, name, device);
    if (ret != 0) {
        ALOGE("wrapper_open: couldn't open");
        return ret;
    }

    real_gralloc_alloc = ((alloc_device_t*)(*device))->alloc;

    ((alloc_device_t*)(*device))->alloc = wrapper_alloc;

    return ret;
}

__attribute__((constructor)) void overload()
{
    unsigned int i;

    ALOGI("Initializing wrapper for TI's gralloc");

    dso_handle = dlopen("/system/vendor/lib/hw/" REAL_GRALLOC, RTLD_NOW);
    if (dso_handle == NULL) {
        char const *err_str = dlerror();
        ALOGE("wrapper_open: %s", err_str ? err_str : "unknown");
        return;
    }

    const char *sym = HAL_MODULE_INFO_SYM_AS_STR;
    real_gralloc_module = (IMG_gralloc_module_t*)dlsym(dso_handle, sym);
    if (real_gralloc_module == NULL) {
        ALOGE("wrapper_open: couldn't find symbol %s", sym);
        dlclose(dso_handle);
        dso_handle = NULL;
        return;
    }

    memcpy(&HAL_MODULE_INFO_SYM, real_gralloc_module, GRALLOC_HMI_SIZE);

    ALOGI("Loaded %s %s", HAL_MODULE_INFO_SYM.base.common.name, HAL_MODULE_INFO_SYM.base.common.author);

    HAL_MODULE_INFO_SYM.base.common.methods = &wrapper_module_methods;
    HAL_MODULE_INFO_SYM.base.lock = wrapper_lock;
}
