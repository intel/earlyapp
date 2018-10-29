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

#include <unistd.h>
#include <string>
#include "Configuration.hpp"

namespace earlyapp
{
    /*
      GPIO control.
      Controls given GPIO number with user set values.
     */
    class GPIOControl
    {
    public:
        /*
          GPIO High / Low.
         */
        enum eGPIOValue
        {
            LOW = 0,
            HIGH = 1
        };

        /*
          Constructor.
        */
        GPIOControl(
            // GPIO number to be controlled.
            int gpioNumber = Configuration::NOT_SET,
            // Sleep value.
            useconds_t sleepTime = 1000);

        /*
          Ouput GPIO with given value.
          Returns true for success, false otherwise.
         */
        bool output(eGPIOValue highLow);

        /*
          Sleep for preset time.
         */
        void sleep(void);

        /*
          GPIO export path.
         */
        std::shared_ptr<std::string> exportPath(void) const;

        /*
          GPIO direction path.
        */
        std::shared_ptr<std::string> directionPath(void) const;

        /*
          GPIO value path.
         */
        std::shared_ptr<std::string> valuePath(void) const;

    private:
        /*
          Is the user setting valid?
         */
        bool m_Valid = false;

        /*
          GPIO number to control.
         */
        int m_GPIONumber = -1;

        /*
          User given sleep time.
         */
        useconds_t m_SleepTime = 0;

        /*
          Hide default constructor.
        */
        GPIOControl(void) { };
    };
} // namespace
