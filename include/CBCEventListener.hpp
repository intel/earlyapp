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

#include <set>

#include "CBCEventDevice.hpp"
#include "CBCEvent.hpp"
#include "CBCEventReceiver.hpp"

namespace earlyapp
{
    /**
       @brief Observes CBC event device and notify to subscribers when any event happened.
     */
    class CBCEventListener
    {
    public:
        /**
           @brief Constructor.
        */
        CBCEventListener(void);

        /**
           @brief Constructor.
           @param pEvDev Event device to listen.
        */
        CBCEventListener(CBCEventDevice* pEvDev);

        /**
           @brief Destructor.
        */
        ~CBCEventListener(void);

        /**
           @brief Set or change event device to listen.
           @param pEvDev Event device to listen.
        */
        CBCEventDevice* setEventDevice(CBCEventDevice* pEvDev);

        /**
           @brief Add CBC event receiver.
           @return true when succeed to register, false otherwise.
        */
        bool addSubscriber(CBCEventReceiver* pSub);

        /**
           @brief Remove CBC event receiver.
           @return true when succeed to remove, false otherwise.
        */
        bool rmSubscriber(CBCEventReceiver* pSub);

        /**
           @brief Observe CBC events and update subscribers.
           @param keepObserve Keep observes the device in loop.
           @param loopInterval Sleep interval for event observing.
        */
        void observeAndNotify(bool keepObserve=true, long loopInterval=0);

        /**
           @brief Inject a CBC event.
           This function doesn't keep previously injected.
           Any events not processed will be lost.
           @param ev A CBC event enum value to inject to the listener.
        */
        void injectEvent(CBCEvent::eCBCEvent ev);


    private:
        // Event device.
        CBCEventDevice* m_pEvDev = nullptr;

        // User injected event.
        CBCEvent::eCBCEvent m_InjEv = CBCEvent::eGEARSTATUS_UNKNOWN;

        // Subscribers.
        std::set<CBCEventReceiver*> m_subs;

        // Notify.
        void notify(std::shared_ptr<CBCEvent> pEv);
    };

} // namespace

