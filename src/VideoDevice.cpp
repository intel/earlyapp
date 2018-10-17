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
#include <gst/gst.h>
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
      Call back to handle dynamic pad for decodebin.
     */
    void VideoDevice::handleNewPad(GstElement* decodeBin, GstPad* pPad, gpointer data)
    {
        GstElement *videoScaler = (GstElement*)data;
        gst_element_link(decodeBin, videoScaler);
    }

    /*
      Returns video pipeline.
     */
    GstElement* VideoDevice::createPipeline(std::shared_ptr<Configuration> pConf)
    {
        // Source
        m_pVideoSrc = gst_element_factory_make("filesrc", nullptr);
        g_object_set(G_OBJECT(m_pVideoSrc), "location", pConf->videoSplashPath().c_str(), nullptr);

        // Sink
        m_pVideoSink = gst_element_factory_make("waylandsink", nullptr);

        // Decode bin
        m_pDecodeBin = gst_element_factory_make("decodebin", nullptr);

        // Scaler
        m_pVideoScale = gst_element_factory_make("videoscale", nullptr);

        // Filter
        m_pScaleFilter = gst_element_factory_make("capsfilter", nullptr);

        // Failed to create GStreamer elements.
        if(
            m_pVideoSrc == nullptr
            || m_pVideoSink == nullptr
            || m_pVideoScale == nullptr
            || m_pScaleFilter == nullptr
            || m_pDecodeBin == nullptr)
        {
            return nullptr;
        }

        // Create a pipeline
        GstElement* videoPipeline = gst_pipeline_new(nullptr);
        gst_bin_add_many(
            GST_BIN(videoPipeline),
            m_pVideoSrc,
            m_pDecodeBin,
            m_pVideoScale,
            m_pScaleFilter,
            m_pVideoSink,
            nullptr);

        GstCaps* caps = scaleCapsfilter();

        if(! gst_element_link_pads(m_pVideoSrc, "src", m_pDecodeBin, "sink"))
        {
            LWRN_(TAG, "Failed to link source and decoder");
        }
        if(! gst_element_link_pads(m_pDecodeBin, "src", m_pVideoScale, "sink"))
        {
            LWRN_(TAG, "Failed to link decoder and scaler");
        }
        if(! gst_element_link_pads_filtered(m_pVideoScale, "src", m_pScaleFilter, "sink", caps))
        {
            LWRN_(TAG, "Failed to link scaler and filter");
        }
        gst_caps_unref(caps);
        if(! gst_element_link_pads(m_pScaleFilter, "src", m_pVideoSink, "sink"))
        {
            LWRN_(TAG, "Failed to link filter and sink");
        }

        // Dynamic pads for decode bin.
        g_signal_connect(m_pDecodeBin, "pad-added", G_CALLBACK(handleNewPad), m_pVideoScale);

        return videoPipeline;
    }

    /*
      Intialize
     */
    void VideoDevice::init(std::shared_ptr<Configuration> pConf)
    {
        // Display size.
        setDisplaySize(pConf->displayWidth(), pConf->displayHeight());

        GstElement* videoPipeline = createPipeline(pConf);

        // Pipeline will be deallocated by GStreamerApp class.
        if(! GStreamerApp::init(videoPipeline, false))
        {
            LERR_(TAG, "Failed to init GST app.");
            return;
        }

        LINF_(TAG, "VideoDevice initialized.");
    }

    /*
      Play the video device.
     */
    void VideoDevice::play(void)
    {
        LINF_(TAG, "VideoDevice play");
        startPlay();
    }

    /*
      Stop.
    */
    void VideoDevice::stop(void)
    {
        LINF_(TAG, "VideoDevice stop");
        stopPlay();
    }

    /*
      Terminate
    */
    void VideoDevice::terminate(void)
    {
        LINF_(TAG, "VideoDevice terminate");

        if(m_pVideoSrc)
        {
            gst_object_unparent(GST_OBJECT(m_pVideoSrc));
            gst_object_unref(GST_OBJECT(m_pVideoSrc));
            m_pVideoSrc = nullptr;
        }

        if(m_pVideoSink)
        {
            gst_object_unparent(GST_OBJECT(m_pVideoSink));
            gst_object_unref(GST_OBJECT(m_pVideoSink));
            m_pVideoSink = nullptr;
        }

        if(m_pDecodeBin)
        {
            gst_object_unparent(GST_OBJECT(m_pDecodeBin));
            gst_object_unref(GST_OBJECT(m_pDecodeBin));
            m_pDecodeBin = nullptr;
        }
    }
} // namespace

