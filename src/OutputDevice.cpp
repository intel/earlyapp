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
#include "EALog.h"
#include "OutputDevice.hpp"

#define TAG "OUTDEV"

namespace earlyapp
{
    DeviceParameter::DeviceParameter(void)
    {
    }

    DeviceParameter::DeviceParameter(const char* filePath)
    {
        setFileToPlay(filePath);
    }

    DeviceParameter::DeviceParameter(const std::string& filePath)
    {
        setFileToPlay(filePath);
    }

    void DeviceParameter::setFileToPlay(const char* filePath)
    {
        m_sFilePath = std::string(filePath);
    }

    void DeviceParameter::setFileToPlay(const std::string& filePath)
    {
        m_sFilePath = filePath;
    }

    // Returns file path.
    std::string& DeviceParameter::fileToPlay(void)
    {
        return m_sFilePath;
    }

    // Preapare for play.
    void OutputDevice::preparePlay(std::shared_ptr<DeviceParameter> playParam)
    {
        LINF_(TAG, "preparePlay()");
    }

    // Play
    void OutputDevice::play(void)
    {
        LINF_(TAG, "play()");
    }

    // Prepare for stop.
    void OutputDevice::prepareStop(void)
    {
        LINF_(TAG, "prepareStop()");
    }

    // Stop.
    void OutputDevice::stop(void)
    {
        LINF_(TAG, "stop()");
    }

    // Terminate.
    void OutputDevice::terminate(void)
    {
        LINF_(TAG, "terminate()");
    }
} // namespace
