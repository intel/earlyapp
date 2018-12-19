/******************************************************************************\
Copyright (c) 2005-2018, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

This sample was distributed or derived from the Intel's Media Samples package.
The original version of this sample may be obtained from https://software.intel.com/en-us/intel-media-server-studio
or https://software.intel.com/en-us/media-client-solutions-support.
\**********************************************************************************/

#pragma once

#ifndef __PLUGIN_LOADER_H__
#define __PLUGIN_LOADER_H__

#include "vm/so_defs.h"
#include "sample_utils.h"
#include "plugin_utils.h"
//#include "mfx_plugin_module.h"
#include <iostream>
#include <iomanip> // for std::setfill, std::setw
#include <memory> // for std::unique_ptr

class MsdkSoModule
{
protected:
    msdk_so_handle m_module;
public:
    MsdkSoModule()
        : m_module(NULL)
    {
    }
    MsdkSoModule(const msdk_string & pluginName)
        : m_module(NULL)
    {
        m_module = msdk_so_load(pluginName.c_str());
        if (NULL == m_module)
        {
            MSDK_TRACE_ERROR(msdk_tstring(MSDK_CHAR("Failed to load shared module: ")) + pluginName);
        }
    }
    template <class T>
    T GetAddr(const std::string & fncName)
    {
        T pCreateFunc = reinterpret_cast<T>(msdk_so_get_addr(m_module, fncName.c_str()));
        if (NULL == pCreateFunc) {
            MSDK_TRACE_ERROR(msdk_tstring("Failed to get function addres: ") + fncName.c_str());
        }
        return pCreateFunc;
    }

    virtual ~MsdkSoModule()
    {
        if (m_module)
        {
            msdk_so_free(m_module);
            m_module = NULL;
        }
    }
};

#endif // PLUGIN_LOADER
