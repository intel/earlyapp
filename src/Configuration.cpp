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


    // Default values.
    const char* Configuration::DEFAULT_AUDIO_SPLASHSOUND_PATH = "./res/countdownbeeps.wav";
    const char* Configuration::DEFAULT_AUDIO_RVCSOUND_PATH = "./res/cambeep.wav";
    const char* Configuration::DEFAULT_CAMERA_INPUTSOURCE = "ici";
    const char* Configuration::DEFAULT_VIDEO_SPLASH_PATH = "./res/countdown.mp4";
    const char* Configuration::DEFAULT_CBCDEVICE_PATH = "/dev/cbc-early-signals";
    const char* Configuration::DEFAULT_TESTCBCDEVICE_PATH = "";
    const unsigned int Configuration::DEFAULT_DISPLAY_WIDTH = DONT_CARE;
    const unsigned int Configuration::DEFAULT_DISPLAY_HEIGHT = DONT_CARE;


    // Configuration keys.
    const char* Configuration::KEY_BOOTUPSOUND = "bootup-sound";
    const char* Configuration::KEY_RVCSOUND = "rvc-sound";
    const char* Configuration::KEY_CAMERASOURCE = "camera-input";
    const char* Configuration::KEY_SPLASHVIDEO = "splash-video";
    const char* Configuration::KEY_CBCDEVICE = "cbc-device";
    const char* Configuration::KEY_TESTCBCDEVICE = "test-cbc-device";
    const char* Configuration::KEY_DISPLAYWIDTH = "width";
    const char* Configuration::KEY_DISPLAYHEIGHT = "height";



    // Return Configuration object.
    std::shared_ptr<Configuration> Configuration::makeConfiguration(int argc, char** argv)
    {
        std::shared_ptr<Configuration> conf = std::make_shared<Configuration>();

        // Initialize program options.
        conf->initProgramOptions(argc, argv);

        return std::move(conf);
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
        const std::string nullStr = std::string("null");
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

                // Camera input source.
                ("camera-input,c",
                 boost::program_options::value<std::string>()->default_value(Configuration::DEFAULT_CAMERA_INPUTSOURCE)->notifier(&checkCameraParameter),
                 "Camera input source.")

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
                 "Display height.");



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
