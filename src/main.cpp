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

#include <iostream>
#include <string>
#include <exception>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fcntl.h>

#include "EALog.h"
#include "OutputDevice.hpp"
#include "CBCEventDevice.hpp"
#include "VirtualCBCEventDevice.hpp"
#include "CBCEventListener.hpp"
#include "SystemStatusTracker.hpp"
#include "DeviceController.hpp"
#include "Configuration.hpp"
#include "GPIOControl.hpp"

#include "GStreamerApp.hpp"
#include "simple-egl.h"

// A log tag for main.
#define TAG "MAIN"

// Event device reading interval.
#define EARLYAPP_EVENT_LOOP_INTERVAL 30

// Device control interval.
#define EARLYAPP_DEVICE_LOOP_INTERVAL 20

// Handles program launching error.
void handleProgramLaunchingError(const std::exception& e)
{
    const char* errMsg = e.what();

    LERR_(TAG, "Program launching error");
    std::cerr << "ERROR: ";
    if(errMsg)
    {
        std::cerr << errMsg << std::endl;
    }
    exit(-1);
}


int main(int argc, char* argv[])
{
    int fd;
    int ret;
    char buf[8];
    bool bNeedResumeToRVC = false; 

#ifdef USE_DMESGLOG
     dmesgLogInit();
     dmesgLogPrint("EA: main");
#endif

//    fd = open(, O_CREAT | O_RDWR | O_SYNC | O_TRUNC, S_IRWXU);
//    if (fd < 0)
//    {
//           fprintf(stderr, "open %s error (%d): %m\n", "/tmp/.earlyapp.sus", errno);
//           return -1;
//    }
//    close(fd);

    /*
      Has event device open well?
     */
    bool bCBCDeviceOpen = false;

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
    if(pConf->useGStreamer())
    {
        gst_init(&argc, &argv);
    }


    /*
      Event threading.
     */
    boost::thread_group* pThreadGrp = nullptr;
    try
    {
        pThreadGrp = new boost::thread_group();
    }
    catch(const boost::thread_resource_error& e)
    {
        handleProgramLaunchingError(e);
    }

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

    // CBC event device has not open?
    bCBCDeviceOpen = (evDev->openSuccessfully()) ? true : false;
    if(! bCBCDeviceOpen)
    {
        LERR_(TAG, "CBC event device didn't open well.");
    }


    std::shared_ptr<earlyapp::CBCEvent> pEv;
    pEv = evDev->readEvent();
    /*
      Device controller - requires SystemStatusTracker.
     */
    earlyapp::DeviceController devCtrl(pConf, &ssTracker);

    try
    {
        devCtrl.init();
    }
    catch(std::bad_alloc&)
    {
        delete evDev;
        LERR_(TAG, "Failed to initialize devices.");
        return -1;
    }

    void* gp_pGPIOClass = NULL;
    gp_pGPIOClass = earlyapp::GPIOControl_create(pConf->gpioNumber(), pConf->gpioSustain());

    if (( pEv != nullptr ) && (pEv->toEnum() == earlyapp::CBCEvent::eGEARSTATUS_EGL)) {
	void* gles_pGPIOClass = NULL;
	if(pConf->gpioNumber() != pConf->NOT_SET)
        {
            gles_pGPIOClass = earlyapp::GPIOControl_create(pConf->gpioNumber(), pConf->gpioSustain());
        }

        simple_egl_main(gles_pGPIOClass);
    }

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
    boost::thread* pEvThread = nullptr;
    try
    {
        pEvThread = pThreadGrp->create_thread(
            boost::bind(
                &earlyapp::CBCEventListener::observeAndNotify,
                &evListener,
                true, EARLYAPP_EVENT_LOOP_INTERVAL));
    }
    catch(const boost::thread_resource_error& e)
    {
        handleProgramLaunchingError(e);
    }
    catch(const boost::lock_error& e)
    {
        handleProgramLaunchingError(e);
    }

    /*inject back the event to avoid missing event */
    if(pEv != nullptr)
        evListener.injectEvent(pEv->toEnum());
    /*
      Main(device) loop.
     */
    do
    {

        //LINF_(TAG, "In main thread");
	boost::this_thread::sleep(
	    boost::posix_time::milliseconds(EARLYAPP_DEVICE_LOOP_INTERVAL));

 //       fd=open("/tmp/.earlyapp.sus", O_RDWR);
        fd=open(pConf->resumesyncPath().c_str(), O_RDWR);
        if (fd < 0)
        {
	    fprintf(stderr, "open %s error (%d): %m\n", "/tmp/.earlyapp.sus", errno);
	    return -1;
        }

        ret = read(fd, buf, 1);
        if( ret == 1 )
        {
            if((buf[0] == 0x32) && ((ssTracker.currentState() == earlyapp::SystemStatusTracker::eSTATE_RVC)||(ssTracker.currentState() == earlyapp::SystemStatusTracker::eSTATE_BOOTRVC))) 
            {
                // Going to suspend
                // Inject forward gear signal to transit to Idle to close the camera streaming.
                evListener.injectEvent(earlyapp::CBCEvent::eGEARSTATUS_FORWARD);
                printf("When suspend, inject eGEARSTATUS_FORWARD to idle, buf=%d ret=%d \n", buf[0], ret);
                lseek(fd, 0x00, SEEK_SET);
                buf[0] = 0x30;
                write(fd, buf, 1);
		bNeedResumeToRVC = true;
	    }
	    else if((buf[0] == 0x31) && bNeedResumeToRVC)
	    {
                evListener.injectEvent(earlyapp::CBCEvent::eGEARSTATUS_REVERSE);
		printf("When resume, inject eGEARSTATUS_REVERSE to RVC buf=%d ret=%d \n", buf[0], ret);
		lseek(fd, 0x00, SEEK_SET);
		buf[0] = 0x30;
		write(fd, buf, 1);
		bNeedResumeToRVC = false;
	    }
        }
        close(fd);

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

        // Is still in init status?
        if(ssTracker.currentState() == earlyapp::SystemStatusTracker::eSTATE_INIT)
        {
            LINF_(TAG, "Injecting forward gear signal.");

            // Inject forward gear signal to transit to BOOTVIDEO.
            evListener.injectEvent(earlyapp::CBCEvent::eGEARSTATUS_FORWARD);
        }
        else if(! bCBCDeviceOpen)
        {
            LERR_(TAG, "Exiting due to no device.");
            bLoopCtrl = false;
            devCtrl.stopAllDevices();
            pEvThread->interrupt();
        }
    } while(bLoopCtrl);

    // Terminate devices.
    devCtrl.terminateAllDevices();

    try
    {
        // Join threads.
        pThreadGrp->join_all();
    }
    catch(const boost::lock_error&)
    {
        LWRN_(TAG, "Thread joining error.");
    }
    catch(const boost::thread_resource_error&)
    {
        LWRN_(TAG, "Thread resource error.");
    }

    // Release resources.
    delete evDev;
    delete pThreadGrp;
    LINF_(TAG, "Finishing...");

#ifdef USE_DMESGLOG
     dmesgLogClose();
#endif

    return 0;
}
