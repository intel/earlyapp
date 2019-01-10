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


#include <gtest/gtest.h>
#include <memory>

#include "DeviceController.hpp"
#include "CBCEvent.hpp"
#include "SystemStatusTracker.hpp"
#include "Configuration.hpp"

#include "GstAudioDevice.hpp"
#include "GstVideoDevice.hpp"
#include "GstCameraDevice.hpp"

#include "AudioDevice.hpp"
#include "VideoDevice.hpp"
#include "CameraDevice.hpp"


using namespace earlyapp;


/**
    @brief Device controller test.
 */
class DeviceControllerTest: public ::testing::Test
{
protected:
    void SetUp(void)
    {
        int argc = 1;
        char* argv[] =
        {
            (char*) "programname"
        };

        pConf = earlyapp::Configuration::makeConfiguration(argc, argv);
        pSST = new SystemStatusTracker();
        pSST->init();

        pDevCtrl = new DeviceController(pConf, pSST);
    }

    void TestDown(void)
    {
        if(pDevCtrl != nullptr)
        {
            delete pDevCtrl;
            pDevCtrl = nullptr;
        }

        if(pSST != nullptr)
        {
            delete pSST;
            pSST = nullptr;
        }
    }

    SystemStatusTracker* pSST = nullptr;
    DeviceController* pDevCtrl = nullptr;
    std::shared_ptr<Configuration> pConf;
};


/**
    @brief Device controller's initial state is UNKNOWN.
 */
TEST_F(DeviceControllerTest, testInitialState)
{
    ASSERT_FALSE(pDevCtrl->isInitialized());

    // Initialize the controller.
    pDevCtrl->init(nullptr, nullptr, nullptr, false);
    ASSERT_TRUE(pDevCtrl->isInitialized());

    // Return false if not initialized properly.
    DeviceController dc(nullptr, nullptr);
    ASSERT_FALSE(dc.isInitialized());

    // Return false for control devices requests.
    ASSERT_FALSE(dc.controlDevices());
}


/**
    @brief Test control devices in Init status.
 */
TEST_F(DeviceControllerTest, testControlDevice)
{
    pDevCtrl->init(nullptr, nullptr, nullptr, false);

    // make sure SST is in init state.
    ASSERT_EQ(pSST->currentState(), SystemStatusTracker::eSTATE_INIT);

    // [BOOTRVC]
    pSST->updateState(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_EQ(pSST->currentState(), SystemStatusTracker::eSTATE_BOOTRVC);
}
