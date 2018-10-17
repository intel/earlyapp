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

#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "EALog.h"
#include "OutputDevice.hpp"
#include "CBCEventDevice.hpp"
#include "VirtualCBCEventDevice.hpp"
#include "CBCEventListener.hpp"
#include "SystemStatusTracker.hpp"
#include "DeviceController.hpp"
#include "Configuration.hpp"

// A log tag for main.
#define TAG "MAIN"

// TODO: REMOVE ME - DEBUG FEATURE.
#define DEBUG_EVENT_LOOP_INTERVAL 200
#define DEBUG_DEVICE_LOOP_INTERVAL 200


int main(int argc, char* argv[])
{
    /*
      Program options.
     */
    std::shared_ptr<earlyapp::Configuration> pConf =
        earlyapp::Configuration::makeConfiguration(argc, argv);
    if(! pConf->isValid())
    {
        return -1;
    }

    /*
      GStreamer.
     */
    gst_init(&argc, &argv);

    /*
      Event threading.
     */
    boost::thread_group threadGrp;

    /*
      Start event tracker.
     */
    earlyapp::CBCEventDevice* evDev = nullptr;
    if(pConf->testCBCDevicePath() == "")
    {
        evDev = new earlyapp::CBCEventDevice(pConf->cbcDevicePath().c_str());
    }
    else
    {
        evDev = new earlyapp::VirtualCBCEventDevice(pConf->testCBCDevicePath().c_str());
    }
    earlyapp::CBCEventListener evListener;
    earlyapp::SystemStatusTracker ssTracker;
    ssTracker.init();

    // Exit - no event device available.
    if(!evDev->openSuccessfully())
    {
        LERR_(TAG, "Exit");
        return -1;
    }

    /*
      Device controller - requires SystemStatusTracker.
     */
    earlyapp::DeviceController devCtrl(pConf, &ssTracker);
    devCtrl.init();


    /*
      Loop control.
      Set it false to exit the loop.
     */
    bool bLoopCtrl = true;


    /*
      Event listenr (event loop)
     */
    // Set event device.
    evListener.setEventDevice(evDev);

    // System status tracker subscribes CBC events.
    evListener.addSubscriber(&ssTracker);


    /*
      Start an event loop in thread.
     */
    boost::thread* pEvThread = threadGrp.create_thread(
        boost::bind(
            &earlyapp::CBCEventListener::observeAndNotify,
            &evListener,
            true, DEBUG_EVENT_LOOP_INTERVAL));


    /*
      Main(device) loop.
     */
    do
    {
        // Was there a status change?
        if(ssTracker.isStatusChanged())
        {
            LINF_(TAG, "Status changed.");

            if(ssTracker.isExitRequested())
            {
                LINF_(TAG, "Exiting");
                bLoopCtrl = false;
                devCtrl.stopAllDevices();
                pEvThread->interrupt();
            }
            // Switching from forward gear to reverse.
            else
            {
                devCtrl.controlDevices();
            }
        }

        //LINF_(TAG, "In main thread");
        boost::this_thread::sleep(
            boost::posix_time::milliseconds(DEBUG_DEVICE_LOOP_INTERVAL));
    } while(bLoopCtrl);

    // Terminate devices.
    devCtrl.terminateAllDevices();


    // Join threads.
    threadGrp.join_all();

    // Release resource.
    if(evDev != nullptr)
    {
        delete evDev;
    }
    LINF_(TAG, "Finishing...");

    return 0;
}
