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
#include "Configuration.hpp"
#include "pipeline_decode.h"

namespace earlyapp
{
    /**
      @brief A class abstracts video playback device without audio.
     */
    class VideoDevice: public OutputDevice
    {
    public:
        /**
           @brief Returns video device instance.
        */
        static VideoDevice* getInstance(void);

        /**
           @brief Initializes video device.
           @param pConf Uset set configurations.
        */
        void init(std::shared_ptr<Configuration> pConf);

        /**
           @brief Playback the video device.
        */
        void play(void);

        /**
           @brief Stop playback.
        */
        void stop(void);

        /**
           @brief Terminate the device an deallocate resources.
        */
        void terminate(void);

        /**
           @brief Destructor.
        */
        virtual ~VideoDevice(void);

    private:
        // Hide the default constructor to prevent instancitating.
        VideoDevice(void) { OutputDevice::m_pDevName = "MSDK Video"; }

        // Pointer for a video playback device instance.
        static VideoDevice* m_pVDev;

        /**
           Media SDK Decoding pipeline
         */
        CDecodingPipeline* m_pDecPipeline = nullptr;

        /**
           Decoding input parameters.
         */
        sInputParams m_Params;
    };
} // namespace

