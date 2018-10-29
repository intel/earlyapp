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
