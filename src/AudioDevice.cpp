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
#include <gst/gst.h>

#include "EALog.h"
#include "OutputDevice.hpp"
#include "AudioDevice.hpp"
#include "Configuration.hpp"


// Log tag for AudioDevice.
#define TAG "AUDIO"

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

        // Create an audio device pipeline.
        m_pAudioPipeline = createPipeline(pConf);

        if(! GStreamerApp::init(m_pAudioPipeline, true))
        {
            LERR_(TAG, "Failed to init GST app.");
            return;
        }

        LINF_(TAG, "Audio device initialized");
    }


    /*
      Create a pipeline for audio device.
     */
    GstElement* AudioDevice::createPipeline(std::shared_ptr<Configuration> pConf)
    {
        LINF_(TAG, "Create audio pipeline");

        // File source.
        m_pAudioSrc = gst_element_factory_make("filesrc", nullptr);

        // Sink, parse, convert.
        m_pAlsaSink = gst_element_factory_make("alsasink", nullptr);
        m_pWavParse = gst_element_factory_make("wavparse", nullptr);
        m_pAudioCnv = gst_element_factory_make("audioconvert", nullptr);

        if(m_pAudioSrc && m_pAlsaSink && m_pWavParse && m_pAudioCnv)
        {
            // Pipeline & link
            m_pAudioPipeline = gst_pipeline_new(nullptr);
            gst_bin_add_many(
                GST_BIN(m_pAudioPipeline),
                m_pAudioSrc,
                m_pWavParse,
                m_pAudioCnv,
                m_pAlsaSink,
                nullptr);
            gst_element_link_many(m_pAudioSrc, m_pWavParse, m_pAudioCnv, m_pAlsaSink, nullptr);
        }
        else
        {
            LERR_(TAG, "Failed to create audio pipeline.");
            return nullptr;
        }

        return m_pAudioPipeline;
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
            std::string playFile = playParam->fileToPlay();
            LINF_(TAG, "*Play file* " << playFile);

            // Update audio source.
            g_object_set(G_OBJECT(m_pAudioSrc), "location", playFile.c_str(), nullptr);
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
        startPlay();
    }

    /*
      Stop.
    */
    void AudioDevice::stop(void)
    {
        LINF_(TAG, "AudioDevice stop");
        stopPlay();
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

        if(m_pAudioSrc)
        {
            gst_object_unparent(GST_OBJECT(m_pAudioSrc));
            gst_object_unref(GST_OBJECT(m_pAudioSrc));
            m_pAudioSrc = nullptr;
        }

        if(m_pAlsaSink)
        {
            gst_object_unparent(GST_OBJECT(m_pAlsaSink));
            gst_object_unref(GST_OBJECT(m_pAlsaSink));
            m_pAlsaSink = nullptr;
        }

        if(m_pWavParse)
        {
            gst_object_unparent(GST_OBJECT(m_pWavParse));
            gst_object_unref(GST_OBJECT(m_pWavParse));
            m_pWavParse = nullptr;
        }

        if(m_pAudioCnv)
        {
            gst_object_unparent(GST_OBJECT(m_pAudioCnv));
            gst_object_unref(GST_OBJECT(m_pAudioCnv));
            m_pAudioCnv = nullptr;
        }
    }
} // namespace

