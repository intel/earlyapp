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

#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "EALog.h"
#include "EAVersion.h"
#include "Configuration.hpp"


// A tag for configuration.
#define TAG "CFG"


namespace earlyapp
{
    // Definitions.
    const int Configuration::DONT_CARE = 0;
    const int Configuration::NOT_SET = -1;


    // Default values.
    const char* Configuration::DEFAULT_AUDIO_SPLASHSOUND_PATH = "/usr/share/earlyapp/jingle.wav";
    const char* Configuration::DEFAULT_AUDIO_RVCSOUND_PATH = "/usr/share/earlyapp/beep.wav";
    const char* Configuration::DEFAULT_CAMERA_INPUTSOURCE = "ici";
    const char* Configuration::DEFAULT_VIDEO_SPLASH_PATH = "/usr/share/earlyapp/splash_video.h264";
    const char* Configuration::DEFAULT_CBCDEVICE_PATH = "/dev/cbc-early-signals";
    const char* Configuration::DEFAULT_TESTCBCDEVICE_PATH = "";
    const unsigned int Configuration::DEFAULT_DISPLAY_WIDTH = DONT_CARE;
    const unsigned int Configuration::DEFAULT_DISPLAY_HEIGHT = DONT_CARE;
    const int Configuration::DEFAULT_GPIONUMBER = NOT_SET;
    const unsigned int Configuration::DEFAULT_GPIOSUSTAIN = 1;


    // Configuration keys.
    const char* Configuration::KEY_BOOTUPSOUND = "bootup-sound";
    const char* Configuration::KEY_RVCSOUND = "rvc-sound";
    const char* Configuration::KEY_CAMERASOURCE = "camera-input";
    const char* Configuration::KEY_SPLASHVIDEO = "splash-video";
    const char* Configuration::KEY_CBCDEVICE = "cbc-device";
    const char* Configuration::KEY_TESTCBCDEVICE = "test-cbc-device";
    const char* Configuration::KEY_DISPLAYWIDTH = "width";
    const char* Configuration::KEY_DISPLAYHEIGHT = "height";
    const char* Configuration::KEY_GPIONUMBER = "gpio-number";
    const char* Configuration::KEY_GPIOSUSTAIN = "gpio-sustain";



    // Return Configuration object.
    std::shared_ptr<Configuration> Configuration::makeConfiguration(int argc, char** argv)
    {
        std::shared_ptr<Configuration> conf = std::make_shared<Configuration>();

        if(conf != nullptr && conf.get() != nullptr)
        {
            // Initialize program options.
            conf->initProgramOptions(argc, argv);

            return conf;
        }
        return nullptr;
    }

    // Is valid.
    bool Configuration::isValid(void)
    {
        return m_Valid;
    }

    // Print usage.
    void Configuration::printUsage(void)
    {
        if(m_pDesc != nullptr)
        {
            std::cerr << *m_pDesc << std::endl;
        }
    }

    // Print version.
    void Configuration::printVersion(char* programName)
    {
        if(programName != nullptr)
        {
            std::cout << programName << " : ";
        }
        std::cout << "version " << EARLYAPP_PROGRAM_VERSION << std::endl;
    }

    // Audio splash sound path
    const std::string& Configuration::audioSplashSoundPath(void)
    {
        return stringMappedValueOf(Configuration::KEY_BOOTUPSOUND);
    }

    // Audio beep sound path
    const std::string& Configuration::audioRVCSoundPath(void)
    {
        return stringMappedValueOf(Configuration::KEY_RVCSOUND);
    }

    // Camera input source.
    const std::string& Configuration::cameraInputSource(void)
    {
        return stringMappedValueOf(Configuration::KEY_CAMERASOURCE);
    }

    // Video splash path.
    const std::string& Configuration::videoSplashPath(void)
    {
        return stringMappedValueOf(Configuration::KEY_SPLASHVIDEO);
    }

    // CBC device path
    const std::string& Configuration::cbcDevicePath(void)
    {
        return stringMappedValueOf(Configuration::KEY_CBCDEVICE);
    }

    // A test device.
    const std::string& Configuration::testCBCDevicePath(void)
    {
        return stringMappedValueOf(Configuration::KEY_TESTCBCDEVICE);
    }

    const std::string& Configuration::stringMappedValueOf(const char* key)
    {
        static const std::string nullStr = std::string("null");
        const std::string* valueStr;

        try
        {
            valueStr = &(m_VM[key].as<std::string>());
        }
        catch(const boost::bad_any_cast&)
        {
            LERR_(TAG, "Map error for key " << key);
            valueStr = &nullStr;
        }
        return *valueStr;
    }

    // Display width
    unsigned int Configuration::displayWidth(void) const
    {
        unsigned int w = m_VM[Configuration::KEY_DISPLAYWIDTH].as<unsigned int>();
        return w;
    }

    // Display height
    unsigned int Configuration::displayHeight(void) const
    {
        unsigned h = m_VM[Configuration::KEY_DISPLAYHEIGHT].as<unsigned int>();
        return h;
    }

