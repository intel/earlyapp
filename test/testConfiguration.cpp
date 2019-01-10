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

using namespace earlyapp;



/**
    @brief Test Configuration class.

    Tests user given options and default values.
 */
class ConfigurationTest: public ::testing::Test
{
protected:
    void SetUp(void)
    {
        m_TestArgv = nullptr;
    }

    void TearDown(void)
    {
        if(m_TestArgv)
            delete m_TestArgv;
        m_TestArgv = nullptr;
    }

    int m_TestArgc = 0;
    char** m_TestArgv = nullptr;

    std::shared_ptr<Configuration> m_Conf;

    void updateConfiguration(void)
    {
        updateConfigurationWith({});
    }

    void updateConfigurationWith(const std::list<char*>& argvs, const char* progName=nullptr)
    {
        // Program name.
        m_TestArgc = argvs.size() + 1;

        //std::cout << "m_TestArgc: " << m_TestArgc << std::endl;
        m_TestArgv = new char* [m_TestArgc];
        m_TestArgv[0] = (progName != nullptr) ? (char*)progName : (char*)"programname";
        std::copy(argvs.begin(), argvs.end(), &m_TestArgv[1]);

        m_Conf = Configuration::makeConfiguration(m_TestArgc, m_TestArgv);
    }
};




/**
    @brief Test audio paths.
 */
TEST_F(ConfigurationTest, testAudioOptionSettings)
{
    // Default paths.
    updateConfiguration();
    ASSERT_STREQ(m_Conf->audioSplashSoundPath().c_str(), Configuration::DEFAULT_AUDIO_SPLASHSOUND_PATH);
    ASSERT_STREQ(m_Conf->audioRVCSoundPath().c_str(), Configuration::DEFAULT_AUDIO_RVCSOUND_PATH);

    // Boot up sound.
    updateConfigurationWith({
            (char*) "--bootup-sound",
            (char*) "/user/set/file/path"});
    ASSERT_STREQ(m_Conf->audioSplashSoundPath().c_str(), "/user/set/file/path");

    // RVC sound.
    updateConfigurationWith({
            (char*) "--rvc-sound",
            (char*) "/user/set/file/path"});
    ASSERT_STREQ(m_Conf->audioRVCSoundPath().c_str(), "/user/set/file/path");
}


/**
    @brief Test camera input sources.
 */
TEST_F(ConfigurationTest, testCameraOptionSettigns)
{
    // Default (ICAM)
    updateConfiguration();
    ASSERT_STREQ(m_Conf->cameraInputSource().c_str(), Configuration::DEFAULT_CAMERA_INPUTSOURCE);

    // ICAM
    updateConfigurationWith({
            (char*) "--camera-input",
            (char*) "icam"});
    ASSERT_STREQ(m_Conf->cameraInputSource().c_str(), "icam");

    // V4L2
    updateConfigurationWith({
            (char*) "--camera-input",
            (char*) "v4l2"});
    ASSERT_STREQ(m_Conf->cameraInputSource().c_str(), "v4l2");

    // Test source.
    updateConfigurationWith({
            (char*) "--camera-input",
            (char*) "test"});
    ASSERT_STREQ(m_Conf->cameraInputSource().c_str(), "test");
}


/**
    @breif Test use gstreamer option.
 */
TEST_F(ConfigurationTest, testUseGStreamerOption)
{
    updateConfigurationWith({(char*) "--use-gstreamer"});
    ASSERT_TRUE(m_Conf->useGStreamer());

    // With out the flag.
    updateConfiguration();
    ASSERT_FALSE(m_Conf->useGStreamer());
}


/**
    @brief Test custom camera option.
    Option format: --gstcamcmd <CUSTOM GSTREAMER OPTION>
 */
TEST_F(ConfigurationTest, testGSTCustomCamOption)
{
    updateConfigurationWith({
            (char*) "--use-gstreamer",
            (char*) "--gstcamcmd",
            (char*) "gst-launch plugin1 ! plugin2 x='123',y='345' ! PLUGIN3"});

    ASSERT_TRUE(m_Conf->useGStreamer());
    ASSERT_STREQ(m_Conf->gstCamCmd().c_str(), "gst-launch plugin1 ! plugin2 x='123',y='345' ! PLUGIN3");
}


