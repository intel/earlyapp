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


#include <gtest/gtest.h>
#include <gtest/gtest.h>

#include "SystemStatusTracker.hpp"
#include "CBCEvent.hpp"

using namespace earlyapp;


/**
   @brief Test initial state is UNKNOWN.
 */
TEST(SystemStatusTrackerTest, testInitialState)
{
    SystemStatusTracker sst;
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_UNKNOWN);

    // Initialize the controller.
    sst.init();
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_INIT);
}


/**
  @brief TestCase: State transitions.

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

/**
   @brief Transition cycle 1.

   INIT -R-> BOOTRVC -F-> IDLE -R-> RVC -F-> IDLE -R-> RVC -F-> IDLE
 */
TEST(SystemStatusTrackerTest, testInitStateTransition)
{
    // Transition has been made successfully?
    bool trSucceed = false;
    SystemStatusTracker sst;

    // Initialize.
    sst.init();

    // [INIT state]
    // No transitions for undefined signals.
    // UNKNOWN
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_INIT);
    
    // eREVERSE will change to BOOTRVC.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_BOOTRVC);


    // [BOOTRVC state]
    // No transition for undefined signals.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_BOOTRVC);
    // UNKNOWN
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_BOOTRVC);    

    // eFORWARD will change to IDLE.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_FORWARD);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);

    // [IDLE state]
    // No transition for undefined signals.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_FORWARD);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);
    // UNKNOWN
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);

    // eREVERSE will change to RVC.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);

    // [RVC state]
    // No transition for undefined signals.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);
    // UNKNOWN
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);

    // eFORWARD will change to IDLE.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_FORWARD);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);

}


/**
    @brief Transition cycle 2.

    INIT -F-> BOOTVIDEO -R-> RVC -F-> IDLE -R-> RVC
 */
TEST(SystemStatusTrackerTest, testInitStateTransition2)
{
    // Transition has been made successfully?
    bool trSucceed = false;
    SystemStatusTracker sst;

    // Initialize.
    sst.init();

    // [INIT state]
    // No transitions for undefined signals.
    // UNKNOWN
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_INIT);
    
    // eFORWARD will change to BOOTVIDEO.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_FORWARD);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_BOOTVIDEO);


    // [BOOTVIDEO state]
    // No transition for undefined signals.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_FORWARD);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_BOOTVIDEO);
    // UNKNOWN
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_BOOTVIDEO);

    // eREVERSE will change to RVC.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);

    // [RVC state]
    // No transition for undefined signals.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);
    // UNKNOWN
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);

    // eFORWARD will change to IDLE.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_FORWARD);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);

    // [IDLE state]
    // No transition for undefined signals.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_FORWARD);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);
    // UNKNOWN
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);

    // eREVERSE will change to RVC.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);


    // [RVC state]
    // No transition for undefined signals.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);
    // UNKNOWN
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_FALSE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);

    // eREVERSE will change to RVC.
    trSucceed = sst.updateState(CBCEvent::eGEARSTATUS_FORWARD);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);

}


/**
   @brief Transition to exit from init state.
 */
TEST(SystemStatusTrackerTest, testExitFromINIT)
{
    SystemStatusTracker sst;
    // Initialize.
    sst.init();

    // [INIT state]
    // eAPPLICATION_EXIT will change to EXIT.
    bool trSucceed = sst.updateState(CBCEvent::eAPPLICATION_EXIT);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_EXIT);
}


/**
   @brief Transition to exit from BOOTRVC.
 */
TEST(SystemStatusTrackerTest, testExitFromBOOTRVC)
{
    SystemStatusTracker sst;

    // Initialize.
    sst.init();

    // eAPPLICATION_EXIT will change to EXIT.
    sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);
    bool trSucceed = sst.updateState(CBCEvent::eAPPLICATION_EXIT);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_EXIT);
}


/**
   @brief Transition to exit from IDLE.
 */
TEST(SystemStatusTrackerTest, testExitFromIDLE)
{
    SystemStatusTracker sst;

    // Initialize.
    sst.init();

    // eAPPLICATION_EXIT will change to EXIT.
    sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);
    sst.updateState(CBCEvent::eGEARSTATUS_FORWARD);
    bool trSucceed = sst.updateState(CBCEvent::eAPPLICATION_EXIT);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_EXIT);
}


/**
   @brief Transition to exit from RVC.
 */
TEST(SystemStatusTrackerTest, testExitFromRVC)
{
    SystemStatusTracker sst;

    // Initialize.
    sst.init();

    // eAPPLICATION_EXIT will change to EXIT.
    sst.updateState(CBCEvent::eGEARSTATUS_FORWARD);
    sst.updateState(CBCEvent::eGEARSTATUS_REVERSE);    
    bool trSucceed = sst.updateState(CBCEvent::eAPPLICATION_EXIT);
    ASSERT_TRUE(trSucceed);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_EXIT);
}


/**
   @brief State's name to string.
 */
TEST(SystemStatusTrackerTest, testToStringName)
{
    SystemStatusTracker sst;

    // Initialize.
    sst.init();

    ASSERT_STREQ("UNKNOWN", sst.stateToString(SystemStatusTracker::eSTATE_UNKNOWN).c_str());
    ASSERT_STREQ("INIT", sst.stateToString(SystemStatusTracker::eSTATE_INIT).c_str());
    ASSERT_STREQ("BOOTRVC", sst.stateToString(SystemStatusTracker::eSTATE_BOOTRVC).c_str());
    ASSERT_STREQ("BOOTVIDEO", sst.stateToString(SystemStatusTracker::eSTATE_BOOTVIDEO).c_str());
    ASSERT_STREQ("IDLE", sst.stateToString(SystemStatusTracker::eSTATE_IDLE).c_str());
    ASSERT_STREQ("RVC", sst.stateToString(SystemStatusTracker::eSTATE_RVC).c_str());
    ASSERT_STREQ("EXIT", sst.stateToString(SystemStatusTracker::eSTATE_EXIT).c_str());
    ASSERT_STREQ("UNKNOWN", sst.stateToString(SystemStatusTracker::eSTATE_MIN).c_str());
    ASSERT_STREQ("EXIT", sst.stateToString(SystemStatusTracker::eSTATE_MAX).c_str());

    // Invalid states.
    int underMinState = static_cast<int>(SystemStatusTracker::eSTATE_MIN) - 1;
    ASSERT_STREQ("UNDEFINED", sst.stateToString((SystemStatusTracker::eSystemState)underMinState).c_str());
    ASSERT_STREQ("UNDEFINED", sst.stateToString((SystemStatusTracker::eSystemState)--underMinState).c_str());
    ASSERT_STREQ("UNDEFINED", sst.stateToString((SystemStatusTracker::eSystemState)--underMinState).c_str());

    int overMaxState = static_cast<int>(SystemStatusTracker::eSTATE_MAX) + 1;
    ASSERT_STREQ("UNDEFINED", sst.stateToString((SystemStatusTracker::eSystemState)overMaxState).c_str());
    ASSERT_STREQ("UNDEFINED", sst.stateToString((SystemStatusTracker::eSystemState)++overMaxState).c_str());
    ASSERT_STREQ("UNDEFINED", sst.stateToString((SystemStatusTracker::eSystemState)++overMaxState).c_str());    
}
