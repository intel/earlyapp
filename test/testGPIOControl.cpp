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
#include "GPIOControl.hpp"

using namespace earlyapp;


/**
   @brief Test GPIO paths.
 */
TEST(TestGPIOControl, testGPIOPaths)
{
    // Test GPIO 442 control with 1000usec sleep time.
    GPIOControl g = GPIOControl(442, 1000);

    // Export path
    ASSERT_STREQ(g.exportPath()->c_str(), "/sys/class/gpio/export");

    // Direction path
    ASSERT_STREQ(g.directionPath()->c_str(), "/sys/class/gpio/gpio442/direction");

    // Vlaue path
    ASSERT_STREQ(g.valuePath()->c_str(), "/sys/class/gpio/gpio442/value");
}
