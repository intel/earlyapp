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

#include <string>
#include <vector>
#include <boost/format.hpp>
#include <mutex>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "EALog.h"
#include "Configuration.hpp"
#include "DeviceController.hpp"
#include "SystemStatusTracker.hpp"
#include "OutputDevice.hpp"

#ifdef USE_GSTREAMER
#include "GstAudioDevice.hpp"
#include "GstVideoDevice.hpp"
#include "GstCameraDevice.hpp"
#else
#include "AudioDevice.hpp"
#include "VideoDevice.hpp"
#include "CameraDevice.hpp"
#endif


// A tag for DeviceController.
#define TAG "DCTL"

namespace earlyapp
{
    /*
      Constructor.
     */
    DeviceController::DeviceController(std::shared_ptr<Configuration> pConf, SystemStatusTracker* pSST)
    {
        m_pConf = pConf;
        m_pSST = pSST;
    }

    /*
      Initialize device controller.
     */
    void DeviceController::init(OutputDevice* pAud, OutputDevice* pVid, OutputDevice* pCam)
    {
        if(m_pSST == nullptr)
        {
            return;
        }

        // Register audio, video, camera devices.
#ifdef USE_GSTREAMER
        m_pAud =
            (pAud == nullptr) ? GstAudioDevice::getInstance():pAud;
        m_pVid =
            (pVid == nullptr) ? GstVideoDevice::getInstance():pVid;
        m_pCam =
            (pCam == nullptr) ? GstCameraDevice::getInstance():pCam;
#else
        m_pAud =
            (pAud == nullptr) ? AudioDevice::getInstance():pAud;
        m_pVid =
            (pVid == nullptr) ? VideoDevice::getInstance():pVid;
        m_pCam =
            (pCam == nullptr) ? CameraDevice::getInstance():pCam;
#endif

        addDevice(m_pAud);
        addDevice(m_pVid);
        addDevice(m_pCam);

        // Wait for the Wayland.
        waitForWayland();

        // Initalize devices.
        for(auto& it: m_Devs)
        {
            it->init(m_pConf);
        }
        m_bInit = true;
    }

    /*
      Control devices based upon system status tracker.
     */
    bool DeviceController::controlDevices(void)
    {
        if(m_pSST == nullptr)
            return false;

        // Current status.
        switch(m_pSST->currentState())
        {
            case SystemStatusTracker::eSTATE_BOOTRVC:
            {
                // Audio device.
                std::shared_ptr<DeviceParameter> audioParam(new DeviceParameter());
                audioParam->setFileToPlay(m_pConf->audioRVCSoundPath());

                if(m_pAud != nullptr)
                {
                    m_pAud->preparePlay(audioParam);
                    m_pAud->play();
                }
                else
                {
                    LWRN_(TAG, "Invaid Audio device: BOOTRVC");
                }

                // Camera device.
                if(m_pCam != nullptr)
                {
                    m_pCam->preparePlay();
                    m_pCam->play();
                }
                else
                {
                    LWRN_(TAG, "Invalid Camera device: BOOTRVC");
                }
            }
            break;

            case SystemStatusTracker::eSTATE_BOOTVIDEO:
            {
                // Audio device.
                std::shared_ptr<DeviceParameter> audioParam(new DeviceParameter());
                audioParam->setFileToPlay(m_pConf->audioSplashSoundPath());
                if(m_pAud != nullptr)
                {
                    m_pAud->preparePlay(audioParam);
                    m_pAud->play();
                }
                else
                {
                    LWRN_(TAG, "Invalid Audio device: BOOTVIDEO");
                }

                // Video device.
                if(m_pVid != nullptr)
                {
                    m_pVid->preparePlay(nullptr);
                    m_pVid->play();
                    // The VideoDevice will be held until it gets EOS.
                    m_pVid->prepareStop();
                    m_pVid->stop();
                }
                else
                {
                    LWRN_(TAG, "Invalid Video device: BOOTVIDEO");
                }
            }
            break;

            case SystemStatusTracker::eSTATE_IDLE:
                stopAllDevices();
            break;

            case SystemStatusTracker::eSTATE_RVC:
            {
                // Audio device.
                std::shared_ptr<DeviceParameter> audioParam(new DeviceParameter());
                audioParam->setFileToPlay(m_pConf->audioRVCSoundPath());

                if(m_pAud != nullptr)
                {
                    m_pAud->preparePlay(audioParam);
                    m_pAud->play();
                }
                else
                {
                    LWRN_(TAG, "Invalid Audio device: RVC");
                }

                // Camera device.
                if(m_pCam != nullptr)
                {
                    m_pCam->preparePlay();
                    m_pCam->play();
                }
                else
                {
                    LWRN_(TAG, "Invalid Camera device: RVC");
                }
            }
            break;
            default:
            break;
        } // End of switch/case

        return true;
    }

    /*
      Stop all devices.
     */
    void DeviceController::stopAllDevices(void)
    {
        for(auto& it: m_Devs)
        {
            it->prepareStop();
            it->stop();
        }
    }

    /*
      Terminate all devices.
     */
    void DeviceController::terminateAllDevices(void)
    {
        for(auto& it: m_Devs)
        {
            it->terminate();
        }
    }

    /*
      Has the controller initialized?
     */
    bool DeviceController::isInitialized(void)
    {
        return m_bInit;
    }

    /*
      How many devices being registered?
     */
    int DeviceController::numDevices(void)
    {
        return (int) m_Devs.size();
    }

    /*
      Add output device.
     */
    void DeviceController::addDevice(OutputDevice* pDev)
    {
        if(pDev != nullptr)
            m_Devs.insert(pDev);
        else
            LWRN_(TAG, "Device not added");
    }

    /*
      Block until the Wayland compositor is ready.
     */
    void DeviceController::waitForWayland(void)
    {
        char WLSocketPath[WAYLAND_SOCKET_PATH_LENGTH];
        const char* pXDGEnv = getenv(XDG_RUNTIME_DIR);
        const char* pWLDispEnv = getenv(WAYLAND_DISPLAY);

        if(pXDGEnv != nullptr)
        {
            // Use default file name if not defined.
            if(pWLDispEnv == nullptr)
            {
                LWRN_(TAG, "WAYLAND_DISPLAY has not defined");
                pWLDispEnv = DEFAULT_WAYLAND_SOCKET;
            }

            struct stat st;
            snprintf(WLSocketPath, WAYLAND_SOCKET_PATH_LENGTH, "%s/%s", pXDGEnv, pWLDispEnv);
            LINF_(TAG, "Waiting for wayland socket: " <<  WLSocketPath);

            while(stat(WLSocketPath, &st) != 0)
            {
                usleep(100);
            }
        }
        else
        {
            LWRN_(TAG, "XDG_RUNTIM_DIR not defined. Abandoning Wayland wayting");
        }
    }
} // namespace
