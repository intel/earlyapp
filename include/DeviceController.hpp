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

#include <set>
#include <mutex>

#include "CBCEvent.hpp"
#include "OutputDevice.hpp"
#include "Configuration.hpp"
#include "SystemStatusTracker.hpp"
#include <pthread.h>

namespace earlyapp
{
	static std::shared_ptr<Configuration> s_pConf;
	static pthread_t s_initdev_tid[8];
    /**
       @brief Controls output device for current state.
    */
    class DeviceController
    {
    public:
        /**
           @brief Constructor.
           @param pConf User set configuration.
           @param pSST System status tracker.
        */
        DeviceController(void);

        /**
           @brief Constructor.
           @param pConf User set configuration.
           @param pSST System status tracker.
        */
        DeviceController(std::shared_ptr<Configuration> pConf, SystemStatusTracker* pSST);

        /**
           @brief Initializes the object.
           @param pAud Audio device instance.
           @param pVid Video device instance.
           @param pCam Camera device instance.
           @param bWaitWL Wait for wayland socket before initialize devices.
        */
        void init(
            OutputDevice* pAud=nullptr,
            OutputDevice* pVid=nullptr,
            OutputDevice* pCam=nullptr,
            bool bWaitWL = true);

        /**
           @brief Control devices.
           @return false for any errors, true otherwise.
        */
        bool controlDevices(void);

        /**
           @brief Stop all devices.
        */
        void stopAllDevices(void);

        /**
           @brief Terminate all devices.
        */
        void terminateAllDevices(void);

        /**
           @brief Has the controller initialized?
           @return true if initialized, false otherwise.
        */
        bool isInitialized(void);

        /**
           @brief Number of registered devices.
        */
        int numDevices(void);

        static void * init_device(void *param);
    private:
        /**
           @brief A flag for initialization.
        */
        bool m_bInit = false;

        /**
           @brief Pointer to the SystemStatusTracker.
        */
        SystemStatusTracker* m_pSST;

        /**
           @brief Container for all controlling  Output devices.
        */
        std::set<OutputDevice*> m_Devs;

        /**
           @brief Configuration.
        */
        std::shared_ptr<Configuration> m_pConf;

        /**
           @brief Audio device instance.
         */
        OutputDevice* m_pAud = nullptr;

        /**
           @brief Video device instance.
        */
        OutputDevice* m_pVid = nullptr;

        /**
           @brief Camera device instance.
        */
        OutputDevice* m_pCam = nullptr;

        /**
           @brief Add output instance if valid.
        */
        inline void addDevice(OutputDevice* pDev);

        /**
          @brief Block until the Wayland compositor is ready.
         */
        void waitForWayland(void);

        /**
           @brief Maximum length of the Wayland socket path.
         */
        const int WAYLAND_SOCKET_PATH_LENGTH = 255;

        /**
           @brief Environment variable XDG_RUNTIME_DIR.
        */
        const char* XDG_RUNTIME_DIR = "XDG_RUNTIME_DIR";

        /**
           @brief Environment variable WAYLAND_DISPLAY.
        */
        const char* WAYLAND_DISPLAY = "WAYLAND_DISPLAY";

        /**
           @brief Default wayland socket name.
        */
        const char* DEFAULT_WAYLAND_SOCKET = "wayland-0";

    };
} // namespace
