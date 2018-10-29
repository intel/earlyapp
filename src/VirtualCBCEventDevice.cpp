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
#include <boost/format.hpp>

#include "EALog.h"
#include "VirtualCBCEventDevice.hpp"
#include "CBCEvent.hpp"


// A log tag for virtual device.
#define TAG "VDEV"

namespace earlyapp
{
    VirtualCBCEventDevice::VirtualCBCEventDevice(const char* cbcDevice)
    {
        if(cbcDevice)
        {
            LINF_(TAG, "Opening a file " << cbcDevice);
            m_pFileName = strdup(cbcDevice);
            m_fdCBCDev = open(cbcDevice, O_CREAT | O_RDWR | O_SYNC, S_IRWXU);

            if(m_pFileName != nullptr
               && m_fdCBCDev > 0)
            {
                m_bOpenSuccess = true;
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

    VirtualCBCEventDevice::~VirtualCBCEventDevice(void)
    {
        if(m_fdCBCDev > 0)
        {
            close(m_fdCBCDev);
            m_fdCBCDev = -1;
        }
        free(m_pFileName);
    }

    /*
      Reads & send event from the device node.
     */
    std::shared_ptr<CBCEvent> VirtualCBCEventDevice::readEvent(void)
    {
        if(m_fdCBCDev < 0)
        {
            LERR_(TAG, "A virtual CBC file not open");
            return nullptr;
        }

        lseek(m_fdCBCDev, 0x00, SEEK_SET);
        unsigned char cbcSignalBuffer[CBCBUFFER_SIZE];
        ssize_t r = read(m_fdCBCDev, cbcSignalBuffer, sizeof(cbcSignalBuffer));
        if( r <= 0)
        {
            // No events.
            return nullptr;
        }

        cbcSignalBuffer[r-1] = 0x00;

        // Allocate a CBCEvent to be sent.
        CBCEvent::eCBCEvent cbcEv = (CBCEvent::eCBCEvent) atoi((const char*) cbcSignalBuffer);
        std::shared_ptr<CBCEvent> e = std::make_shared<CBCEvent>(cbcEv);

        LINF_(TAG,
              boost::str(
                  boost::format("Got an event %s:%d")
                  %e->toString()
                  %e->toEnum()));

        // Erase previous data in the file.
        close(m_fdCBCDev);
        m_fdCBCDev = open(m_pFileName, O_CREAT | O_RDWR | O_SYNC | O_TRUNC, S_IRWXU);

        return e;
    }

} // namespace


