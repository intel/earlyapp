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

#ifdef USE_LOGOUTPUT
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

// Informative logs.
#define LINF_(tag, str) \
    BOOST_LOG_TRIVIAL(info) \
    << "[" << tag << "] " \
    << str

// Debug logs.
#define LDBG_(tag, str) \
    BOOST_LOG_TRIVIAL(debug) \
    << "[" << tag << "] " \
    << str

// Warning logs.
#define LWRN_(tag, str) \
    BOOST_LOG_TRIVIAL(warning) \
    << "[" << tag << "] " \
    << str

// Error logs.
#define LERR_(tag, str) \
    BOOST_LOG_TRIVIAL(error) \
    << "[" << tag << "] " \
    << str

#else
// No log output
#define LINF_(tag, str)
#define LDBG_(tag, str)
#define LWRN_(tag, str)
#define LERR_(tag, str)

#endif // USE_LOGOUTPUT

#ifdef USE_DMESGLOG
//Init demsg log
int dmesgLogInit(void);

//Log message to dmesg log
int dmesgLogPrint(const char*);

//Close demsg log
int dmesgLogClose(void);

#endif

