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
#include <boost/thread.hpp>

#include "OutputDevice.hpp"
#include "Configuration.hpp"


namespace earlyapp
{
    /**
      @brief A class abstracts audio playback device.
     */
    class AudioDevice: public OutputDevice
    {
    public:
        /**
           @brief Returns audio device singlton instance.
         */
        static AudioDevice* getInstance(void);

        /**
           @brief Initializes the audio device.
           @param pConf User set configurations.
         */
        void init(std::shared_ptr<Configuration> pConf);

        /**
           @brief Prepare for playback audio file.
           @param playParam Parameters for audio playback including file path.
        */
        void preparePlay(std::shared_ptr<DeviceParameter> playParam=nullptr);

        /**
           @brief Play the audio device.
         */
        void play(void);

        /**
           @brief Stop the audio device.
        */
        void stop(void);

        /**
           @brief Terminate the device and eturn all resources.
        */
        void terminate(void);

        /**
           @brief Destructor.
        */
        virtual ~AudioDevice(void);


    private:
        /**
           @brief Wave file header.
         */
        typedef struct WAV_HEADER
        {
            /* RIFF Chunk Descriptor */
            unsigned char   riff[4];           // RIFF
            unsigned int    chunkSize;         // RIFF Chunk Size
            unsigned char   wave[4];           // WAVE Header
            /* "fmt" sub-chunk */
            unsigned char   subchunk1ID[4];    // FMT header
            unsigned int    subchunk1Size;     // Size of the fmt chunk
            unsigned short  audioFormat;       // Audio format 1=PCM,6=mulaw,7=alaw,257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
            unsigned short  numberOfChannels;  // Number of channels 1=Mono 2=Sterio
            unsigned int    sampleRatePerSec;  // Sampling Frequency in Hz
            unsigned int    byteRatePerSec;    // bytes per second
            unsigned short  blockAlign;        // 2=16-bit mono, 4=16-bit stereo
            unsigned short  bitsPerSample;     // Number of bits per sample
            /* "data" sub-chunk */
            unsigned char   subchunk2ID[4];    // "data"  string
            unsigned int    subchunk2Size;     // Sampled data length
        } WavHeader;

        // Hide the default constructor to prevent instancitating.
        AudioDevice(void) { OutputDevice::m_pDevName = "ALSA Audio"; }

        // Program configuration.
        std::shared_ptr<Configuration> m_pConf;

        static  AudioDevice* m_pADev;

        /**
           @brief Playback file name.
         */
        std::string m_WavFileName;

        /*
          @brief ALSA playback thread.
         */
        boost::thread* m_PlayThread = nullptr;

        /**
          @brief Releases audio resources except pipeline.
         */
        void releaseAudioResource(void);

        /**
           @brief A thread function to palyback using ALSA.
         */
        static void playbackALSA(const char* filePath);
    };
} // namespace
