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
#include <boost/thread.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include "pipeline-cfg.h"
#include "icitest.h"

#ifdef __cplusplus
}
#endif

namespace earlyapp
{
    /**
      @brief A class abstracts camera device.
     */
    class CameraDevice: public OutputDevice
    {
    public:
        /**
           @brief Returns instance of CameraDevice object.
        */
        static CameraDevice* getInstance(void);

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
        virtual ~CameraDevice(void);

    private:
        /**
           @brief Default constructor hidden in preivate to prevent instancitation.
        */
        CameraDevice(void) { OutputDevice::m_pDevName = "ICI Camera"; }

        /**
           @brief Camear device instance.
        */
        static CameraDevice* m_pCDev;

        /**
           @brief User configuration.
        */
        std::shared_ptr<Configuration> m_pConf;

        setup m_iciParam;

        int m_stream_id = -1;

        int m_ICIEnabled = 1;

        /*
          Boost thread.
         */
        boost::thread_group* m_pThreadGrpRVC = nullptr;
        boost::thread* m_pThreadRVC = nullptr;

        static void displayCamera(setup, int, void*);

        void* m_pGPIOClass = NULL;

        /**
           @brief Default camera width, height.
        */
        const unsigned int DEFAULT_CAMERA_WIDTH = 720;
        const unsigned int DEFAULT_CAMERA_HEIGHT = 240;
    };
} // namespace
