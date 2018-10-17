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

#include <mutex>

#include "CBCEventReceiver.hpp"
#include "CBCEvent.hpp"

namespace earlyapp
{
    /*
      SystemStatusTracker
      : Tracks system status.
     */
    class SystemStatusTracker: public CBCEventReceiver
    {
    public:
        // System states
        enum eSystemState
        {
            eSTATE_UNKNOWN,
            eSTATE_INIT,
            eSTATE_BOOTRVC,
            eSTATE_BOOTVIDEO,
            eSTATE_IDLE,
            eSTATE_RVC,
            eSTATE_EXIT,
            eSTATE_MIN = eSTATE_UNKNOWN,
            eSTATE_MAX = eSTATE_EXIT
        };


        // Initializer.
        void init(void);

        // Handle CBC events.
        // Returns true if the event handled by the handler.
        bool handleCBCEvent(std::shared_ptr<CBCEvent> pEv);

        // Was there a gear status change?
        // This will return false when after returns true.
        bool isStatusChanged(void);

        // Application exit requested.
        bool isExitRequested(void);

        // Current state
        eSystemState currentState(void);

        // Make a state transition with given signal.
        // - True: transition made.
        // - False: not transited.
        bool updateState(std::shared_ptr<CBCEvent> pEv);
        bool updateState(CBCEvent::eCBCEvent e);

        // State to relevant string.
        std::string stateToString(void);
        static std::string stateToString(eSystemState st);


    private:
        // Tracks whether status changed.
        bool m_statusChanged = false;

        // System exit requested.
        bool m_exitReq = false;

        // System state
        eSystemState m_SysState = eSTATE_UNKNOWN;

        // Mutex for system state.
        std::mutex m_StateMtx;
    };
} // namespace
