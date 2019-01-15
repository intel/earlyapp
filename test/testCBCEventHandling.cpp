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

#include <iostream>
#include <cstdio>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

#include "CBCEvent.hpp"
#include "CBCEventListener.hpp"
#include "SystemStatusTracker.hpp"

using namespace earlyapp;

/**
    @brief A mock event device.
*/
class MockEventDevice: public CBCEventDevice
{
public:
    MOCK_METHOD0(readEvent, std::shared_ptr<CBCEvent>());
};


/**
   @brief Test event listener subscription.
 */
TEST(testCBCEventLister, testSubscription)
{
    CBCEventListener evListener;

    // Mock event device.
    MockEventDevice mockEvDev;
    std::shared_ptr<CBCEvent> pRGearEvent(new CBCEvent(CBCEvent::eGEARSTATUS_REVERSE));
    std::shared_ptr<CBCEvent> pFGearEvent(new CBCEvent(CBCEvent::eGEARSTATUS_FORWARD));
    EXPECT_CALL(mockEvDev, readEvent())
        .WillOnce(::testing::Return(pRGearEvent))
        .WillOnce(::testing::Return(pFGearEvent))
        .WillOnce(::testing::Return(pRGearEvent))
        .WillOnce(::testing::Return(pFGearEvent));
    evListener.setEventDevice(&mockEvDev);

    // SystemStatusTracker.
    SystemStatusTracker sst;
    sst.init();

    /*
      Subscribe events.
     */
    bool addRes = evListener.addSubscriber(&sst);
    ASSERT_TRUE(addRes);

    // Reverse gear engaged event.
    evListener.observeAndNotify(false);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_BOOTRVC);

    // Forward gear engaged event.
    evListener.observeAndNotify(false);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);


    /*
      Unsubscribe the event shouldn't impact the gear status.
     */
    bool rmRes = evListener.rmSubscriber(&sst);
    ASSERT_TRUE(rmRes);

    // Reverse gear event shouldn't be devlivered to the subscriber.
    evListener.observeAndNotify(false);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);

    // Forward gear event
    evListener.observeAndNotify(false);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);
}


/**
   @brief Test receiving undefined events. This should keep SST unchanged.
 */
TEST(testCBCEventListener, testUndefinedEvents)
{
    CBCEventListener evListener;

    // Mock event device.
    MockEventDevice mockEvDev;
    std::shared_ptr<CBCEvent> pRGearEvent(new CBCEvent(CBCEvent::eGEARSTATUS_REVERSE));
    std::shared_ptr<CBCEvent> pFGearEvent(new CBCEvent(CBCEvent::eGEARSTATUS_FORWARD));
    EXPECT_CALL(mockEvDev, readEvent())
        .WillOnce(::testing::Return(pRGearEvent))
        .WillOnce(::testing::Return(pFGearEvent));
    evListener.setEventDevice(&mockEvDev);

    // SystemStatusTracker.
    SystemStatusTracker sst;
    // no init -> STATE is in UNKNOWN
    evListener.addSubscriber(&sst);


    // Initial gear status is UNKNOWN.
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_UNKNOWN);

    // Stays for uneverse gear engaged events.
    // REVERSE
    evListener.observeAndNotify(false);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_UNKNOWN);

    // FORWARD
    evListener.observeAndNotify(false);
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_UNKNOWN);
}


/**
   @brief Test series of grear signals and SST changes.

   System status should changes as UNKNWON -> INIT -> REVERSE -> FORWARD -> REVERSE -> REVERSE -> FORWARD -> FORWARD.
   And should keep the same states for undefined signals for the states.
 */
TEST(testCBCEventListener, testGearStatusChange)
{
    CBCEventListener evListener;

    // Mock event device.
    MockEventDevice mockEvDev;
    std::shared_ptr<CBCEvent> pRGearEvent(new CBCEvent(CBCEvent::eGEARSTATUS_REVERSE));
    std::shared_ptr<CBCEvent> pFGearEvent(new CBCEvent(CBCEvent::eGEARSTATUS_FORWARD));
    EXPECT_CALL(mockEvDev, readEvent())
        .WillOnce(::testing::Return(pRGearEvent))
        .WillOnce(::testing::Return(pFGearEvent))
        .WillOnce(::testing::Return(pRGearEvent))
        .WillOnce(::testing::Return(pRGearEvent))
        .WillOnce(::testing::Return(pFGearEvent))
        .WillOnce(::testing::Return(pFGearEvent));
    evListener.setEventDevice(&mockEvDev);


    // SystemStatusTracker.
    SystemStatusTracker sst;
    evListener.addSubscriber(&sst);


    // Initial status is UNKNOWN.
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_UNKNOWN);

    // Init
    sst.init();
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_INIT);

    // REVERSE
    // Reverse gear engaged event.
    evListener.observeAndNotify(false);
    ASSERT_TRUE(sst.isStatusChanged());
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_BOOTRVC);

    // FORWARD
    // Forward gear engaged event.
    evListener.observeAndNotify(false);
    ASSERT_TRUE(sst.isStatusChanged());
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);

    // REVERSE
    // Reverse gear engaged event.
    evListener.observeAndNotify(false);
    ASSERT_TRUE(sst.isStatusChanged());
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);

    // REVERSE
    // Reverse gear engaged event.
    evListener.observeAndNotify(false);
    ASSERT_FALSE(sst.isStatusChanged());
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_RVC);

    // FORWARD
    // Forward gear engaged event.
    evListener.observeAndNotify(false);
    ASSERT_TRUE(sst.isStatusChanged());
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);

    // FORWARD
    // Forward gear engaged event.
    evListener.observeAndNotify(false);
    ASSERT_FALSE(sst.isStatusChanged());
    ASSERT_EQ(sst.currentState(), SystemStatusTracker::eSTATE_IDLE);
}

