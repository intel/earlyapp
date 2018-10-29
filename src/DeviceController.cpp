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
#include "EALog.h"
#include "DeviceController.hpp"
#include "SystemStatusTracker.hpp"
#include "AudioDevice.hpp"
#include "VideoDevice.hpp"
#include "CameraDevice.hpp"
#include "Configuration.hpp"

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
    void DeviceController::init(AudioDevice* pAud, VideoDevice* pVid, CameraDevice* pCam)
    {
        if(m_pSST == nullptr)
        {
            return;
        }

        // Register audio, video, camera devices.
        m_pAud = (pAud == nullptr) ? AudioDevice::getInstance():pAud;
        m_pVid = (pVid == nullptr) ? VideoDevice::getInstance():pVid;
        m_pCam = (pCam == nullptr) ? CameraDevice::getInstance():pCam;

        m_devs.insert(m_pAud);
        m_devs.insert(m_pVid);
        m_devs.insert(m_pCam);

        // Initalization.
        for(auto& it: m_devs)
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
                m_pAud->preparePlay(audioParam);
                m_pAud->play();

                // Camera device.
                m_pCam->preparePlay();
                m_pCam->play();
            }
            break;

            case SystemStatusTracker::eSTATE_BOOTVIDEO:
            {
                // Audio device.
                std::shared_ptr<DeviceParameter> audioParam(new DeviceParameter());
                audioParam->setFileToPlay(m_pConf->audioSplashSoundPath());
                m_pAud->preparePlay(audioParam);
                m_pAud->play();

                // Video device.
                m_pVid->preparePlay(nullptr);
                m_pVid->play();
                // The VideoDevice will be held until it gets EOS.
                m_pVid->prepareStop();
                m_pVid->stop();
            }
            break;

            case SystemStatusTracker::eSTATE_IDLE:
                m_pAud->prepareStop();
                m_pAud->stop();

                m_pCam->prepareStop();
                m_pCam->stop();

                m_pVid->prepareStop();
                m_pVid->stop();
            break;

            case SystemStatusTracker::eSTATE_RVC:
            {
                // Audio device.
                std::shared_ptr<DeviceParameter> audioParam(new DeviceParameter());
                audioParam->setFileToPlay(m_pConf->audioRVCSoundPath());
                m_pAud->preparePlay(audioParam);
                m_pAud->play();

                // Camera device.
                m_pCam->preparePlay();
                m_pCam->play();
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
        for(auto& it: m_devs)
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
        for(auto& it: m_devs)
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
        return (int) m_devs.size();
    }
} // namespace
