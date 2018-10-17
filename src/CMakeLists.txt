#
# Copyright Intel Corporation All Rights Reserved.
#
# The source code, information and material ("Material") contained herein is
# owned by Intel Corporation or its suppliers or licensors, and title to such
# Material remains with Intel Corporation or its suppliers or licensors. The
# Material contains proprietary information of Intel or its suppliers and
# licensors. The Material is protected by worldwide copyright laws and treaty
# provisions. No part of the Material may be used, copied, reproduced,
# modified, published, uploaded, posted, transmitted, distributed or disclosed
# in any way without Intel's prior express written permission. No license under
# any patent, copyright or other intellectual property rights in the Material
# is granted to or conferred upon you, either expressly, by implication,
# inducement, estoppel or otherwise. Any license under such intellectual
# property rights must be express and approved by Intel in writing.
#
# Unless otherwise agreed by Intel in writing, you may not remove or alter this
# notice or any other notice embedded in Materials by Intel or Intel's
# suppliers or licensors in any way."
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
    OutputDevice.cpp
    SystemStatusTracker.cpp
    VideoDevice.cpp
    VirtualCBCEventDevice.cpp)


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
ADD_COMPILE_OPTIONS(${GST_CFLAGS})


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
ADD_EXECUTABLE(${PROGRAM_EXE} ${SRC_FILES} ${EXE_MAIN})


# Installation.
INSTALL(TARGETS ${PROGRAM_EXE} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

