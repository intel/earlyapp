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


#include "hw_device.h"
#include "vaapi_utils_drm.h"


CHWDevice* CreateVAAPIDevice(void);

/** VAAPI DRM implementation. */
class CVAAPIDeviceDRM : public CHWDevice
{
public:
    CVAAPIDeviceDRM(int type);
    virtual ~CVAAPIDeviceDRM(void);

    virtual mfxStatus Init(mfxHDL hWindow, mfxU16 nViews, mfxU32 nAdapterNum);
    virtual mfxStatus Reset(void) { return MFX_ERR_NONE; }
    virtual void Close(void) { }

    virtual mfxStatus SetHandle(mfxHandleType type, mfxHDL hdl) { return MFX_ERR_UNSUPPORTED; }
    virtual mfxStatus GetHandle(mfxHandleType type, mfxHDL *pHdl)
    {
        if ((MFX_HANDLE_VA_DISPLAY == type) && (NULL != pHdl))
        {
            *pHdl = m_DRMLibVA.GetVADisplay();

            return MFX_ERR_NONE;
        }
        return MFX_ERR_UNSUPPORTED;
    }

    virtual mfxStatus RenderFrame(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc);
    virtual void      UpdateTitle(double fps) { }
    virtual void      SetMondelloInput(bool isMondelloInputEnabled) { }

    inline drmRenderer* getRenderer() { return m_rndr; }
protected:
    DRMLibVA m_DRMLibVA;
    drmRenderer * m_rndr;
private:
    // no copies allowed
    CVAAPIDeviceDRM(const CVAAPIDeviceDRM &);
    void operator=(const CVAAPIDeviceDRM &);
};




class Wayland;

#define HANDLE_WAYLAND_DRIVER   (MFX_HANDLE_VA_DISPLAY << 4)

class CVAAPIDeviceWayland : public CHWDevice
{
public:
    CVAAPIDeviceWayland(){
        m_nRenderWinX = 0;
        m_nRenderWinY = 0;
        m_nRenderWinW = 0;
        m_nRenderWinH = 0;
        m_isMondelloInputEnabled = false;
        m_Wayland = NULL;
    }
    virtual ~CVAAPIDeviceWayland(void);

    virtual mfxStatus Init(mfxHDL hWindow, mfxU16 nViews, mfxU32 nAdapterNum);
    virtual mfxStatus Reset(void) { return MFX_ERR_NONE; }
    virtual void Close(void);

    virtual mfxStatus SetHandle(mfxHandleType type, mfxHDL hdl) { return MFX_ERR_UNSUPPORTED; }
    virtual mfxStatus GetHandle(mfxHandleType type, mfxHDL *pHdl)
    {
        if((MFX_HANDLE_VA_DISPLAY == type) && (NULL != pHdl)) {
            *pHdl = m_DRMLibVA.GetVADisplay();
            return MFX_ERR_NONE;
        } else if((HANDLE_WAYLAND_DRIVER  == type) && (NULL != m_Wayland)) {
            *pHdl = m_Wayland;
            return MFX_ERR_NONE;
    }
    return MFX_ERR_UNSUPPORTED;
    }
    virtual mfxStatus RenderFrame(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc);
    virtual void UpdateTitle(double fps) { }

    virtual void SetMondelloInput(bool isMondelloInputEnabled)
    {
        m_isMondelloInputEnabled = isMondelloInputEnabled;
    }

protected:
    DRMLibVA m_DRMLibVA;
    Wayland *m_Wayland;
private:
    mfxU32 m_nRenderWinX;
    mfxU32 m_nRenderWinY;
    mfxU32 m_nRenderWinW;
    mfxU32 m_nRenderWinH;

    bool m_isMondelloInputEnabled;

    // no copies allowed
    CVAAPIDeviceWayland(const CVAAPIDeviceWayland &);
    void operator=(const CVAAPIDeviceWayland &);
};

