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


CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

# Project name and version
PROJECT("earlyapp")
SET(PROJECT_VERSION_MAJOR 0)
SET(PROJECT_VERSION_MINOR 9)
SET(PROJECT_VERSION_PATCH 5)

# Version file.
CONFIGURE_FILE("version.h.in" ${CMAKE_BINARY_DIR}/include/EAVersion.h)


# [Features]
#  - Log output
OPTION(USE_LOGOUTPUT "Control detailed log output to standard out" OFF)
IF(USE_LOGOUTPUT)
    ADD_DEFINITIONS(-DUSE_LOGOUTPUT)
ENDIF(USE_LOGOUTPUT)

#  - Log demsg output
OPTION(USE_DMESGLOG "Log output to dmesg log" OFF)
IF(USE_DMESGLOG)
    ADD_DEFINITIONS(-DUSE_DMESGLOG)
ENDIF(USE_DMESGLOG)


# Program executable.
SET(PROGRAM_EXE ${CMAKE_PROJECT_NAME})
SUBDIRS(src)

# Service configuration
SUBDIRS(config)

# Resources files
SUBDIRS(res)

