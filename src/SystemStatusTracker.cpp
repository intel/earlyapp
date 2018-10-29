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

#include <boost/format.hpp>

#include "EALog.h"
#include "SystemStatusTracker.hpp"
#include "CBCEvent.hpp"

// Tag for SystemStatusTracker.
#define TAG "SST"

namespace earlyapp
{
    /*
      Initialize the device controller.
     */
    void SystemStatusTracker::init(void)
    {
        m_SysState = eSTATE_INIT;
    }

    /*
      Has gear status changed?
      This will return false once return true.
     */
    bool SystemStatusTracker::isStatusChanged(void)
    {
        if(m_statusChanged)
        {
            m_statusChanged = false;
            return true;
        }
        return false;
    }

    /*
      CBC event handler.
     */
    bool SystemStatusTracker::handleCBCEvent(std::shared_ptr<CBCEvent> pEv)
    {
        if(pEv == nullptr || ! pEv->isValid())
        {
            LWRN_(TAG, "Invalid event");
            return false;
        }

        // State transition.
        if((m_statusChanged = updateState(pEv)))
        {
            return true;
        }

        return false;
    }

    /*
      Application exit requested.
     */
    bool SystemStatusTracker::isExitRequested(void)
    {
        return m_exitReq;
    }

    /*
      currentState
      Returns current state.
     */
    SystemStatusTracker::eSystemState SystemStatusTracker::currentState(void)
    {
        eSystemState curSt;

        // Critical section (read)
        m_StateMtx.lock();
        curSt = m_SysState;
        m_StateMtx.unlock();

        return curSt;
    }

    /*
      updateState
      - True: Transition made.
      - False: Fail to transit.
     */
    bool SystemStatusTracker::updateState(std::shared_ptr<CBCEvent> pEv)
    {
        if(pEv == nullptr)
            return false;

        return updateState(pEv->toEnum());
    }

    /*
      TestCase: State transitions.

      State transition table.

      Current state | signal  | next state
      --------------+---------+------------
      INIT          | REVERSE | BOOTRVC
      INIT          | FORWARD | BOOTVIDEO
      BOOTRVC       | FORWARD | IDLE
      BOOTVIDEO     | REVERSE | RVC
      IDLE          | REVERSE | RVC
      RVC           | FORWARD | IDLE

               +---------------F-------------->+
               |                               |
        +------+-+   R     +-------+    F    +----+
     -->|  INIT  +-------> |BOOTRVC| ------> |IDLE+---+
        +--------+         +-------+     +-> +----+   |
                 |                      F|            |R
                 |   F    +---------+    +---+----+   |
                 +------->|BOOTVIDEO|   R    |RVC |<--+
                          +---------+------->+----+

      *) Signal eEXIT from all states will make transition to EXIT state.
      **) Unspecified signals will be ignored.
    */
    bool SystemStatusTracker::updateState(CBCEvent::eCBCEvent e)
    {
        eSystemState prvState = m_SysState;
        eSystemState nextState = m_SysState;

        LINF_(TAG, boost::str(
                  boost::format("Current state: %s(%d), signal: %s(%d)")
                  % stateToString() % m_SysState
                  % CBCEvent::toString(e) % e));

        switch(m_SysState)
        {
        case eSTATE_INIT:
            // REVERSE gear signal.
            if(e == CBCEvent::eGEARSTATUS_REVERSE)
            {
                nextState = eSTATE_BOOTRVC;
            }
            // FOWARD gear signal.
            else if(e == CBCEvent::eGEARSTATUS_FORWARD)
            {
                nextState = eSTATE_BOOTVIDEO;
            }
            // EXIT
            else if(e == CBCEvent::eAPPLICATION_EXIT)
            {
                nextState = eSTATE_EXIT;
            }
            break;

        case eSTATE_BOOTRVC:
            // FOWARD gear signal.
            if(e == CBCEvent::eGEARSTATUS_FORWARD)
            {
                nextState = eSTATE_IDLE;
            }
            // EXIT
            else if(e == CBCEvent::eAPPLICATION_EXIT)
            {
                nextState = eSTATE_EXIT;
            }
            break;

        case eSTATE_BOOTVIDEO:
            // REVERSE gear signal.
            if(e == CBCEvent::eGEARSTATUS_REVERSE)
            {
                nextState = eSTATE_RVC;
            }
            // EXIT
            else if(e == CBCEvent::eAPPLICATION_EXIT)
            {
                nextState = eSTATE_EXIT;
            }
            break;

        case eSTATE_IDLE:
            // REVERSE gear signal.
            if(e == CBCEvent::eGEARSTATUS_REVERSE)
            {
                nextState = eSTATE_RVC;
            }
            // EXIT
            else if(e == CBCEvent::eAPPLICATION_EXIT)
            {
                nextState = eSTATE_EXIT;
            }
            break;

        case eSTATE_RVC:
            // FORWARD gear signal.
            if(e == CBCEvent::eGEARSTATUS_FORWARD)
            {
                nextState = eSTATE_IDLE;
            }
            // EXIT
            else if(e == CBCEvent::eAPPLICATION_EXIT)
            {
                nextState = eSTATE_EXIT;
            }
            break;

        default:
            // Exit from all states.
            if(e == CBCEvent::eAPPLICATION_EXIT)
            {
                nextState = eSTATE_EXIT;
            }
            break;
        }

        // Update device state if state has changed
        if(prvState != nextState)
        {
            // Critical section(update)
            m_StateMtx.lock();
            m_SysState = nextState;
            m_StateMtx.unlock();

            LINF_(TAG, boost::str(
                      boost::format("State changed from %s(%d) -> %s(%d)")
                      % stateToString(prvState) % prvState
                      % stateToString(nextState) % nextState));

            // Application exit control.
            if(nextState == eSTATE_EXIT)
            {
                LINF_(TAG, "Application exit requested.");
                m_exitReq = true;
            }

            return true;
        }
        else
        {
            LWRN_(TAG,
                  boost::str(
                      boost::format("State won't be changed from %s(%d).")
                      % stateToString(prvState) % prvState));

            return false;
        }
    }

    /*
      stateToString
     */
    std::string SystemStatusTracker::stateToString(void)
    {
        return stateToString(m_SysState);
    }

    /*
      stateToString
     */
    std::string SystemStatusTracker::stateToString(eSystemState st)
    {
        const std::vector<const char*> strT =
        {
            "UNKNOWN",
            "INIT",
            "BOOTRVC",
            "BOOTVIDEO",
            "IDLE",
            "RVC",
            "EXIT",
            "UNDEFINED"
        };

        int idx = (int) st;
        if(st < eSTATE_MIN || st > eSTATE_MAX)
        {
            idx = (int)strT.size() - 1;
        }

        const std::string r = std::string(strT[idx]);
        return r;
    }

} // namespace
