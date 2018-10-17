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


