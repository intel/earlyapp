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

#include <set>

#include "CBCEventDevice.hpp"
#include "CBCEvent.hpp"
#include "CBCEventReceiver.hpp"

namespace earlyapp
{
    /*
      CBCEventListener
      : Observes CBC event device and notify to subscribers when any event happened.
     */
    class CBCEventListener
    {
    public:
        // Constructor/destructor.
        CBCEventListener(void);
        CBCEventListener(CBCEventDevice* pEvDev);
        ~CBCEventListener(void);

        // Set event device.
        CBCEventDevice* setEventDevice(CBCEventDevice* pEvDev);

        // Add CBC event receiver.
        bool addSubscriber(CBCEventReceiver* pSub);

        // Remove CBC event receiver.
        bool rmSubscriber(CBCEventReceiver* pSub);

        // Observe CBC events and update subscribers.
        //  - keepObserve: Keep observes the device in loop.
        //  - loopInterval: Sleep interval for event observing.
        void observeAndNotify(bool keepObserve=true, long loopInterval=0);


    private:
        // Event device.
        CBCEventDevice* m_pEvDev;

        // Subscribers.
        std::set<CBCEventReceiver*> m_subs;

        // Notify.
        void notify(std::shared_ptr<CBCEvent> pEv);
    };

} // namespace

