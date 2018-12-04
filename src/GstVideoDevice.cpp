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
#include <gst/gst.h>
#include <boost/format.hpp>

#include "EALog.h"
#include "OutputDevice.hpp"
#include "GstVideoDevice.hpp"
#include "Configuration.hpp"

// A log tag for video device.
#define TAG "VIDEO"

namespace earlyapp
{
    /*
      Define a device instance variable.
    */
    GstVideoDevice* GstVideoDevice::m_pVDev = nullptr;

    /*
      Destructor.
     */
    GstVideoDevice::~GstVideoDevice(void)
    {
        if(m_pVDev != nullptr)
        {
            delete m_pVDev;
        }
    }

    /*
      A static function to get an instance(singleton).
    */
    GstVideoDevice* GstVideoDevice::getInstance(void)
    {
        if(m_pVDev == nullptr)
        {
            LINF_(TAG, "Creating a GstVideoDevice instance");
            m_pVDev = new GstVideoDevice();
        }

        return m_pVDev;
    }

    /*
      Call back to handle dynamic pad for decodebin.
     */
    void GstVideoDevice::handleNewPad(GstElement* decodeBin, GstPad* pPad, gpointer data)
    {
        GstElement *videoScaler = (GstElement*)data;
        gst_element_link(decodeBin, videoScaler);
    }

    /*
      Returns video pipeline.
     */
    GstElement* GstVideoDevice::createPipeline(std::shared_ptr<Configuration> pConf)
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
    void GstVideoDevice::init(std::shared_ptr<Configuration> pConf)
    {
        OutputDevice::init(pConf);

        // Display size.
        setDisplaySize(pConf->displayWidth(), pConf->displayHeight());

        GstElement* videoPipeline = createPipeline(pConf);

        // Pipeline will be deallocated by GStreamerApp class.
        if(! GStreamerApp::init(videoPipeline, false))
        {
            LERR_(TAG, "Failed to init GST app.");
            return;
        }

        LINF_(TAG, "GstVideoDevice initialized.");
    }

    /*
      Play the video device.
     */
    void GstVideoDevice::play(void)
    {
        LINF_(TAG, "GstVideoDevice play");
        OutputDevice::outputGPIOPattern();
        startPlay();
    }

    /*
      Stop.
    */
    void GstVideoDevice::stop(void)
    {
        LINF_(TAG, "GstVideoDevice stop");
        stopPlay();
    }

    /*
      Terminate
    */
    void GstVideoDevice::terminate(void)
    {
        LINF_(TAG, "GstVideoDevice terminate");

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

