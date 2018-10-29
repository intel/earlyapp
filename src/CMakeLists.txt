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


# Build configuration. (Debug/Release)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_VERBOSE_MAKEFILE true)


# Source files.
SET(EXE_MAIN main.cpp)
SET(SRC_FILES
    AudioDevice.cpp
    CameraDevice.cpp
    CBCEvent.cpp
    CBCEventDevice.cpp
    CBCEventListener.cpp
    CBCEventReceiver.cpp
    Configuration.cpp
    DeviceController.cpp
    GStreamerApp.cpp
    GPIOControl.cpp
    OutputDevice.cpp
    SystemStatusTracker.cpp
    VideoDevice.cpp
    VirtualCBCEventDevice.cpp
    EALog.cpp)


# Header files.
# GStreamer uses pkg-config.
find_package(PkgConfig)
pkg_check_modules(
    GST
    REQUIRED
    gstreamer-1.0>=1.12)

INCLUDE_DIRECTORIES(
    ${CMAKE_BINARY_DIR}/include
    ${PROJECT_SOURCE_DIR}/include
    ${GST_INCLUDE_DIRS})


# Compile options.
ADD_COMPILE_OPTIONS(-Wall ${GST_CFLAGS})


# Definitions
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)


# Libraries
LINK_LIBRARIES(
    stdc++
    pthread
    boost_log
    boost_thread
    boost_system
    boost_program_options
    ${GST_LIBRARIES})


#Object libary for unittests.
ADD_LIBRARY(src OBJECT ${SRC_FILES})


# Build target extcutable.
ADD_EXECUTABLE(${PROGRAM_EXE} ${EXE_MAIN})
TARGET_LINK_LIBRARIES(${PROGRAM_EXE} src)


# Installation.
INSTALL(TARGETS ${PROGRAM_EXE} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

