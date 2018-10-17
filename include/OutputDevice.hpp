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

#include "Configuration.hpp"


namespace earlyapp
{
    /*
      A parameter class to setup start/play.
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
      OutputDevice.
      Super class for Audio, Video and Camera devices.
     */
    class OutputDevice
    {
    public:
        /*
          Initialize.
         */
        virtual void init(std::shared_ptr<Configuration> pConf) = 0;

        /*
          preparePlay
          : This will be called before play().
            Parameters for play will be given by the playParam argument.
         */
        virtual void preparePlay(std::shared_ptr<DeviceParameter> playParam=nullptr);


        /*
          play
          : Plays the device.
         */
        virtual void play(void);

        /*
          prepareStop
          : This will be called before stop().
         */
        virtual void prepareStop(void);

        /*
          stop
          : Stops the device.
         */
        virtual void stop(void);

        /*
          Terminate
          : Deallocate all resources.
         */
        virtual void terminate(void);

        /*
          Destructor.
         */
        virtual ~OutputDevice(void) { };
    };

} // namespace

