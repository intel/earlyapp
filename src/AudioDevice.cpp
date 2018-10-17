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

