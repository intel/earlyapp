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
    /**
       @brief Configuration for the application that set from on user given parameters.
    */
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
        static const useconds_t DEFAULT_GPIOSUSTAIN;
        static const bool DEFAULT_USE_GSTREAMER;


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
        static const char* KEY_GPIOSUSTAIN;
        static const char* KEY_USEGSTREAMER;


        /**
           @brief Constructor.
        */
        Configuration(void) = default;

        /**
          @brief Destructor.
         */
        ~Configuration(void);

        /*
          @brief Creates and return a Configuration class object with given parameters.
        */
        static std::shared_ptr<Configuration> makeConfiguration(int argc, char** argv);

        /**
           @brief Is configuration data valid?
           @return true if Configuration is parsed and ready.
         */
        bool isValid(void);

        /**
          @brief Print usage.
         */
        void printUsage(void);

        /**
          @brief Print version number.
         */
        void printVersion(char* programName);

        /**
          @brief Returns audio splash sound path.
        */
        const std::string& audioSplashSoundPath(void);

        /**
          @brief Returns audio RVC sound path.
        */
        const std::string& audioRVCSoundPath(void);

        /**
          @brief Returns camera input source.
         */
        const std::string& cameraInputSource(void);

        /**
          @brief Returns splash video path.
         */
        const std::string& videoSplashPath(void);

        /**
          @brief Returns CBC device path.
         */
        const std::string& cbcDevicePath(void);

        /**
          @brief Returns test cbc device if given by user otherwise nullptr.
         */
        const std::string& testCBCDevicePath(void);

        /**
          @brief Returns display width.
         */
        unsigned int displayWidth(void) const;

        /**
          @brief Returns display height.
         */
        unsigned int displayHeight(void) const;

        /**
          @brief Returns user set output GPIO.
         */
        int gpioNumber(void) const;

        /**
          @brief Returns GPIO peak sustaining time in ms.
         */
        unsigned int gpioSustain(void) const;

        /**
           @brief Returns whether user asked to use GStreamer.
        */
        bool useGStreamer(void) const;

        /**
           @brief Disable copy assigned operators.
        */
        Configuration& operator=(const Configuration&) = delete;
        Configuration(const Configuration&) = delete;


    private:
        /**
          @brief Options description.
         */
        boost::program_options::options_description* m_pDesc = nullptr;

        /**
          @brief Option variables map.
         */
        boost::program_options::variables_map m_VM;

        /**
          @brief Is configuration valid.
         */
        bool m_Valid = false;

        /**
          @brief Initialize and return program options.
          @return True when succeed.
        */
        bool initProgramOptions(int argc, char** argv);

        /**
          @brief Returns mapped value from the variables_map m_VM.
         */
        const std::string& stringMappedValueOf(const char* key);

        /**
          @brief Camera option checker.
          Raises exception for not suppored camera values.
         */
        static void checkCameraParameter(std::string optStr);

        /**
           @brief Default exception handler for option parsing.
         */
        void handleProgramOptionException(const std::exception& e);
    };
} // namespace
