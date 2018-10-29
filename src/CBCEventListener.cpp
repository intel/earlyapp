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

#include <set>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "EALog.h"
#include "CBCEventListener.hpp"


// A log tag for CBCEventListener.
#define TAG "EVL"

namespace earlyapp
{
    /*
      Constructors.
    */
    CBCEventListener::CBCEventListener(void)
    {
        m_pEvDev = nullptr;
    }

    CBCEventListener::CBCEventListener(CBCEventDevice* pEvDev)
    {
        m_pEvDev = nullptr;
        if(pEvDev != nullptr)
        {
            setEventDevice(pEvDev);
        }
    }

    /*
      Destructor.
    */
    CBCEventListener::~CBCEventListener(void)
    {
    }

    /*
      Set event device.
      Returns previous event device pointer.
      nullptr will be returned if user trys to set the same event deivce
      or first time setting.
     */
    CBCEventDevice* CBCEventListener::setEventDevice(CBCEventDevice* pEvDev)
    {
        CBCEventDevice* pOldDev = m_pEvDev;

        if(pOldDev == pEvDev)
        {
            // Device is not changed.
            LINF_(TAG, "Device is has not changed");
        }
        else
        {
            m_pEvDev = pEvDev;
            LINF_(TAG, boost::str(
                      boost::format("Device has changed from %x to %x") % pOldDev % pEvDev));
        }
        return pOldDev;
    }

    /*
      Add CBCEventReceiver to subscribe events.
      Returns true for success, false otherwise.
     */
    bool CBCEventListener::addSubscriber(CBCEventReceiver* pSub)
    {
        // Subscriber adding result
        std::pair<std::set<CBCEventReceiver*>::iterator, bool> r;
        r = m_subs.insert(pSub);

        // Failed to add due to duplicatation.
        if(! r.second)
        {
            LWRN_(TAG, boost::str(
                      boost::format("Subscriber %x has not been added.") % pSub));
            return false;
        }

        return true;
    }

    /*
      Remove CBCEventReceiver to unsubscribe events.
      Returns true for success, false otherwise.
     */
    bool CBCEventListener::rmSubscriber(CBCEventReceiver* pSub)
    {
        // Number of elements.
        size_t beforeRm = m_subs.size();

        // Remove the subscriber.
        m_subs.erase(pSub);
        size_t afterRm = m_subs.size();

        if(beforeRm > afterRm)
            return true;
        return false;
    }

    /*
      Observe CBC event from the device node and update subscribers.
    */
    void CBCEventListener::observeAndNotify(bool keepObserve, long loopInterval)
    {
        if(!m_pEvDev)
        {
            LERR_(TAG, "No event device!");
            return;
        }

        boost::posix_time::time_duration li =
            boost::posix_time::milliseconds(loopInterval);

        // CBCEvent checking loop.
        // Keep checks CBC event device and notify.
        do
        {
            std::shared_ptr<CBCEvent> pEv;

            // User injected event.
            if(m_InjEv != CBCEvent::eGEARSTATUS_UNKNOWN)
            {
                LINF_(TAG, "User injected siganl: " << m_InjEv);
                pEv = std::make_shared<CBCEvent>(m_InjEv);
                notify(pEv);
                m_InjEv = CBCEvent::eGEARSTATUS_UNKNOWN;
            }
            else if((pEv = m_pEvDev->readEvent()) != nullptr)
            {
                LINF_(TAG, "Notifying CBC event.");
                notify(pEv);
            }
            else
            {
                //LINF_(TAG, "NULL for event reading.");
            }

            if(keepObserve && loopInterval > 0)
                boost::this_thread::sleep(li);
        } while(keepObserve && m_pEvDev);

    }

    /*
      Notify to subscribers.
     */
    void CBCEventListener::notify(std::shared_ptr<CBCEvent> pEv)
    {
        for(auto& i: m_subs)
        {
            i->handleCBCEvent(pEv);
        }
    }

    /*
      Inject a CBC event.
     */
    void CBCEventListener::injectEvent(CBCEvent::eCBCEvent ev)
    {
        // Is the event valid?
        if(ev == CBCEvent::eGEARSTATUS_UNKNOWN
           || ev == CBCEvent::eCBCEVENT_MIN
           || ev == CBCEvent::eCBCEVENT_MAX)
        {
            LWRN_(TAG, "Invalid event injection request has been denied.");
            return;
        }
        m_InjEv = ev;
    }

} // namespace
