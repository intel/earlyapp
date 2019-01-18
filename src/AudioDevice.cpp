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

#include <string>
#include <boost/thread.hpp>
#include <alsa/asoundlib.h>

#include "EALog.h"
#include "OutputDevice.hpp"
#include "AudioDevice.hpp"
#include "Configuration.hpp"


// Log tag for AudioDevice.
#define TAG "AUDIO"
#define DEFAULT_PCM "default"

namespace earlyapp
{
    /*
      Define a device instance variable.
    */
    AudioDevice* AudioDevice::m_pADev = nullptr;

    /*
      Destructor.
     */
    AudioDevice::~AudioDevice(void)
    {
        if(m_pADev != nullptr)
        {
            delete m_pADev;
        }
    }

    /*
      A static function to get an instance(singleton).
    */
    AudioDevice* AudioDevice::getInstance(void)
    {
        if(m_pADev == nullptr)
        {
            LINF_(TAG, "Creating a AudioDevice instance");
            m_pADev = new AudioDevice();
        }

        return m_pADev;
    }

    /*
      Intialize
     */
    void AudioDevice::init(std::shared_ptr<Configuration> pConf)
    {
        OutputDevice::init(pConf);

        LINF_(TAG, "Audio device initialized");
    }


    /*
      preparePlay
     */
    void AudioDevice::preparePlay(std::shared_ptr<DeviceParameter> playParam)
    {
        LINF_(TAG, "AudioDevice preparePlay()");

        if(playParam != nullptr)
        {
            // Make sure previous playback stopped.
            stop();

            // Fetch a file name to play.
            m_WavFileName = playParam->fileToPlay();
            LINF_(TAG, "*Play file* " << m_WavFileName);
        }
        else
        {
            LINF_(TAG, "*Nothing to play.");
            return;
        }
        return;
    }

    /*
      Play the audio device.
     */
    void AudioDevice::play(void)
    {
        LINF_(TAG, "AudioDevice play");

        m_PlayThread = new boost::thread(playbackALSA, m_WavFileName.c_str());
    }


    /*
      Playback ALSA.
     */
    void AudioDevice::playbackALSA(const char* filePath)
    {
        // Wav file info.
        WavHeader wavInfo;

        /**
           Open the file.
        */
        FILE* fpWav = nullptr;
        if((fpWav = fopen(filePath, "rb")) == nullptr)
        {
            LERR_(TAG, "Failed to open a wav file: " << filePath);
            return;
        }

        /**
           Read header.
        */
        size_t readHeaderSize = fread(&wavInfo, 1, sizeof(WavHeader), fpWav);
        if(readHeaderSize != sizeof(WavHeader))
        {
            LERR_(TAG, "Failed to read wav header - size: " << readHeaderSize);
            return;
        }

        /**
           Start play back.
        */
        size_t buffSize = 4096;
        size_t sampleSize = 2;
        size_t readSize = 0;
        unsigned char* buff = new unsigned char[buffSize];
        int pcm = 0;
        snd_pcm_t* pALSAHandle = nullptr;
        LINF_(TAG, "Start ALSA playback");

        // Prepare ALSA device.
        for (int cnt = 0; ; cnt++) {
            if((pcm = snd_pcm_open(&pALSAHandle, DEFAULT_PCM, SND_PCM_STREAM_PLAYBACK, 0)) ==  0)
                break;
            LERR_(TAG, "Failed to open default PCM device: " << snd_strerror(pcm));
            if (cnt > 16) 
                return;
            boost::this_thread::sleep(boost::posix_time::milliseconds(200));
        }

        for (int cnt = 0; ; cnt++) {
            if((pcm = snd_pcm_set_params(
                    pALSAHandle,
                    SND_PCM_FORMAT_S16_LE,
                    SND_PCM_ACCESS_RW_INTERLEAVED,
                    wavInfo.numberOfChannels,
                    wavInfo.sampleRatePerSec,
                    1,
                    50000)) == 0)
                break;
            LERR_(TAG, "Fail to set configuration: " << snd_strerror(pcm));
            if (cnt > 16) { 
                snd_pcm_close(pALSAHandle);
                return;
            }
            boost::this_thread::sleep(boost::posix_time::milliseconds(50));
        }

        while((readSize = fread(buff, 1, buffSize, fpWav)) > 0)
        {
            snd_pcm_uframes_t frames = 0;

            if((frames = snd_pcm_writei(pALSAHandle, buff, readSize / sampleSize)) < 0)
            {
                frames = snd_pcm_recover(pALSAHandle, frames, 0);
                LERR_(TAG, "Failed to write audio data");
                return;
            }
        }
        LINF_(TAG, "Finished ALSA playback");

        snd_pcm_drain(pALSAHandle);
        snd_pcm_close(pALSAHandle);

        /**
           Close resources.
        */
        delete buff;
        fclose(fpWav);
    }

    /*
      Stop.
    */
    void AudioDevice::stop(void)
    {
        LINF_(TAG, "AudioDevice stop");

        if(m_PlayThread != nullptr)
        {
            m_PlayThread->join();
            m_PlayThread = nullptr;

            LINF_(TAG, "Thread joined");
        }
    }

    /*
      Terminate.
    */
    void AudioDevice::terminate(void)
    {
        LINF_(TAG, "AudioDevice terminate");
        releaseAudioResource();
    }

    /*
      Release resource
    */
    void AudioDevice::releaseAudioResource(void)
    {
        LINF_(TAG, "Releasing resources...");

        if(m_PlayThread != nullptr)
        {
            m_PlayThread->join();
            m_PlayThread = nullptr;
        }
    }
} // namespace

