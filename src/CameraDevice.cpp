
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
#include <boost/thread.hpp>

#include "EALog.h"
#include "OutputDevice.hpp"
#include "CameraDevice.hpp"
#include "Configuration.hpp"

// A log tag for Camera device
#define TAG "CAMERA"

namespace earlyapp
{
    /*
      Define a device instance variable.
    */
    CameraDevice* CameraDevice::m_pCDev = nullptr;

    /*
      Destructor.
     */
    CameraDevice::~CameraDevice(void)
    {
        if(m_pCDev != nullptr)
        {
            delete m_pCDev;
        }
    }

    /*
      A static function to get an instance(singleton).
    */
    CameraDevice* CameraDevice::getInstance(void)
    {
        if(m_pCDev == nullptr)
        {
            LINF_(TAG, "Creating a CameraDevice instance");
            m_pCDev = new CameraDevice();
        }

        return m_pCDev;
    }


    /*
      Intialize
     */
    void CameraDevice::init(std::shared_ptr<Configuration> pConf)
    {
        OutputDevice::init(pConf);

        m_pConf = pConf;

        m_ICIEnabled = ConfigureICI();
        strcpy(m_iciParam.stream, "/dev/intel_stream27");

        // Set output width/height with user set values.
        // Set default if not set.
        m_iciParam.ow = m_pConf->displayWidth();
        m_iciParam.oh = m_pConf->displayHeight();
        if((int) m_iciParam.ow == Configuration::DONT_CARE)
            m_iciParam.ow = DEFAULT_CAMERA_WIDTH;
        if((int) m_iciParam.oh == Configuration::DONT_CARE)
            m_iciParam.oh = DEFAULT_CAMERA_HEIGHT;

        m_iciParam.iw = DEFAULT_CAMERA_WIDTH;
        m_iciParam.ih = DEFAULT_CAMERA_HEIGHT;
        m_iciParam.isys_w = 0;
        m_iciParam.isys_h = 0;
        m_iciParam.stride_width = 736;
        m_iciParam.use_wh = 0;
        m_iciParam.in_fourcc = ICI_FORMAT_UYVY;

        m_iciParam.buffer_count = 4;
        m_iciParam.port = 4;
        m_iciParam.fullscreen = 0;
        m_iciParam.interlaced = 0;
        m_iciParam.frames_count = 0;
        m_iciParam.stream_input = CVBS_INPUT;
        m_iciParam.mem_type = ICI_MEM_DMABUF;
        m_stream_id = 27;
        LINF_(TAG, "Camerea intialized.");
    }

    /*
      Play the video device.
     */
    void CameraDevice::play(void)
    {
        LINF_(TAG, "CameraDevice play");
        if(m_ICIEnabled)
        {
            // Create a thread for the camera dispaly and run.
                m_pThreadGrpRVC = new(boost::thread_group);
                m_pThreadRVC = m_pThreadGrpRVC->create_thread(
                    boost::bind(
                        &displayCamera, m_iciParam, m_stream_id));
            OutputDevice::outputGPIOPattern();
        }
        else
        {
            LINF_(TAG, "Fail CameraDevice play...");
        }
    }

    /*
      Stop.
    */
    void CameraDevice::stop(void)
    {
        LINF_(TAG, "Stopping camera...");
	    if(m_ICIEnabled)
        {
            iciStopDisplay(0);
            // Wait for thread join.
            if(m_pThreadGrpRVC)
            {
                m_pThreadGrpRVC->join_all();
                delete m_pThreadGrpRVC;
                m_pThreadGrpRVC = nullptr;
                m_pThreadRVC = nullptr;
            }
        }
        else
            LINF_(TAG, "Fail Stopping camera...");
    }

    /*
      Terminate.
    */
    void CameraDevice::terminate(void)
    {
        LINF_(TAG, "CameraDevice terminate");
    }

    void CameraDevice::displayCamera(setup m_iciParam, int stream_id)
    {
        LINF_(TAG, "Display loop.");

        iciStartDisplay(m_iciParam, stream_id, 1);
    }

} // namespace

