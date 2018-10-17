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
        for(auto it = m_devs.begin(); it != m_devs.end(); ++it)
        {
            (*it)->init(m_pConf);
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
        for(auto it = m_devs.begin(); it != m_devs.end(); ++it)
        {
            (*it)->prepareStop();
            (*it)->stop();
        }
    }

    /*
      Terminate all devices.
     */
    void DeviceController::terminateAllDevices(void)
    {
        for(auto it = m_devs.begin(); it != m_devs.end(); ++it)
        {
            (*it)->terminate();
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
