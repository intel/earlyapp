/******************************************************************************\
Copyright (c) 2005-2018, Intel Corporation


Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

This sample was distributed or derived from the Intel's Media Samples package.
The original version of this sample may be obtained from https://software.intel.com/en-us/intel-media-server-studio
or https://software.intel.com/en-us/media-client-solutions-support.
\**********************************************************************************/

#include "vaapi_utils.h"
#include <dlfcn.h>
#include <stdexcept>

#include "vaapi_utils_drm.h"

#include "class_wayland.h"

namespace MfxLoader
{

SimpleLoader::SimpleLoader(const char * name)
{
    dlerror();
    so_handle = dlopen(name, RTLD_GLOBAL | RTLD_NOW);
    if (NULL == so_handle)
    {
        std::cerr << dlerror() << std::endl;
        throw std::runtime_error("Can't load library");
    }
}

void * SimpleLoader::GetFunction(const char * name)
{
    void * fn_ptr = dlsym(so_handle, name);
    if (!fn_ptr)
        throw std::runtime_error("Can't find function");
    return fn_ptr;
}

SimpleLoader::~SimpleLoader()
{
    dlclose(so_handle);
}

#define SIMPLE_LOADER_STRINGIFY1( x) #x
#define SIMPLE_LOADER_STRINGIFY(x) SIMPLE_LOADER_STRINGIFY1(x)
#define SIMPLE_LOADER_DECORATOR1(fun,suffix) fun ## _ ## suffix
#define SIMPLE_LOADER_DECORATOR(fun,suffix) SIMPLE_LOADER_DECORATOR1(fun,suffix)


// Following macro applied on vaInitialize will give:  vaInitialize((vaInitialize_type)lib.GetFunction("vaInitialize"))
#define SIMPLE_LOADER_FUNCTION(name) name( (SIMPLE_LOADER_DECORATOR(name, type)) lib.GetFunction(SIMPLE_LOADER_STRINGIFY(name)) )


VA_Proxy::VA_Proxy()
    : lib("libva.so.2")
    , SIMPLE_LOADER_FUNCTION(vaInitialize)
    , SIMPLE_LOADER_FUNCTION(vaTerminate)
    , SIMPLE_LOADER_FUNCTION(vaCreateSurfaces)
    , SIMPLE_LOADER_FUNCTION(vaDestroySurfaces)
    , SIMPLE_LOADER_FUNCTION(vaCreateBuffer)
    , SIMPLE_LOADER_FUNCTION(vaDestroyBuffer)
    , SIMPLE_LOADER_FUNCTION(vaMapBuffer)
    , SIMPLE_LOADER_FUNCTION(vaUnmapBuffer)
    , SIMPLE_LOADER_FUNCTION(vaSyncSurface)
    , SIMPLE_LOADER_FUNCTION(vaDeriveImage)
    , SIMPLE_LOADER_FUNCTION(vaDestroyImage)
    , SIMPLE_LOADER_FUNCTION(vaGetLibFunc)
    , SIMPLE_LOADER_FUNCTION(vaAcquireBufferHandle)
    , SIMPLE_LOADER_FUNCTION(vaReleaseBufferHandle)
    , SIMPLE_LOADER_FUNCTION(vaMaxNumEntrypoints)
    , SIMPLE_LOADER_FUNCTION(vaQueryConfigEntrypoints)
    , SIMPLE_LOADER_FUNCTION(vaGetConfigAttributes)
    , SIMPLE_LOADER_FUNCTION(vaCreateConfig)
    , SIMPLE_LOADER_FUNCTION(vaCreateContext)
    , SIMPLE_LOADER_FUNCTION(vaDestroyConfig)
    , SIMPLE_LOADER_FUNCTION(vaDestroyContext)
{
}

VA_Proxy::~VA_Proxy()
{}


DRM_Proxy::DRM_Proxy()
    : lib("libdrm.so.2")
    , SIMPLE_LOADER_FUNCTION(drmIoctl)
    , SIMPLE_LOADER_FUNCTION(drmModeAddFB)
    , SIMPLE_LOADER_FUNCTION(drmModeFreeConnector)
    , SIMPLE_LOADER_FUNCTION(drmModeFreeCrtc)
    , SIMPLE_LOADER_FUNCTION(drmModeFreeEncoder)
    , SIMPLE_LOADER_FUNCTION(drmModeFreePlane)
    , SIMPLE_LOADER_FUNCTION(drmModeFreePlaneResources)
    , SIMPLE_LOADER_FUNCTION(drmModeFreeResources)
    , SIMPLE_LOADER_FUNCTION(drmModeGetConnector)
    , SIMPLE_LOADER_FUNCTION(drmModeGetCrtc)
    , SIMPLE_LOADER_FUNCTION(drmModeGetEncoder)
    , SIMPLE_LOADER_FUNCTION(drmModeGetPlane)
    , SIMPLE_LOADER_FUNCTION(drmModeGetPlaneResources)
    , SIMPLE_LOADER_FUNCTION(drmModeGetResources)
    , SIMPLE_LOADER_FUNCTION(drmModeRmFB)
    , SIMPLE_LOADER_FUNCTION(drmModeSetCrtc)
    , SIMPLE_LOADER_FUNCTION(drmSetMaster)
    , SIMPLE_LOADER_FUNCTION(drmDropMaster)
    , SIMPLE_LOADER_FUNCTION(drmModeSetPlane)
{
}

DrmIntel_Proxy::~DrmIntel_Proxy()
{}

DrmIntel_Proxy::DrmIntel_Proxy()
    : lib("libdrm_intel.so.1")
    , SIMPLE_LOADER_FUNCTION(drm_intel_bo_gem_create_from_prime)
    , SIMPLE_LOADER_FUNCTION(drm_intel_bo_unreference)
    , SIMPLE_LOADER_FUNCTION(drm_intel_bufmgr_gem_init)
    , SIMPLE_LOADER_FUNCTION(drm_intel_bufmgr_destroy)
{
}

DRM_Proxy::~DRM_Proxy()
{}

VA_DRMProxy::VA_DRMProxy()
    : lib("libva-drm.so.2")
    , SIMPLE_LOADER_FUNCTION(vaGetDisplayDRM)
{
}

VA_DRMProxy::~VA_DRMProxy()
{}



#undef SIMPLE_LOADER_FUNCTION

} // MfxLoader

