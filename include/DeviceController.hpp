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

#include "CBCEvent.hpp"
#include "OutputDevice.hpp"
#include "AudioDevice.hpp"
#include "VideoDevice.hpp"
#include "CameraDevice.hpp"
#include "SystemStatusTracker.hpp"
#include "Configuration.hpp"


namespace earlyapp
{
    class DeviceController
    {
    public:
        // Constructor.
        DeviceController(std::shared_ptr<Configuration> pConf, SystemStatusTracker* pSST);

        // Initialize.
        void init(
            AudioDevice* pAud=nullptr,   // Audio device.
            VideoDevice* pVid=nullptr,   // Video device.
            CameraDevice* pCam=nullptr); // Camera device.

        // Control devices.
        bool controlDevices(void);

        // Stop all devices.
        void stopAllDevices(void);

        // Terminate all devices.
        void terminateAllDevices(void);

        // Has the controller initialized?
        bool isInitialized(void);

        // Number of registered devices.
        int numDevices(void);

    private:
        // Initialized
        bool m_bInit = false;

        // Pointer to the SystemStatusTracker.
        SystemStatusTracker* m_pSST;

        // Output devices.
        std::set<OutputDevice*> m_devs;

        // Configuration.
        std::shared_ptr<Configuration> m_pConf;

        AudioDevice* m_pAud = nullptr;
        VideoDevice* m_pVid = nullptr;
        CameraDevice* m_pCam = nullptr;
    };
} // namespace