    // GPIO output number
    int Configuration::gpioNumber(void) const
    {
        int gpio = m_VM[Configuration::KEY_GPIONUMBER].as<int>();
        return gpio;
    }

    // GPIO peak sustaining time in ms.
    unsigned int Configuration::gpioSustain(void) const
    {
        unsigned int peakSustain = m_VM[Configuration::KEY_GPIOSUSTAIN].as<unsigned int>();
        return peakSustain;
    }

    // Destructor.
    Configuration::~Configuration(void)
    {
        if(m_pDesc != nullptr)
        {
            delete m_pDesc;
            m_pDesc = nullptr;
        }
    }

    // Initialize program options.
    bool Configuration::initProgramOptions(int argc, char** argv)
    {
        try
        {
            m_pDesc = new boost::program_options::options_description{ "Allowed options" };

            m_pDesc->add_options()
                // Help.
                ("help", "Print usages")

                // Version.
                ("version,v", "Print version number")
/*
  Camera source option is supported with GStreamer.
 */
#ifdef USE_GSTREAMER
                // Camera input source.
                ("camera-input,c",
                 boost::program_options::value<std::string>()->default_value(Configuration::DEFAULT_CAMERA_INPUTSOURCE)->notifier(&checkCameraParameter),
                 "Camera input source.")
#endif
                // Splash video.
                ("splash-video,s",
                 boost::program_options::value<std::string>()->default_value(Configuration::DEFAULT_VIDEO_SPLASH_PATH),
                 "File path for splash video.")

                // CBC device path.
                ("cbc-device,d",
                 boost::program_options::value<std::string>()->default_value(Configuration::DEFAULT_CBCDEVICE_PATH),
                 "CBC device path.")

                // Test CBC device path.
                ("test-cbc-device,t",
                 boost::program_options::value<std::string>()->default_value(Configuration::DEFAULT_TESTCBCDEVICE_PATH),
                 "A test CBC file path.")

                // Audio path - bootup sound.
                (Configuration::KEY_BOOTUPSOUND,
                 boost::program_options::value<std::string>()->default_value(Configuration::DEFAULT_AUDIO_SPLASHSOUND_PATH),
                 "Audio file path for boot up sound.")

                // Audio path - RVC sound.
                (Configuration::KEY_RVCSOUND,
                 boost::program_options::value<std::string>()->default_value(Configuration::DEFAULT_AUDIO_RVCSOUND_PATH),
                 "Audio file path for RVC sound.")

                // Display width
                ("width,w",
                 boost::program_options::value<unsigned int>()->default_value(Configuration::DEFAULT_DISPLAY_WIDTH),
                 "Display width.")

                // Display height
                ("height,h",
                 boost::program_options::value<unsigned int>()->default_value(Configuration::DEFAULT_DISPLAY_HEIGHT),
                 "Display height.")

                // GPIO number
                (Configuration::KEY_GPIONUMBER,
                 boost::program_options::value<int>()->default_value(Configuration::DEFAULT_GPIONUMBER),
                 "GPIO number for KPI measurements. Negative values will be ignored.")

                // GPIO sustaining time.
                (Configuration::KEY_GPIOSUSTAIN,
                 boost::program_options::value<unsigned int>()->default_value(Configuration::DEFAULT_GPIOSUSTAIN),
                 "GPIO sustaining time in ms for KPI measurements.");


            boost::program_options::store(
                boost::program_options::parse_command_line(argc, argv, *m_pDesc), m_VM);
            boost::program_options::notify(m_VM);

            // Help.
            if(m_VM.count("help"))
            {
                m_Valid = false;
                printUsage();
            }
            else if(m_VM.count("version"))
            {
                printVersion(argv[0]);
            }
            // Configuration is valid. Run it.
            else
            {
                // Configuration is ready.
                m_Valid = true;
            }
        }
        catch(const boost::program_options::error& e)
        {
            handleProgramOptionException(e);
            return false;
        }
        catch(const boost::bad_lexical_cast& e)
        {
            handleProgramOptionException(e);
            return false;
        }
        return true;
    }


    // Camera option checker.
    void Configuration::checkCameraParameter(std::string optStr)
    {
        // Supported camera options.
        if(
            optStr.compare("ici") != 0
            && optStr.compare("v4l2") != 0
            && optStr.compare("test") != 0)
        {
            // Given option is not supported.
            boost::program_options::error e(
                std::string("Undefined camera input value: ")
                .append(optStr));
            throw e;
        }
    }

    // Program option parsing exception handler.
    void Configuration::handleProgramOptionException(const std::exception& e)
    {
        const char* errMsg = e.what();

        LERR_(TAG, "Option parse error");
        std::cerr << "ERROR: ";
        if(errMsg)
        {
            std::cerr << errMsg << std::endl;
        }

        // Not a valid option.
        m_Valid = false;

        if(m_pDesc)
        {
            delete m_pDesc;
            m_pDesc = nullptr;
        }
    }

} // namespace
