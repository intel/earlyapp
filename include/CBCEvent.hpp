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
    /**
      @brief A class abstracting CBC Events.
     */
    class CBCEvent
    {
    public:
        /**
           @brief CBC event enum values.
          CBCEvent::toString() should be updated when new event added.
         */
        enum eCBCEvent
        {
            /** @brief Gear status not updated yet. */
            eGEARSTATUS_UNKNOWN,
        
            /** @brief Gear status chagned to reverse. */
            eGEARSTATUS_REVERSE,

            /** @brief Gear status changed to forward. */
            eGEARSTATUS_FORWARD,

            /** @brief Application exit. */
            eAPPLICATION_EXIT,

            /** @brief MIN index. */
            eCBCEVENT_MIN = eGEARSTATUS_UNKNOWN,

            /** @brief MAX index. */
            eCBCEVENT_MAX = eAPPLICATION_EXIT,
        };

        /**
           @brief Constructor.
           @param ev CBC event enum value.
        */
        CBCEvent(eCBCEvent ev);

        /**
           @brief Destrocutor.
        */
        virtual ~CBCEvent(void);
        
        /**
           @brief Event enum value.
        */
        eCBCEvent toEnum(void);

        /**
           @brief Returns current status's relaevant string.
           @return Relevant string for the enum CBC events.
        */
        std::string toString(void) const;

        /** 
            @brief A static member function that returns corresponding string for given CBC events.
            @param ev CBC event enum.
            @return A corresponding string for the enum value.
        */
        static std::string toString(eCBCEvent ev);

        /**
           @brief A checker whether this CBC event object is valid or not.
           @return true if the object is a valid CBC event, false otherwise.
        */
        bool isValid(void);

        /**
           @brief A static member function returns whether given CBC event enum is valid or not.
           @return true if given enum is valid CBC event, false otherwise.
        */
        static bool isValid(eCBCEvent ev);


    private:
        /**
           @brief A member funciton that hold CBC enum data.
         */
        eCBCEvent m_cbcEnumValue = eGEARSTATUS_UNKNOWN;
    };
} // namespace


