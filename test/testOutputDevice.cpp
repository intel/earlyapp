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

#include "Configuration.hpp"

#include "GstAudioDevice.hpp"
#include "GstVideoDevice.hpp"
#include "GstCameraDevice.hpp"

#include "AudioDevice.hpp"
#include "VideoDevice.hpp"
#include "CameraDevice.hpp"


using namespace earlyapp;


/**
   @brief Test device names.
 */
TEST(OutputDeviceTest, deviceNames)
{
    OutputDevice* pAud = AudioDevice::getInstance();
    ASSERT_STREQ(pAud->deviceName(), "ALSA Audio");

    OutputDevice* pVid = VideoDevice::getInstance();
    ASSERT_STREQ(pVid->deviceName(), "MSDK Video");

    OutputDevice* pCam = CameraDevice::getInstance();
    ASSERT_STREQ(pCam->deviceName(), "ICI Camera");

    OutputDevice* pGstAud = GstAudioDevice::getInstance();
    ASSERT_STREQ(pGstAud->deviceName(), "Gst Audio");

    OutputDevice* pGstVid = GstVideoDevice::getInstance();
    ASSERT_STREQ(pGstVid->deviceName(), "Gst Video");

    OutputDevice* pGstCam = GstCameraDevice::getInstance();
    ASSERT_STREQ(pGstCam->deviceName(), "Gst Camera");
}


/**
    @brief Test custom camera command test.
 */
const char* SIMPLE_GSTCAMCMD = "v4l2src ! fakesink";
const char* ICAMSRC_GSTCAMCMD = "icamerasrc device-name=mondello-3 input-height=480 input-width=720 input-format=UYVY deinterlace-method=3 interlace-mode=7 ! 'video/x-raw,format=NV12,height=1080,width=1920' ! vaapipostproc !  waylandsink";

TEST(OutputDeviceTest, gstCameraCustomCommand)
{
    char* argv[] =
    {
        (char*) "programname",
        (char*) "--gstcamcmd",
        (char*) SIMPLE_GSTCAMCMD
    };
    int argc = 3;

    // GStreamer requires initialization before been used.
    gst_init(nullptr, (char***)nullptr);

    std::shared_ptr<Configuration> pConf = Configuration::makeConfiguration(argc, argv);
    OutputDevice* pOutputGstCam = GstCameraDevice::getInstance();
    GstCameraDevice* pGstCam = static_cast<GstCameraDevice*>(pOutputGstCam);
    ASSERT_NE(pGstCam, nullptr);


    pGstCam->init(pConf);
    GstElement* pPipeline = pGstCam->gstPipeline();
    ASSERT_NE(pPipeline, nullptr);

    // Icamerasrc.
    gst_init(nullptr, (char***)nullptr);
    argv[2] = (char*) ICAMSRC_GSTCAMCMD;
    pConf = Configuration::makeConfiguration(argc, argv);
    pGstCam->init(pConf);
    GstElement* pNewPipeline = pGstCam->gstPipeline();
    EXPECT_NE(pPipeline, nullptr);
    ASSERT_NE(pNewPipeline, pPipeline);
}
