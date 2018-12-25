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

SET(FASTBOOT_EXE ${CMAKE_PROJECT_NAME}-fastboot)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_SEARCH_MODULE(LIBDRM REQUIRED libdrm)

SET(SRC_FILES main.c
	splash_screen_drm.c
	)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR} ${LIBDRM_INCLUDE_DIRS})

LINK_LIBRARIES(
    pthread
    ${LIBDRM_LIBRARIES}
    ${LINK_LIBRARIES})

ADD_COMPILE_OPTIONS(
    -Wall
    -Wformat -Wformat-security
    -O2 -D_FORTIFY_SOURCE=2
    -fPIE -fPIC
    -fstack-protector-strong)

SET(CMAKE_EXE_LINKER_FLAGS "-pie -z noexecstack -z relro -z now")

ADD_DEFINITIONS(-DWORKDIR="/var/lib/${PROJECT_NAME}")
ADD_DEFINITIONS(-DSPLASH_SCREEN_FB_FILE="${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/clear_fb.fb")
ADD_DEFINITIONS(-DSPLASH_SCREEN_START_CMD="${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/kpi_gpio.sh 442 1")
ADD_DEFINITIONS(-DSPLASH_SCREEN_END_CMD="${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/kpi_gpio.sh 442 0")

ADD_EXECUTABLE(${FASTBOOT_EXE} ${SRC_FILES})

INSTALL(TARGETS ${FASTBOOT_EXE} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
