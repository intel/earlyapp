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

#include <string>


namespace earlyapp
{
    /*
      CBC Event.
     */
    class CBCEvent
    {
    public:
        /*
          CBC event enum values.
          CBCEvent::toString() should be updated when new event added.
         */
        enum eCBCEvent
        {
            // Gear status not updated yet.
            eGEARSTATUS_UNKNOWN,
        
            // Gear status chagned to reverse.
            eGEARSTATUS_REVERSE,

            // Gear status changed to forward.
            eGEARSTATUS_FORWARD,

            // Application exit.
            eAPPLICATION_EXIT,

            // MIN index
            eCBCEVENT_MIN = eGEARSTATUS_UNKNOWN,

            // MAX index
            eCBCEVENT_MAX = eAPPLICATION_EXIT,
        };

        // Constructor.
        CBCEvent(eCBCEvent ev);

        // Destrocutor.
        virtual ~CBCEvent(void);
        
        // Event enum value.
        eCBCEvent toEnum(void);

        // Returns current status's relaevant string.
        std::string toString(void) const;

        // Returs CBCEvent string.
        static std::string toString(eCBCEvent ev);

        // Is the CBCEvent is valid?
        bool isValid(void);

        // Is given CBCEvent is valid?
        static bool isValid(eCBCEvent ev);


    private:
        eCBCEvent m_cbcEnumValue = eGEARSTATUS_UNKNOWN;
    };
} // namespace


