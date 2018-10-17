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
