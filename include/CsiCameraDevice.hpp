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

#include "csi_common.h"

#ifdef __cplusplus
}
#endif

namespace earlyapp
{
    /**
      @brief A class abstracts camera device.
     */
    class CsiCameraDevice: public OutputDevice
    {
    public:
        /**
           @brief Returns instance of CameraDevice object.
        */
        static CsiCameraDevice* getInstance(void);

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
        virtual ~CsiCameraDevice(void);

    private:
        /**
           @brief Default constructor hidden in preivate to prevent instancitation.
        */
        CsiCameraDevice(void) { OutputDevice::m_pDevName = "CSI Camera"; }

        /**
           @brief Camear device instance.
        */
        static CsiCameraDevice* m_pCDev;

        /**
           @brief User configuration.
        */
        std::shared_ptr<Configuration> m_pConf;

        set_up m_csiParam;

	/*
          Boost thread.
         */
        boost::thread_group* m_pThreadGrpCsiRVC = nullptr;
        boost::thread* m_pThreadCsiRVC = nullptr;

        static void displayCamera(set_up, void*);

        void* m_pGPIOClass = NULL;

        /**
           @brief Default camera width, height.
        */
        const unsigned int DEFAULT_CAMERA_WIDTH = 720;
        const unsigned int DEFAULT_CAMERA_HEIGHT = 240;
    };
} // namespace
