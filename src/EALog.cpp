////////////////////////////////////////////////////////////////////////////////
//
// Copyright Intel Corporation All Rights Reserved.
//
// The source code, information and material ("Material") contained herein is
// owned by Intel Corporation or its suppliers or licensors, and title to such
// Material remains with Intel Corporation or its suppliers or licensors. The
// Material contains proprietary information of Intel or its suppliers and
// licensors. The Material is protected by worldwide copyright laws and treaty
// provisions. No part of the Material may be used, copied, reproduced,
// modified, published, uploaded, posted, transmitted, distributed or disclosed
// in any way without Intel's prior express written permission. No license under
// any patent, copyright or other intellectual property rights in the Material
// is granted to or conferred upon you, either expressly, by implication,
// inducement, estoppel or otherwise. Any license under such intellectual
// property rights must be express and approved by Intel in writing.
//
// Unless otherwise agreed by Intel in writing, you may not remove or alter this
// notice or any other notice embedded in Materials by Intel or Intel's
// suppliers or licensors in any way."
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "EALog.h"


#ifdef USE_DMESGLOG
int dmesg_fd = 0;

int dmesgLogInit(void)
{
    dmesg_fd = open("/dev/kmsg", O_RDWR);
    if (dmesg_fd < 0)
    {
        return(-1);
    }
    return 0;
}

/*
 Enabel logging to dmesg
 Example: 
 dmesgLogPrint(("testing-gst-init"); 
 */
int dmesgLogPrint(const char* stringPtr)
{
    int ret = -1;

    if (dmesg_fd > 0)
    {
        ret=write(dmesg_fd, stringPtr, strlen(stringPtr)+1);	    
    }

    return(ret);
}



int dmesgLogClose(void)
{
    int ret = -1;

    if (dmesg_fd > 0)
    {
        ret=close(dmesg_fd);
    }

    return(ret);
}
#endif

