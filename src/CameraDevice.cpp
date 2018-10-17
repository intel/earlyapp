////////////////////////////////////////////////////////////////////////////////
//
// Copyright Intel Corporation All Rights Reserved.
//
// The source code, information and material ("Material") contained herein is
// owned by Intel Corporation or its suppliers or licensors, and title to such
// Material remains with Intel Corporation or its suppliers or licensors. The
// Material contains proprietary information of Intel or its suppliers and
// licensors. The Material is protected by worldwide copyright laws and treaty
// provisions. No part of the Material may be used, copied, reproduced,
// modified, published, uploaded, posted, transmitted, distributed or disclosed
// in any way without Intel's prior express written permission. No license under
// any patent, copyright or other intellectual property rights in the Material
// is granted to or conferred upon you, either expressly, by implication,
// inducement, estoppel or otherwise. Any license under such intellectual
// property rights must be express and approved by Intel in writing.
//
// Unless otherwise agreed by Intel in writing, you may not remove or alter this
// notice or any other notice embedded in Materials by Intel or Intel's
// suppliers or licensors in any way."
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

