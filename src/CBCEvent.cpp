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
            "UNKNWON",
            "REVERSE GEAR",
            "FORWARD GEAR",
            "APP EXIT",
            "UNDEFINED"
        };

        int idx = (int)ev;

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
