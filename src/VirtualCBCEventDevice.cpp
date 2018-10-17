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
        size_t r = read(m_fdCBCDev, cbcSignalBuffer, sizeof(cbcSignalBuffer));
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


