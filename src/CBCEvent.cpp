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

#include <vector>

#include "CBCEvent.hpp"

namespace earlyapp
{
    /*
      Constructor.
      Initializes enum value with given parameter.
     */
    CBCEvent::CBCEvent(eCBCEvent ev)
    {
        m_cbcEnumValue = ev;
    }

    /*
      Destructor.
     */
    CBCEvent::~CBCEvent(void)
    {
    }

    /*
      toEnum
      Returns current enum value.
     */
    CBCEvent::eCBCEvent CBCEvent::toEnum(void)
    {
        return m_cbcEnumValue;
    }

    /*
      toString
      Returns string for current status enum value.
      "Undefined" will be return for invalid values.
     */
    std::string CBCEvent::toString(void) const
    {
        return toString(m_cbcEnumValue);
    }

    /*
      toString
      Returns string for given enum value.
      "Undefined" will be return for invalid values.
     */
    std::string CBCEvent::toString(eCBCEvent ev)
    {
        const std::vector<const char*> strT =
        {
            "UNKNOWN",
            "REVERSE GEAR",
            "FORWARD GEAR",
            "APP EXIT",
            "UNDEFINED"
        };

        int idx = static_cast<int>(ev);

        if(! isValid(ev))
        {
            // The last item (zero base index).
            idx = (int)strT.size() - 1;
        }


        const std::string r = std::string(strT[idx]);
        return r;
    }

    /*
      isValid
      Returns whether the event is valid or not.
     */
    bool CBCEvent::isValid(void)
    {
        return isValid(m_cbcEnumValue);
    }

    /*
      isValid
      Returns whether given eCBCEvent is valid or not.
     */
    bool CBCEvent::isValid(eCBCEvent ev)
    {
        if(ev < eCBCEVENT_MIN || ev >  eCBCEVENT_MAX)
            return false;

        return true;
    }
} // namespace
