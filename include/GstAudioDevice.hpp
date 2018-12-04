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

#include <string>
#include <gst/gst.h>
#include "OutputDevice.hpp"
#include "GStreamerApp.hpp"
#include "Configuration.hpp"


namespace earlyapp
{
    /**
      @brief A class abstracts audio playback device.
     */
    class GstAudioDevice: public OutputDevice, public GStreamerApp
    {
    public:
        /**
           @brief Returns audio device singlton instance.
         */
        static GstAudioDevice* getInstance(void);

        /**
           @brief Initializes the audio device.
           @param pConf User set configurations.
         */
        void init(std::shared_ptr<Configuration> pConf);

        /**
           @brief Prepare for playback audio file.
           @param playParam Parameters for audio playback including file path.
        */
        void preparePlay(std::shared_ptr<DeviceParameter> playParam=nullptr);

        /**
           @brief Play the audio device.
         */
        void play(void);

        /**
           @brief Stop the audio device.
        */
        void stop(void);

        /**
           @brief Terminate the device and eturn all resources.
        */
        void terminate(void);

        /**
           @brief Destructor.
        */
        virtual ~GstAudioDevice(void);

    protected:
        /**
          @brief Create an audio GStreamer pipeline.
          @param pConf User set configurations.
         */
        virtual GstElement* createPipeline(std::shared_ptr<Configuration> pConf);

    private:
        // Hide the default constructor to prevent instancitating.
        GstAudioDevice(void) { }

        // Program configuration.
        std::shared_ptr<Configuration> m_pConf;

        static  GstAudioDevice* m_pADev;

        /*
          GStreamer elements.
         */
        GstElement* m_pAudioSrc = nullptr;
        GstElement* m_pAlsaSink = nullptr;
        GstElement* m_pWavParse = nullptr;
        GstElement* m_pAudioCnv = nullptr;
        GstElement* m_pAudioPipeline = nullptr;

        /**
          @brief Releases audio resources except pipeline.
         */
        void releaseAudioResource(void);
    };
} // namespace
