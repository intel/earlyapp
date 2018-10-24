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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <boost/format.hpp>

#include "EALog.h"
#include "CBCEvent.hpp"
#include "CBCEventDevice.hpp"

// A log tag for CBCEventDevice.
#define TAG "DEV"

namespace earlyapp
{
    CBCEventDevice::CBCEventDevice(const char* cbcDevice)
    {
        m_PollFd = {0,};

        if(cbcDevice)
        {
            LINF_(TAG, "Opening device " << cbcDevice);
            m_fdCBCDev = open(cbcDevice, O_RDWR | O_NOCTTY | O_NDELAY);

            if(m_fdCBCDev > 0)
            {
                // Initialization for CBC signal.
                const char initCmd = 0xff;
                write(m_fdCBCDev, &initCmd, 1);

                // Initialize Poll FD.
                m_PollFd.fd = m_fdCBCDev;
                m_PollFd.events = POLLIN;

                m_bOpenSuccess = true;
                LINF_(TAG, " : success");
            }
            else
            {
                m_bOpenSuccess = false;
                LERR_(TAG,
                      boost::str(
                          boost::format("Failed to open device %s (%s)")
                          % cbcDevice
                          % strerror(errno)));
            }
        }
    }

    CBCEventDevice::~CBCEventDevice(void)
    {
        if(m_fdCBCDev > 0)
        {
            close(m_fdCBCDev);
        }
    }

    /*
      openSuccessfully.
      Returns whether the CBC event device open successfully.
     */
    bool CBCEventDevice::openSuccessfully(void)
    {
        return m_bOpenSuccess;
    }

    /*
      readEvent.
      Polls the device node until reads something from it.
     */
    std::shared_ptr<CBCEvent> CBCEventDevice::readEvent(void)
    {
        if(m_fdCBCDev < 0)
        {
            LERR_(TAG, "CBC device not open");
            return nullptr;
        }

        // Poll the device node.
        if(poll(&m_PollFd, 1, CBCEVENT_POLL_INTERVAL) < 0)
        {
            LERR_(TAG, "Failed to read CBC device");

            char* errMsg = strerror(errno);
            if(errMsg)
            {
                LERR_(TAG, ": " << errMsg);
            }
            return nullptr;
        }

        // Event received.
        if(m_PollFd.revents & POLLIN)
        {
            unsigned char cbcSignalBuffer[CBCBUFFER_SIZE];
            ssize_t readChars;

            // Read error.
            if((readChars = read(m_PollFd.fd, &cbcSignalBuffer, CBCBUFFER_SIZE)) < 0)
            {
                char* errMsg = strerror(errno);
                LERR_(TAG, "Failed to read CBC signal buffer");
                if(errMsg)
                {
                    LERR_(TAG, ": " << errMsg);
                }
                return nullptr;
            }
            // Nothing happend.
            else if(readChars == 0)
            {
                return nullptr;
            }

            // Allocate a CBCEvent to be sent.
            CBCEvent::eCBCEvent cbcEv = (CBCEvent::eCBCEvent) cbcSignalBuffer[CBC_DATA_INDEX];
            switch(cbcEv)
            {
                 case 1:
                     cbcEv = CBCEvent::CBCEvent::eGEARSTATUS_REVERSE;
                     break;
                 case 3:
                     cbcEv = CBCEvent::CBCEvent::eGEARSTATUS_FORWARD;
                     break;
                 case 5:
                     cbcEv = CBCEvent::CBCEvent::eAPPLICATION_EXIT;
                     break;
                 default:
                     cbcEv = CBCEvent::CBCEvent::eGEARSTATUS_UNKNOWN;
                     LWRN_(TAG,"Wrong button, Please input correct button");
            }
            std::shared_ptr<CBCEvent> e = std::make_shared<CBCEvent>(cbcEv);

            LINF_(TAG, "Buffer: " << std::hex << cbcSignalBuffer[0] << cbcSignalBuffer[1] << cbcSignalBuffer[2] << cbcSignalBuffer[3] << cbcSignalBuffer[4] << cbcSignalBuffer[5]);
            LINF_(TAG,
                  boost::str(
                      boost::format("Got an event %s:%d")
                      %e->toString()
                      %e->toEnum()));

            return e;
        }

        return nullptr;
    }
} // namespace
