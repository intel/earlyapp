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

#pragma once

#include <gst/gst.h>
#include <boost/thread.hpp>
#include "Configuration.hpp"

namespace earlyapp
{
    class GStreamerApp
    {
    public:
        /*
          Destructor.
         */
        ~GStreamerApp(void);

        /*
          Init
           - gstInitStr: GStreamer initialization string.
           - createLoop: True to create own loop.
        */
        bool init(const char* gstInitStr, bool createLoop=true);

        /*
          Init
           - gstPipeLine: A pointer for a GStreamer pipeline.
           - createLoop: True to create own loop.
        */
        bool init(GstElement* gstPipeline, bool createLoop=true);

        /*
          Start play.
         */
        void startPlay(void);

        /*
          Stop play.
         */
        void stopPlay(void);

        /*
          Set display size.
         */
        void setDisplaySize(unsigned int width, unsigned int height);

        /*
          Display width
         */
        int displayWidth(void);

        /*
          Display height
         */
        int displayHeight(void);

    protected:
        /*
          Create a GStreamer pipeline.
          Concrete classes should implement this member function that returns
          a GStreamer pipeline based on user input.
         */
        virtual GstElement* createPipeline(std::shared_ptr<Configuration> pConf) = 0;

        /*
          Video scale capsfilter.
          Returns scale caps based on user input.
         */
        GstCaps* scaleCapsfilter(void);

    private:
        /*
          True - create a loop.
         */
        bool m_bCreateLoop;

        /*
          Width/height.
         */
        int m_DispWidth = 0;
        int m_DispHeight = 0;

        /*
          GStreamer elements.
         */
        GstElement* m_pGSTPipeline = nullptr;
        GstBus* m_pGSTBus = nullptr;
        GstMessage* m_pGSTMsg = nullptr;
        GMainLoop* m_pGSTLoop = nullptr;

        /*
          Boost thread.
         */
        boost::thread_group* m_pThreadGrp = nullptr;
        boost::thread* m_pThread = nullptr;


        /*
          Release all resources.
         */
        void releaseResources(void);

        /*
          Concerete class can modify this.
         */
        static void displayLoop(GstElement* pipeline, GMainLoop* loop);
    };
} // namespace
