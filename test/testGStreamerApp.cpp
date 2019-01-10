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

#include <gtest/gtest.h>
#include "Configuration.hpp"
#include "GStreamerApp.hpp"
#include "VideoDevice.hpp"

using namespace earlyapp;

/*
  Test GStreamerApp class.
 */
class GStreamerAppTest: public ::testing::Test
{

};

/*
  Test of creating pipelines.
 */
TEST_F(GStreamerAppTest, testGstInitialization)
{
#if 0
    const char* gstLaunch = "videotestsrc ! waylandsink";

    GStreamerApp g;
    bool initResult = g.init(gstLaunch, false);
    ASSERT_TRUE(initResult);
#endif
}

/*
  VideoDevice
 */
TEST_F(GStreamerAppTest, testVideoDevice)
{

}

/*
  Null capsfilter linking won't be fails.
 */
TEST_F(GStreamerAppTest, testNullCapsfilterAndUnref)
{
#if 0
    GstElement* videoScaler = gst_element_factory_make("videoscale", nullptr);
    GstElement* scaleFilter = gst_element_factory_make("capsfilter", nullptr);

    if(! gst_element_link_pads_filtered(videoScaler, "src", scaleFilter, "sink", nullptr))
    {
        // Fail, should be reached here even for nullptr linkings.
        ASSERT_TRUE(false);
    }
    else
    {
        // Success.
        ASSERT_TRUE(true);
    }

    ASSERT_NE(videoScaler, nullptr);
    ASSERT_NE(scaleFilter, nullptr);
#endif
}
