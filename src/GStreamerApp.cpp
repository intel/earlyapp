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

#include <gst/gst.h>

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
    bool GStreamerApp::init(const char* gstInitStr, bool createLoop)
    {
        LINF_(TAG, "Initializing GStreamerApp...");
        m_bCreateLoop = createLoop;

        LINF_(TAG, "Launching string: " << gstInitStr);
        if((m_pGSTPipeline = gst_parse_launch(gstInitStr, nullptr)) == nullptr)
        {
            LERR_(TAG, "Failed to GST launch.");
            return false;
        }

        return true;
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
            LERR_(TAG, "Pipe line is invalid.");
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
            LERR_(TAG, "Pipeline is invalid");
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
