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

#include <gst/gst.h>
#include <unistd.h>

#include "EALog.h"
#include "GStreamerApp.hpp"

#define TAG "GST"

namespace earlyapp
{
    /*
      Destructor.
     */
    GStreamerApp::~GStreamerApp(void)
    {
        if(m_pGSTPipeline)
        {
            gst_object_unref(GST_OBJECT(m_pGSTPipeline));
            m_pGSTPipeline = nullptr;
        }
    }


    /*
      Intialize
    */
    bool GStreamerApp::init(GstElement* gstPipeline, bool createLoop)
    {
        LINF_(TAG, "Initializing GStreamerApp...");
        m_bCreateLoop = createLoop;
        m_pGSTPipeline = gstPipeline;

        if(m_pGSTPipeline == nullptr)
        {
            LERR_(TAG, "Pipeline is invalid.");
            return false;
        }
        return true;
    }


    /*
      Play the video device.
    */
    void GStreamerApp::startPlay(void)
    {
        LINF_(TAG, "Start display");

        // GST pipeline
        if(m_pGSTPipeline == nullptr)
        {
            LERR_(TAG, "Pipeline is invalid(nullptr)");
            return;
        }

        // Get a bus.
        m_pGSTBus = gst_element_get_bus(m_pGSTPipeline);

        // Create a loop.
        if(m_bCreateLoop)
        {
            m_pGSTLoop = g_main_loop_new(nullptr, false);

            // Create a thread for the camera dispaly and run.
            m_pThreadGrp = new(boost::thread_group);
            m_pThread = m_pThreadGrp->create_thread(
                boost::bind(
                    &displayLoop, m_pGSTPipeline, m_pGSTLoop));
            m_pGSTBus = gst_element_get_bus(m_pGSTPipeline);
            m_pGSTMsg = gst_bus_timed_pop_filtered(
		m_pGSTBus,
		GST_CLOCK_TIME_NONE,
		(GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
        }
        // In a same trhead.
        else
        {
            LINF_(TAG, "Display loop in the same thread.");

            if(gst_element_set_state(m_pGSTPipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
            {
                LERR_(TAG, "Failed to start play.");
                return;
            }
            m_pGSTBus = gst_element_get_bus(m_pGSTPipeline);
            m_pGSTMsg = gst_bus_timed_pop_filtered(
                m_pGSTBus,
                GST_CLOCK_TIME_NONE,
                (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
        }
    }

    /*
      Display loop.
     */
    void GStreamerApp::displayLoop(GstElement* pipeline, GMainLoop* loop)
    {
        LINF_(TAG, "Display loop.");

        if(gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
        {
            LERR_(TAG, "Failed to start play.");
            return;
        }

        g_main_loop_run(loop);
    }


    /*
      Stop.
    */
    void GStreamerApp::stopPlay(void)
    {
        LINF_(TAG, "Stop display");

        // Stop the loop.
        if(m_pGSTLoop)
            g_main_loop_quit(m_pGSTLoop);

        // Stop GStreamer play.
        gst_element_set_state(m_pGSTPipeline, GST_STATE_NULL);
        if(m_pGSTBus)
        {
            gst_object_unref(GST_OBJECT(m_pGSTBus));
            m_pGSTBus = nullptr;
        }

        // Wait for thread join.
        if(m_pThreadGrp)
        {
            m_pThreadGrp->join_all();
            delete m_pThreadGrp;
            m_pThreadGrp = nullptr;
            m_pThread = nullptr;
        }

        // Release loop resource.
        if(m_pGSTLoop)
        {
            g_main_loop_unref(m_pGSTLoop);
            m_pGSTLoop = nullptr;
        }
    }

    /*
      Set display size.
    */
    void GStreamerApp::setDisplaySize(unsigned int width, unsigned int height)
    {
        m_DispWidth = width;
        m_DispHeight = height;
    }

    /*
      Display width
    */
    int GStreamerApp::displayWidth(void)
    {
        return m_DispWidth;
    }

    /*
      Display Height
    */
    int GStreamerApp::displayHeight(void)
    {
        return m_DispHeight;
    }

    /*
      Video scale capsfilter
    */
    GstCaps* GStreamerApp::scaleCapsfilter(void)
    {
        GstCaps* caps = nullptr;

        if(displayWidth() != Configuration::DONT_CARE
            && displayHeight() != Configuration::DONT_CARE)
        {
            caps = gst_caps_new_simple(
            "video/x-raw",
            "width", G_TYPE_INT, displayWidth(),
            "height", G_TYPE_INT, displayHeight(),
            nullptr);
        }
        else if(displayWidth() != Configuration::DONT_CARE)
        {
            caps = gst_caps_new_simple(
                "video/x-raw",
                "width", G_TYPE_INT, displayWidth(),
                nullptr);
        }
        else if(displayHeight() != Configuration::DONT_CARE)
        {
            caps = gst_caps_new_simple(
                "video/x-raw",
                "height", G_TYPE_INT, displayHeight(),
                nullptr);
        }
        else
        {
            caps = nullptr;
        }
        return caps;
    }
} // namespace
