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
            CBCEvent::eCBCEvent cbcEv = CBCEvent::eGEARSTATUS_UNKNOWN;
            unsigned char cbcDataPayload = cbcSignalBuffer[CBC_DATA_INDEX];
            switch(cbcDataPayload)
            {
                 case 0x01:
                     cbcEv = CBCEvent::eGEARSTATUS_REVERSE;
                     break;
                 case 0x03:
                     cbcEv = CBCEvent::eGEARSTATUS_FORWARD;
                     break;
                 case 0x05:
                     cbcEv = CBCEvent::eAPPLICATION_EXIT;
                     break;
		 case 0x02:
		     cbcEv = CBCEvent::eGEARSTATUS_EGL;
		     break;
                 default:
                     cbcEv = CBCEvent::eGEARSTATUS_UNKNOWN;
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
