/******************************************************************************\
Copyright (c) 2005-2018, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

This sample was distributed or derived from the Intel's Media Samples package.
The original version of this sample may be obtained from https://software.intel.com/en-us/intel-media-server-studio
or https://software.intel.com/en-us/media-client-solutions-support.
\**********************************************************************************/

#include "vaapi_device.h"
#include "class_wayland.h"
#include "wayland-drm-client-protocol.h"

CVAAPIDeviceWayland::~CVAAPIDeviceWayland(void)
{
    Close();
}

mfxStatus CVAAPIDeviceWayland::Init(mfxHDL hWindow, mfxU16 nViews, mfxU32 nAdapterNum)
{
    mfxStatus mfx_res = MFX_ERR_NONE;

    if(nViews)
    {
        m_Wayland = (Wayland*)WaylandCreate();
        if(!m_Wayland->InitDisplay()) {
            return MFX_ERR_DEVICE_FAILED;
        }

        if(NULL == m_Wayland->GetDisplay())
        {
            mfx_res = MFX_ERR_UNKNOWN;
            return mfx_res;
        }
       if(-1 == m_Wayland->DisplayRoundtrip())
        {
            mfx_res = MFX_ERR_UNKNOWN;
            return mfx_res;
        }
        if(!m_Wayland->CreateSurface())
        {
            mfx_res = MFX_ERR_UNKNOWN;
            return mfx_res;
        }
    }
    return mfx_res;
}

mfxStatus CVAAPIDeviceWayland::RenderFrame(mfxFrameSurface1 * pSurface, mfxFrameAllocator * /*pmfxAlloc*/)
{
    uint32_t drm_format = 0;
    int offsets[3], pitches[3];
    mfxStatus mfx_res = MFX_ERR_NONE;
    vaapiMemId * memId = NULL;
    struct wl_buffer *m_wl_buffer = NULL;
    if(NULL==pSurface) {
        mfx_res = MFX_ERR_UNKNOWN;
        return mfx_res;
    }
    m_Wayland->Sync();
    memId = (vaapiMemId*)(pSurface->Data.MemId);

    if (pSurface->Info.FourCC == MFX_FOURCC_NV12)
    {
        drm_format = WL_DRM_FORMAT_NV12;
    } else if(pSurface->Info.FourCC == MFX_FOURCC_RGB4)
    {
        drm_format = WL_DRM_FORMAT_ARGB8888;

        if (m_isMondelloInputEnabled)
        {
            drm_format = WL_DRM_FORMAT_XBGR8888;
        }
    }

    offsets[0] = memId->m_image.offsets[0];
    offsets[1] = memId->m_image.offsets[1];
    offsets[2] = memId->m_image.offsets[2];
    pitches[0] = memId->m_image.pitches[0];
    pitches[1] = memId->m_image.pitches[1];
    pitches[2] = memId->m_image.pitches[2];
    m_wl_buffer = m_Wayland->CreatePrimeBuffer(memId->m_buffer_info.handle
      , pSurface->Info.CropW
      , pSurface->Info.CropH
      , drm_format
      , offsets
      , pitches);
    if(NULL == m_wl_buffer)
    {
            msdk_printf("\nCan't wrap flink to wl_buffer\n");
            mfx_res = MFX_ERR_UNKNOWN;
            return mfx_res;
    }

    m_Wayland->RenderBuffer(m_wl_buffer, pSurface->Info.CropW, pSurface->Info.CropH);

    // GPIO output.
    if(!m_bGotFirstFrame && m_pGPIOCtrl != nullptr)
    {
        m_pGPIOCtrl->outputPattern();
        m_bGotFirstFrame = true;
    }

    return mfx_res;
}

void CVAAPIDeviceWayland::Close(void)
{
    m_Wayland->FreeSurface();
}

CHWDevice* CreateVAAPIDevice(earlyapp::GPIOControl* pGPIOCtrl)
{
    return new CVAAPIDeviceWayland(pGPIOCtrl);
}
