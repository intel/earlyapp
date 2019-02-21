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


# pkg-config
FIND_PACKAGE(PkgConfig REQUIRED)


# Check Boost libraries
FIND_PACKAGE(Boost REQUIRED
    COMPONENTS
    log
    thread
    system
    program_options)


# Source files.
SET(EXE_MAIN main.cpp)
SET(SRC_FILES
    CBCEvent.cpp
    CBCEventDevice.cpp
    CBCEventListener.cpp
    CBCEventReceiver.cpp
    Configuration.cpp
    DeviceController.cpp
    GPIOControl.cpp
    OutputDevice.cpp
    SystemStatusTracker.cpp
    VirtualCBCEventDevice.cpp
    EALog.cpp)


# GStreamer dependencies.
SET(GSTDEV_SRCFILES
    GStreamerApp.cpp
    GstAudioDevice.cpp
    GstCameraDevice.cpp
    GstVideoDevice.cpp)


# Non-GStreamer dependencies.
SET(DEV_SRCFILES
    AudioDevice.cpp
    CameraDevice.cpp
    VideoDevice.cpp)


# wayland-client
PKG_CHECK_MODULES(WAYLAND REQUIRED wayland-client)

# GStreamer package config.
PKG_CHECK_MODULES(GST REQUIRED gstreamer-1.0)

# Medai SDK, ALSA, EGL, LIBVA
PKG_CHECK_MODULES(MSDK REQUIRED libmfxhw64)
PKG_CHECK_MODULES(ALSA REQUIRED alsa)
PKG_CHECK_MODULES(EGL REQUIRED egl)
PKG_CHECK_MODULES(LIBVA REQUIRED libva)


# Include dirs
INCLUDE_DIRECTORIES(
    ${CMAKE_BINARY_DIR}/include
    ${PROJECT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${WAYLAND_INCLUDE_DIRS}
    ${GST_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/ext/MediaSDK/include
    ${PROJECT_SOURCE_DIR}/ext/CameraICI/include
    ${PROJECT_SOURCE_DIR}/ext/GLES2/include
    ${MSDK_INCLUDE_DIRS}
    ${ALSA_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS})


# Link libraries
LINK_LIBRARIES(
    stdc++
    pthread
    ${Boost_LIBRARIES}
    ${WAYLAND_LIBRARIES}
    ${GST_LIBRARIES}
    msdk
    iciCam
    gles2
    ${LINK_LIBRARIES}
    ${MSDK_LIBRARIES}
    ${ALSA_LIBRARIES}
    ${EGL_LIBRARIES})


# Compile options.
ADD_COMPILE_OPTIONS(
    -Wall
    -Wformat -Wformat-security
    -O2 -D_FORTIFY_SOURCE=2
    -fPIE -fPIC
    -fstack-protector-strong
    ${GST_CFLAGS})


# Add linker flags.
SET(CMAKE_EXE_LINKER_FLAGS "-pie -z noexecstack -z relro -z now")

# Object libary.
ADD_LIBRARY(src OBJECT ${SRC_FILES} ${DEV_SRCFILES} ${GSTDEV_SRCFILES})

# Build target extcutable.
ADD_EXECUTABLE(${PROGRAM_EXE} ${EXE_MAIN})
TARGET_LINK_LIBRARIES(${PROGRAM_EXE} src)

# Installation.
INSTALL(TARGETS ${PROGRAM_EXE} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
