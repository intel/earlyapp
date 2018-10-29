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
      Create a GStreamer pipeline.
     */
    GstElement* CameraDevice::createPipeline(std::shared_ptr<Configuration> pConf)
    {
        // Camera pipeline.
        GstElement* camPipeline = gst_pipeline_new(nullptr);
        GstElement* camSrcCapsFilter = nullptr;
        /*
          Camera input source. - ICI, V4L2, Test source.
         */
        std::string camSrc = pConf->cameraInputSource();
        if(camSrc.compare("ici") == 0)
        {
            m_pCamSrc = gst_element_factory_make("icamerasrc", nullptr);
            g_object_set(G_OBJECT(m_pCamSrc), "device-name", 1, nullptr);
            g_object_set(G_OBJECT(m_pCamSrc), "interlace-mode", 7, nullptr);
            g_object_set(G_OBJECT(m_pCamSrc), "deinterlace-method", 3, nullptr);


            // TODO: Check evironment value GST_PLUGIN_PATH

            // Capsfilter.
            GstCaps* iciCaps = gst_caps_new_simple(
                "video/x-raw",
                "format", G_TYPE_STRING, "UYVY",
                "width", G_TYPE_INT, 720,
                "height", G_TYPE_INT, 480,
                nullptr);

            camSrcCapsFilter = gst_element_factory_make("capsfilter", nullptr);

            gst_bin_add(GST_BIN(camPipeline), m_pCamSrc);
            gst_bin_add(GST_BIN(camPipeline), camSrcCapsFilter);

            if(! gst_element_link_pads_filtered(m_pCamSrc, "src", camSrcCapsFilter, "sink", iciCaps))
            {
                LWRN_(TAG, "Failed to link ici source to ici filter");
            }
            gst_caps_unref(iciCaps);
        }
        else if(camSrc.compare("v4l2") == 0)
        {
            m_pCamSrc = gst_element_factory_make("v4l2src", nullptr);
            gst_bin_add(GST_BIN(camPipeline), m_pCamSrc);
        }
        else
        {
            m_pCamSrc = gst_element_factory_make("videotestsrc", nullptr);
            gst_bin_add(GST_BIN(camPipeline), m_pCamSrc);
        }

        m_pCamSink = gst_element_factory_make("waylandsink", nullptr);
        m_pPostProc = gst_element_factory_make("vaapipostproc", nullptr);
        m_pScale = gst_element_factory_make("videoscale", nullptr);
        m_pScaleFilter = gst_element_factory_make("capsfilter", nullptr);


        // Failed to create GStreamer elements.
        if(
            m_pCamSrc == nullptr
            || m_pPostProc == nullptr
            || m_pScale == nullptr
            || m_pScaleFilter == nullptr
            || m_pCamSink == nullptr)
        {
            return nullptr;
        }

        // Add to bin.
        gst_bin_add(GST_BIN(camPipeline), m_pPostProc);
        gst_bin_add(GST_BIN(camPipeline), m_pScale);
        gst_bin_add(GST_BIN(camPipeline), m_pScaleFilter);
        gst_bin_add(GST_BIN(camPipeline), m_pCamSink);

        GstCaps* caps = scaleCapsfilter();

        // Link GstElements.
        if(camSrcCapsFilter != nullptr)
        {
            if(! gst_element_link_pads(camSrcCapsFilter, "src", m_pPostProc, "sink"))
            {
                LWRN_(TAG, "Failed to link source capsfilter to post-processor");
            }
        }
        else
        {
            if(! gst_element_link_pads(m_pCamSrc, "src", m_pPostProc, "sink"))
            {
                LWRN_(TAG, "Failed to link source to post-processor");
            }
        }
        if(! gst_element_link_pads(m_pPostProc, "src", m_pScale, "sink"))
        {
            LWRN_(TAG, "Failed to link post-processor to scaler");
        }
        if(! gst_element_link_pads_filtered(m_pScale, "src", m_pScaleFilter, "sink", caps))
        {
            LWRN_(TAG, "Failed to link scaler to caps-filter");
        }
        gst_caps_unref(caps);
        if(! gst_element_link_pads(m_pScaleFilter, "src", m_pCamSink, "sink"))
        {
            LWRN_(TAG, "Failed to link caps-filter to sink");
        }

        return camPipeline;
    }

    /*
      Intialize
     */
    void CameraDevice::init(std::shared_ptr<Configuration> pConf)
    {
        OutputDevice::init(pConf);

        m_pConf = pConf;
        // Display size.
        setDisplaySize(pConf->displayWidth(), pConf->displayHeight());

        GstElement* camPipeline = createPipeline(pConf);
        if(! GStreamerApp::init(camPipeline, true))
        {
            LERR_(TAG, "Failed to init GST app.");
            return;
        }

        LINF_(TAG, "Camerea intialized.");
    }

    /*
      Play the video device.
     */
    void CameraDevice::play(void)
    {
        LINF_(TAG, "CameraDevice play");
        // Initialization again makes ICI camera last longer.
        //init(m_pConf);
        OutputDevice::outputGPIOPattern();
        startPlay();
    }

    /*
      Stop.
    */
    void CameraDevice::stop(void)
    {
        LINF_(TAG, "Stopping camera...");
        stopPlay();
    }

    /*
      Terminate.
    */
    void CameraDevice::terminate(void)
    {
        LINF_(TAG, "CameraDevice terminate");
    }

} // namespace

