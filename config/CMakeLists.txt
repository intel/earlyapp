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


# Resource files.
SET(CONF_FILES
    earlyapp.target
    earlyapp_gst.target
    earlyapp_gpnative.target
    earlyapp.slice
    ias-earlyapp.service
    ias-earlyapp-setup.service
    earlyapp_resume.service
    earlyapp_suspend.service
    earlyapp-setup-cbc.service
    earlyapp-setup-render.service
    earlyapp-setup-aud.service
    earlyapp-setup_gst.service
    earlyapp-setup_gpnative.service
    earlyapp-setup-ipu.service
    simple-egl.service
    simple-egl_resume.service
    early-audio.service
    early-audio_resume.service
    mem_hot_add.service)

# Early App Service file.
# GStreamer base devices require different settings.
SET(EAS_TARGET earlyapp.service)
SET(EAS_SRC
    earlyapp_gpnative.service
    earlyapp_gst.service
    earlyapp.service)

# Script files.
SET(SCRIPT_FILES
    early_audio.sh
    kpi_gpio.sh
    mem_hot_add.sh)

# Install earlyapp.service
INSTALL(FILES ${EAS_SRC}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system/)

# Install other configuration files.
INSTALL(FILES ${CONF_FILES}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system/)

# Install scripts.
INSTALL(PROGRAMS ${SCRIPT_FILES}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/)

# Install udev rules.
INSTALL(FILES 50-earlyapp.rules
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/udev/rules.d/)