/**
    @brief Test spalsh video options.
 */
TEST_F(ConfigurationTest, testSplashVideoOptionSettings)
{
    // Default path.
    updateConfiguration();
    ASSERT_STREQ(m_Conf->videoSplashPath().c_str(), Configuration::DEFAULT_VIDEO_SPLASH_PATH);

    // User set splash video path.
    updateConfigurationWith({
            (char*) "--splash-video",
            (char*) "/user/set/file/path"});
    ASSERT_STREQ(m_Conf->videoSplashPath().c_str(), "/user/set/file/path");
}


/**
    @brief Test CBC device setting options.
 */
TEST_F(ConfigurationTest, testCBCDeviceOptionSettings)
{
    // Default path.
    updateConfiguration();
    ASSERT_STREQ(m_Conf->cbcDevicePath().c_str(), Configuration::DEFAULT_CBCDEVICE_PATH);

    // User set CBC device path.
    updateConfigurationWith({
            (char*) "--cbc-device",
            (char*) "/user/set/file/path"});
    ASSERT_STREQ(m_Conf->cbcDevicePath().c_str(), "/user/set/file/path");
}


/**
    @brief Test GPIO option settings.
 */
TEST_F(ConfigurationTest, testGPIOOptionSettigns)
{
    // User set GPIO number.
    updateConfigurationWith({
            (char*) "--gpio-number",
            (char*) "440"});
    ASSERT_EQ(m_Conf->gpioNumber(), 440);

    // Not set (default).
    updateConfiguration();
    ASSERT_EQ(m_Conf->gpioNumber(), Configuration::NOT_SET);
}


/**
    @brief Test user set width/height.
 */
TEST_F(ConfigurationTest, testScreenWidthHeightOptionSettings)
{
    // Set width, no height.
    updateConfigurationWith({
            (char*) "--width",
            (char*) "400"});
    ASSERT_EQ(m_Conf->displayWidth(), (unsigned int)400);
    ASSERT_EQ(m_Conf->displayHeight(), (unsigned int) Configuration::DONT_CARE);


    // Set height, no width
    updateConfigurationWith({
            (char*) "--height",
            (char*) "400"});
    ASSERT_EQ(m_Conf->displayWidth(), (unsigned int)Configuration::DONT_CARE);
    ASSERT_EQ(m_Conf->displayHeight(), (unsigned int)400);


    // Set no height, no width
    updateConfiguration();
    ASSERT_EQ(m_Conf->displayWidth(), (unsigned int)Configuration::DONT_CARE);
    ASSERT_EQ(m_Conf->displayHeight(), (unsigned int)Configuration::DONT_CARE);


    // Set width, height
    updateConfigurationWith({
            (char*) "-w",
            (char*) "400",
            (char*) "-h",
            (char*) "200"});
    ASSERT_EQ(m_Conf->displayWidth(), (unsigned int) 400);
    ASSERT_EQ(m_Conf->displayHeight(), (unsigned int) 200);
}


/**
    @brief Test wrong / incomplete user inputs.
 */
TEST_F(ConfigurationTest, testWrongUsersetOptions)
{
    // Lack of options - bootup-sound
    updateConfigurationWith({(char*) "--bootup-sound"});
    ASSERT_FALSE(m_Conf->isValid());


    // Lack of options - rvc-sound
    updateConfigurationWith({(char*) "--rvc-sound"});
    ASSERT_FALSE(m_Conf->isValid());

    // Camera unsupported values.
    updateConfigurationWith({(char*) "--camera-input"});
    ASSERT_FALSE(m_Conf->isValid());

    // Camera unsupported values.
    updateConfigurationWith({
            (char*) "--camera-input",
            (char*) "unsupported-source"});
    ASSERT_FALSE(m_Conf->isValid());
}