mfxStatus va_to_mfx_status(VAStatus va_res)
{
    mfxStatus mfxRes = MFX_ERR_NONE;

    switch (va_res)
    {
    case VA_STATUS_SUCCESS:
        mfxRes = MFX_ERR_NONE;
        break;
    case VA_STATUS_ERROR_ALLOCATION_FAILED:
        mfxRes = MFX_ERR_MEMORY_ALLOC;
        break;
    case VA_STATUS_ERROR_ATTR_NOT_SUPPORTED:
    case VA_STATUS_ERROR_UNSUPPORTED_PROFILE:
    case VA_STATUS_ERROR_UNSUPPORTED_ENTRYPOINT:
    case VA_STATUS_ERROR_UNSUPPORTED_RT_FORMAT:
    case VA_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE:
    case VA_STATUS_ERROR_FLAG_NOT_SUPPORTED:
    case VA_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED:
        mfxRes = MFX_ERR_UNSUPPORTED;
        break;
    case VA_STATUS_ERROR_INVALID_DISPLAY:
    case VA_STATUS_ERROR_INVALID_CONFIG:
    case VA_STATUS_ERROR_INVALID_CONTEXT:
    case VA_STATUS_ERROR_INVALID_SURFACE:
    case VA_STATUS_ERROR_INVALID_BUFFER:
    case VA_STATUS_ERROR_INVALID_IMAGE:
    case VA_STATUS_ERROR_INVALID_SUBPICTURE:
        mfxRes = MFX_ERR_NOT_INITIALIZED;
        break;
    case VA_STATUS_ERROR_INVALID_PARAMETER:
        mfxRes = MFX_ERR_INVALID_VIDEO_PARAM;
    default:
        mfxRes = MFX_ERR_UNKNOWN;
        break;
    }
    return mfxRes;
}
