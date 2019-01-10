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
#include "CBCEvent.hpp"


using namespace earlyapp;

/**
    @brief Tests event validities.

    CBCEvnet constructor creates event based from enum values which could
    be integer. This test is for checking validities of given enum(int) values.
 */
TEST(CBCEventTest, testEventValidity)
{
    // Unknown
    CBCEvent unknownEv = CBCEvent(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_TRUE(unknownEv.isValid());

    // Reverse gear
    CBCEvent rEv = CBCEvent(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_TRUE(rEv.isValid());

    // Forward gear
    CBCEvent fEv = CBCEvent(CBCEvent::eGEARSTATUS_FORWARD);
    ASSERT_TRUE(fEv.isValid());

    // App exit
    CBCEvent appExitEv = CBCEvent(CBCEvent::eAPPLICATION_EXIT);
    ASSERT_TRUE(appExitEv.isValid());

    // Min
    CBCEvent minEv = CBCEvent(CBCEvent::eCBCEVENT_MIN);
    ASSERT_TRUE(minEv.isValid());

    // Max
    CBCEvent maxEv = CBCEvent(CBCEvent::eCBCEVENT_MAX);
    ASSERT_TRUE(maxEv.isValid());

    // Invalid event values.
    CBCEvent underMinEv = CBCEvent(
        ((CBCEvent::eCBCEvent)
         (static_cast<int>(CBCEvent::eCBCEVENT_MIN) - 1)));
    ASSERT_FALSE(underMinEv.isValid());

    CBCEvent overMaxEv = CBCEvent(
        ((CBCEvent::eCBCEvent)
         (static_cast<int>(CBCEvent::eCBCEVENT_MAX) + 1)));
    ASSERT_FALSE(overMaxEv.isValid());
}


/**
    @brief Test event string conversion.
 */
TEST(CBCEventTest, testStringConversionOfEvents)
{
    // Unknown
    CBCEvent unknownEv = CBCEvent(CBCEvent::eGEARSTATUS_UNKNOWN);
    ASSERT_STREQ("UNKNOWN", unknownEv.toString().c_str());
    ASSERT_STREQ("UNKNOWN", unknownEv.toString(CBCEvent::eGEARSTATUS_UNKNOWN).c_str());

    // Reverse gear
    CBCEvent rEv = CBCEvent(CBCEvent::eGEARSTATUS_REVERSE);
    ASSERT_STREQ("REVERSE GEAR", rEv.toString().c_str());
    ASSERT_STREQ("REVERSE GEAR", rEv.toString(CBCEvent::eGEARSTATUS_REVERSE).c_str());

    // Forward gear
    CBCEvent fEv = CBCEvent(CBCEvent::eGEARSTATUS_FORWARD);
    ASSERT_STREQ("FORWARD GEAR", fEv.toString().c_str());
    ASSERT_STREQ("FORWARD GEAR", fEv.toString(CBCEvent::eGEARSTATUS_FORWARD).c_str());

    // App exit
    CBCEvent appExitEv = CBCEvent(CBCEvent::eAPPLICATION_EXIT);
    ASSERT_STREQ("APP EXIT", appExitEv.toString().c_str());
    ASSERT_STREQ("APP EXIT", appExitEv.toString(CBCEvent::eAPPLICATION_EXIT).c_str());

    // Min
    CBCEvent minEv = CBCEvent(CBCEvent::eCBCEVENT_MIN);
    ASSERT_STREQ("UNKNOWN", minEv.toString().c_str());
    ASSERT_STREQ("UNKNOWN", minEv.toString(CBCEvent::eCBCEVENT_MIN).c_str());

    // Max
    CBCEvent maxEv = CBCEvent(CBCEvent::eCBCEVENT_MAX);
    ASSERT_STREQ("APP EXIT", maxEv.toString().c_str());
    ASSERT_STREQ("APP EXIT", maxEv.toString(CBCEvent::eCBCEVENT_MAX).c_str());
    ASSERT_TRUE(maxEv.isValid());

    // Invalid.
    int invalidMin = static_cast<int>(CBCEvent::eCBCEVENT_MIN) - 1;
    CBCEvent underMinEv = CBCEvent((CBCEvent::eCBCEvent) invalidMin);
    ASSERT_STREQ("UNDEFINED", underMinEv.toString().c_str());
    ASSERT_STREQ("UNDEFINED", underMinEv.toString((CBCEvent::eCBCEvent) invalidMin).c_str());

    underMinEv = CBCEvent((CBCEvent::eCBCEvent) --invalidMin);
    ASSERT_STREQ("UNDEFINED", underMinEv.toString().c_str());
    ASSERT_STREQ("UNDEFINED", underMinEv.toString((CBCEvent::eCBCEvent) invalidMin).c_str());
    underMinEv = CBCEvent((CBCEvent::eCBCEvent) --invalidMin);
    ASSERT_STREQ("UNDEFINED", underMinEv.toString().c_str());
    ASSERT_STREQ("UNDEFINED", underMinEv.toString((CBCEvent::eCBCEvent) invalidMin).c_str());


    int invalidMax = static_cast<int>(CBCEvent::eCBCEVENT_MAX) + 1;
    CBCEvent overMaxEv = CBCEvent((CBCEvent::eCBCEvent) invalidMax);
    ASSERT_STREQ("UNDEFINED", overMaxEv.toString().c_str());
    ASSERT_STREQ("UNDEFINED", overMaxEv.toString((CBCEvent::eCBCEvent) invalidMax).c_str());

    overMaxEv = CBCEvent((CBCEvent::eCBCEvent) ++invalidMax);
    ASSERT_STREQ("UNDEFINED", overMaxEv.toString().c_str());
    ASSERT_STREQ("UNDEFINED", overMaxEv.toString((CBCEvent::eCBCEvent) invalidMax).c_str());

    overMaxEv = CBCEvent((CBCEvent::eCBCEvent) ++invalidMax);
    ASSERT_STREQ("UNDEFINED", overMaxEv.toString().c_str());
    ASSERT_STREQ("UNDEFINED", overMaxEv.toString((CBCEvent::eCBCEvent) invalidMax).c_str());
}
