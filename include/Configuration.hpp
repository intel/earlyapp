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
        static const int NOT_SET;


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
        static const int DEFAULT_GPIONUMBER;


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
        static const char* KEY_GPIONUMBER;


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

        /*
          Returns user set output GPIO.
         */
        int gpioNumber(void) const;


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
