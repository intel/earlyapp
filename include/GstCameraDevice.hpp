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
    /**
      @brief A class abstracts camera device.
     */
    class GstCameraDevice: public OutputDevice, public GStreamerApp
    {
    public:
        /**
           @brief Returns instance of GstCameraDevice object.
        */
        static GstCameraDevice* getInstance(void);

        /**
           @brief Initialize camera device.
           @pConf User set configuration parameters.
        */
        void init(std::shared_ptr<Configuration> pConf);

        /**
           @brief Play the camera device.
        */
        void play(void);

        /**
           @brief Stop the camera device.
        */
        void stop(void);

        /**
           @brief Terminate all resources.
        */
        void terminate(void);


        /**
           @brief Destructor.
        */
        virtual ~GstCameraDevice(void);

    protected:
        /**
          @brief Create a GStreamer camera pipeline.
          @pConf User set configuration parameter.
         */
        virtual GstElement* createPipeline(std::shared_ptr<Configuration> pConf);

    private:
        /**
           @brief Default constructor hidden in preivate to prevent instancitation.
        */
        GstCameraDevice(void) { OutputDevice::m_pDevName = "Gst Camera"; }

        /**
           @brief Create a custom command GStreamer pipeline.
           @param customGstCmd GStreamer command string for creating a pipeline.
           @return A new camera pipeline, nullptr for errors.
         */
        GstElement* createPipelineFromString(std::string& customGstCmd);

        /**
           @brief Create a fixed GStreamer pipeline for ICI.
           @param camInputSrc Camera input source.
           @return A new camera pipeline, nullptr for errors.
         */
        GstElement* createFixedPipeline(std::string& camInputSrc);

        /**
           @brief Camear device instance.
        */
        static GstCameraDevice* m_pCDev;

        /**
           @brief User configuration.
        */
        std::shared_ptr<Configuration> m_pConf;

        /*
          GStreamer elements.
         */
        GstElement* m_pCamSrc = nullptr;
        GstElement* m_pCamSink = nullptr;
        GstElement* m_pPostProc = nullptr;
        GstElement* m_pScale = nullptr;
        GstElement* m_pScaleFilter = nullptr;
    };
} // namespace
