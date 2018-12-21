////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2018 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
// SPDX-License-Identifier: MIT
//
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <boost/format.hpp>

#include "EALog.h"
#include "OutputDevice.hpp"
#include "VideoDevice.hpp"
#include "Configuration.hpp"

// A log tag for video device.
#define TAG "VIDEO"

namespace earlyapp
{
    /*
      Define a device instance variable.
    */
    VideoDevice* VideoDevice::m_pVDev = nullptr;

    /*
      Destructor.
     */
    VideoDevice::~VideoDevice(void)
    {
        if(m_pVDev != nullptr)
        {
            delete m_pVDev;
        }
    }

    /*
      A static function to get an instance(singleton).
    */
    VideoDevice* VideoDevice::getInstance(void)
    {
        if(m_pVDev == nullptr)
        {
            LINF_(TAG, "Creating a VideoDevice instance");
            m_pVDev = new VideoDevice();
        }

        return m_pVDev;
    }


    /*
      Intialize
     */
    void VideoDevice::init(std::shared_ptr<Configuration> pConf)
    {
        OutputDevice::init(pConf);

        m_pDecPipeline = new CDecodingPipeline(m_pGPIOCtrl);
        if(m_pDecPipeline == nullptr)
        {
            LERR_(TAG, "Failed to create decoder instance.");
            return;
        }

        m_Params.bUseHWLib = true;
        m_Params.bUseFullColorRange = false;
        m_Params.videoType = MFX_CODEC_AVC;

        m_Params.mode = MODE_RENDERING;
        m_Params.memType = D3D9_MEMORY;
        m_Params.mode = MODE_RENDERING;

        // File path.
        strcpy(m_Params.strSrcFile, pConf->videoSplashPath().c_str());
        // Width
        m_Params.Width = pConf->displayWidth();

        // Height
        m_Params.Height = pConf->displayHeight();

        // Default ASync depth.
        m_Params.nAsyncDepth = 4;

        // Initialize decoding pipeline.
        m_pDecPipeline->Init(&m_Params);

        
        LINF_(TAG, "VideoDevice initialized.");
    }

    /*
      Play the video device.
     */
    void VideoDevice::play(void)
    {
        LINF_(TAG, "VideoDevice play");

        // Start decoding and display.
        m_pDecPipeline->RunDecoding();
    }

    /*
      Stop.
    */
    void VideoDevice::stop(void)
    {
        LINF_(TAG, "VideoDevice stop");

        // Stop play
        if(m_pDecPipeline)
        {
            delete m_pDecPipeline;
            m_pDecPipeline = nullptr;
        }
    }

    /*
      Terminate
    */
    void VideoDevice::terminate(void)
    {
        LINF_(TAG, "VideoDevice terminate");

        // TODO: release resources.

    }
} // namespace

