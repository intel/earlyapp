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
