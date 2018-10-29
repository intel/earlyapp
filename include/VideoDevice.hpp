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

#include "OutputDevice.hpp"
#include "GStreamerApp.hpp"
#include "Configuration.hpp"


namespace earlyapp
{
    /*
      VideoDevice
      : A class abstracts video playback device without audio.
     */
    class VideoDevice: public OutputDevice, GStreamerApp
    {
    public:
        static VideoDevice* getInstance(void);

        void init(std::shared_ptr<Configuration> pConf);

        void play(void);

        void stop(void);

        void terminate(void);

        virtual ~VideoDevice(void);

    protected:
        /*
          Create an video pipeline.
         */
        virtual GstElement* createPipeline(std::shared_ptr<Configuration> pConf);

    private:
        // Hide the default constructor to prevent instancitating.
        VideoDevice(void) { }

        // Pointer for a video playback device instance.
        static VideoDevice* m_pVDev;

        // Handle dynamic pad.
        static void handleNewPad(GstElement* decodeBin, GstPad* pPad, gpointer data);

        /*
          GStreamer elements.
        */
        GstElement* m_pVideoSrc = nullptr;
        GstElement* m_pVideoSink = nullptr;
        GstElement* m_pDecodeBin = nullptr;
        GstElement* m_pVideoScale = nullptr;
        GstElement* m_pScaleFilter = nullptr;
    };
} // namespace

