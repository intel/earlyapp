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

#include <string>
#include <boost/program_options.hpp>


namespace earlyapp
{
    class Configuration
    {
    public:
        /*
          Definitions.
         */
        static const int DONT_CARE;


        /*
          Default values.
        */
        static const char* DEFAULT_AUDIO_SPLASHSOUND_PATH;
        static const char* DEFAULT_AUDIO_RVCSOUND_PATH;
        static const char* DEFAULT_CAMERA_INPUTSOURCE;
        static const char* DEFAULT_VIDEO_SPLASH_PATH;
        static const char* DEFAULT_CBCDEVICE_PATH;
        static const char* DEFAULT_TESTCBCDEVICE_PATH;
        static const unsigned int DEFAULT_DISPLAY_WIDTH;
        static const unsigned int DEFAULT_DISPLAY_HEIGHT;


        /*
          Configuration KEYs.
         */
        static const char* KEY_BOOTUPSOUND;
        static const char* KEY_RVCSOUND;
        static const char* KEY_CAMERASOURCE;
        static const char* KEY_SPLASHVIDEO;
        static const char* KEY_CBCDEVICE;
        static const char* KEY_TESTCBCDEVICE;
        static const char* KEY_DISPLAYWIDTH;
        static const char* KEY_DISPLAYHEIGHT;


        /*
          Destructor.
         */
        ~Configuration(void);

        /*
          Creates and return a Configuration class object with
          given parameters.
        */
        static std::shared_ptr<Configuration> makeConfiguration(int argc, char** argv);

        /*
          Is configuration data valid?
          True - Configuration is parsed and ready.
         */
        bool isValid(void);

        /*
          Print usage.
         */
        void printUsage(void);

        /*
          Print version number.
         */
        void printVersion(char* programName);

        /*
          Returns audio splash sound path.
        */
        const std::string& audioSplashSoundPath(void);

        /*
          Returns audio RVC sound path.
        */
        const std::string& audioRVCSoundPath(void);

        /*
          Returns camera input source.
         */
        const std::string& cameraInputSource(void);

        /*
          Returns splash video path.
         */
        const std::string& videoSplashPath(void);

        /*
          Returns CBC device path.
         */
        const std::string& cbcDevicePath(void);

        /*
          Returns test cbc device if given by user otherwise nullptr.
         */
        const std::string& testCBCDevicePath(void);

        /*
          Returns display width.
         */
        unsigned int displayWidth(void) const;

        /*
          Returns display height.
         */
        unsigned int displayHeight(void) const;


    private:
        /*
          Options description.
         */
        boost::program_options::options_description* m_pDesc = nullptr;

        /*
          Option variables map.
         */
        boost::program_options::variables_map m_VM;

        /*
          Is configuration valid.
         */
        bool m_Valid = false;

        /*
          Initialize and return program options.
          True when succeed.
        */
        bool initProgramOptions(int argc, char** argv);

        /*
          Returns mapped value from the variables_map m_VM.
         */
        const std::string& stringMappedValueOf(const char* key);

        /*
          Camera option checker.
          Raises exception for not suppored camera values.
         */
        static void checkCameraParameter(std::string optStr);

        /*
          Default exception handler for option parsing.
         */
        void handleProgramOptionException(const std::exception& e);
    };
} // namespace
