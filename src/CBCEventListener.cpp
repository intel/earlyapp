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
        setEventDevice(pEvDev);
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

            //LINF_(TAG, "Checking CBC event device.");
            if((pEv = m_pEvDev->readEvent()) != nullptr)
            {
                LINF_(TAG, "Notifying CBC event.");
                notify(pEv);
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
        for(auto i=m_subs.begin(); i != m_subs.end(); ++i)
        {
            (*i)->handleCBCEvent(pEv);
        }
    }

} // namespace
