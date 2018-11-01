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

#pragma once

#include <poll.h>
#include <memory>
#include "CBCEvent.hpp"


// Time interval for checking CBC events(ms)
#define CBCEVENT_POLL_INTERVAL 10

// CBC read buffer size.
#define CBCBUFFER_SIZE 100

// CBC early signal data index.
// : An index for a signal that being used for the application.
#define CBC_DATA_INDEX 3

namespace earlyapp
{
    /**
      @brief Abstracts CBC Event device.
     */
    class CBCEventDevice
    {
    public:
        /**
           @brief Constructor.
           @param cbcDevice Path for CBC device node.
        */
        CBCEventDevice(const char* cbcDevice=nullptr);

        /**
           @brief Destructor.
        */
        virtual ~CBCEventDevice(void);

        /**
           @brief Is CBC event device open successfuly?
           @return true if the device successfully, false otherwise.
        */
        bool openSuccessfully(void);

        /**
           @brief Read CBC event from the device (file).
           @return Returns received CBC event.
        */
        virtual std::shared_ptr<CBCEvent> readEvent(void);

    protected:
        /**
           @brief File descriptor for the CBC device file.
         */
        int m_fdCBCDev = -1;

        /**
           @brief Device open success flag.
        */
        bool m_bOpenSuccess = false;

    private:
        /**
           @brief Poll FD.
        */
        struct pollfd m_PollFd;
    };
} // namespace


