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

# Source files.
SET(SRC_FILES
    pipeline_decode.cpp
    mfx_buffering.cpp
    thread_linux.cpp
    thread.cpp
    sample_utils.cpp
    atomic_linux.cpp
    time_linux.cpp
    general_allocator.cpp
    avc_spl.cpp
    avc_nal_spl.cpp
    avc_bitstream.cpp
    base_allocator.cpp
    sysmem_allocator.cpp
    vaapi_device.cpp
    vaapi_utils.cpp
    vaapi_allocator.cpp
    vaapi_utils_drm.cpp
    class_wayland.cpp
    listener_wayland.cpp
    wayland-drm-protocol.c)


FIND_PACKAGE(PkgConfig REQUIRED)

# Media SDK
PKG_CHECK_MODULES(MSDK REQUIRED mfx)

# LibDRM
PKG_CHECK_MODULES(LIBDRM REQUIRED libdrm)


INCLUDE_DIRECTORIES(
    ${CMAKE_BINARY_DIR}/include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/ext/MediaSDK/include
    ${Boost_INCLUDE_DIRS}
    ${GST_INCLUDE_DIRS}
    ${MSDK_INCLUDE_DIRS}
    ${LIBDRM_INCLUDE_DIRS})

# Compile options.
ADD_COMPILE_OPTIONS(
    -Wall
    -Wformat -Wformat-security
    -O2 -D_FORTIFY_SOURCE=2
    -fPIE -fPIC
    -fstack-protector-strong
    ${GST_CFLAGS}
    ${MSDK_CFLAGS}
    ${LIBDRM_CFLAGS})

# Add linker flags.
SET(CMAKE_EXE_LINKER_FLAGS "-pie -z noexecstack -z relro -z now")


# Libraries
LINK_LIBRARIES(
     stdc++
     pthread
     ${MSDK_LIBRARIES}
     ${LIBDRM_LIBRARIES})


#Object libary for unittests.
ADD_LIBRARY(msdk OBJECT ${SRC_FILES})
