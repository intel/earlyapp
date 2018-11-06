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

#include "Configuration.hpp"
#include "GPIOControl.hpp"

namespace earlyapp
{
    /**
      @brief A parameter class to setup start/play. 
      Make sub-classes when concrete class need device specific parameters.
    */
    class DeviceParameter
    {
    public:
        // Constructor.
        DeviceParameter(void);
        DeviceParameter(const char* filePath);
        DeviceParameter(const std::string& filePath);

        // Sets file to be played.
        virtual void setFileToPlay(const char* filePath);
        virtual void setFileToPlay(const std::string& filePath);

        // Returns file to be played.
        virtual std::string& fileToPlay(void);

    private:
        std::string m_sFilePath;
    };


    /*
      @brief Super class for Audio, Video and Camera devices 
      that provides common device control interface.
     */
    class OutputDevice
    {
    public:
        /**
          @brief Initialize the device.
          @param pConf User set configuration for the application.
         */
        virtual void init(std::shared_ptr<Configuration> pConf);

        /**
          @brief This will be called before play().
          Parameters for play will be given by the playParam argument.
         */
        virtual void preparePlay(std::shared_ptr<DeviceParameter> playParam=nullptr);


        /**
          @brief Play the device.
         */
        virtual void play(void);

        /**
          @brief This member function will be called before stop().
         */
        virtual void prepareStop(void);

        /**
          @brief Stop the device.
         */
        virtual void stop(void);

        /**
          @brief Terminate the device and deallocate all resources.
         */
        virtual void terminate(void);

        /**
          @brief Destructor.
         */
        virtual ~OutputDevice(void);

        /**
          @brief GPIO output for KPI measurements.
         */
        void outputGPIOPattern(void);

        /**
           @brief Disable copy assigned operators.
        */
        OutputDevice& operator=(const OutputDevice&) = delete;
        OutputDevice(const OutputDevice&) = delete;
        OutputDevice(void) = default;

    private:
        /**
          @brief GPIO control, nullptr if user didn't provide GPIO control option.
        */
        GPIOControl* m_pGPIOCtrl = nullptr;
    };
} // namespace

