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
