#
# Copyright (C) 2018 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom
# the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
# OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
#
# SPDX-License-Identifier: MIT
#

SET(CMAKE_VERBOSE_MAKEFILE true)


# GTest.
SET(GTEST_DIR ${PROJECT_SOURCE_DIR}/ext/googletest/googletest)
SET(GTEST_LIB ${GTEST_DIR}/lib/libgtest.a)
SET(GMOCK_DIR ${PROJECT_SOURCE_DIR}/ext/googletest/googlemock)
SET(GMOCK_LIB ${GMOCK_DIR}/lib/libgmock.a)


# pkg-config
FIND_PACKAGE(PkgConfig REQUIRED)

# Source files.
SET(SRC_FILES
    testmain.cpp
    testCBCEvent.cpp
    testCBCEventHandling.cpp
    testConfiguration.cpp
    testDeviceController.cpp
    testSystemStatusTracker.cpp
    testGPIOControl.cpp
    testGStreamerApp.cpp
    testOutputDevice.cpp)


# GStreamer package config.
PKG_CHECK_MODULES(GST REQUIRED gstreamer-1.0)

# Media SDK, DRM
PKG_CHECK_MODULES(MSDK REQUIRED libmfxhw64)
PKG_CHECK_MODULES(LIBDRM REQUIRED libdrm)


# Libraries
LINK_LIBRARIES(
    src
    stdc++
    pthread
    boost_log
    boost_thread
    boost_system
    boost_program_options
    msdk
    iciCam
    ${GTEST_LIB}
    ${GMOCK_LIB}
    ${GST_LIBRARIES}
    ${MSDK_LIBRARIES})


INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/ext/MediaSDK/include
    ${PROJECT_SOURCE_DIR}/ext/CameraICI/include
    ${GTEST_DIR}/include
    ${GMOCK_DIR}/include
    ${GST_INCLUDE_DIRS}
    ${MSDK_INCLUDE_DIRS}
    ${LIBDRM_INCLUDE_DIRS})


# Definitions
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)


# Build target extcutable.
ADD_EXECUTABLE(${UNITTEST_EXE} ${SRC_FILES})
