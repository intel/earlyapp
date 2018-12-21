////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2018 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
// SPDX-License-Identifier: MIT
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ICITEST_COMMON_H
#define ICITEST_COMMON_H

#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>

#define BATCH_SIZE 0x80000

enum input {
	HDMI_INPUT = 1,
	CVBS_INPUT,
	TPG_INPUT,
    OVTI_INPUT
};

struct setup {
	char stream[32];
	unsigned int iw, ih;
	unsigned int ow, oh;
	unsigned int isys_w, isys_h;
	unsigned int stride_width;
	unsigned int use_wh : 1;
	unsigned int in_fourcc;
	unsigned int buffer_count;
	unsigned int port;
	unsigned int fullscreen;
	unsigned int interlaced;
	unsigned int frames_count;
	enum input stream_input;
	int mem_type;
};

#if 1
extern int first_frame_received;
extern int first_frame_rendered;

extern int running;

extern struct timeval *curr_time, *prev_time;
#endif

#define _ISP_MODE_PREVIEW       0x8000
#define _ISP_MODE_STILL         0x2000
#define _ISP_MODE_VIDEO         0x4000

#define GET_TS(t) clock_gettime(CLOCK_MONOTONIC, &t)

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define ERRSTR strerror(errno)

#define BYE_ON(cond, ...) \
	do { \
		if (cond) { \
			int errsv = errno; \
			fprintf(stderr, "ERROR(%s:%d) : ", \
					__FILE__, __LINE__); \
			errno = errsv; \
			fprintf(stderr,  __VA_ARGS__); \
			abort(); \
		} \
	} while(0)

static inline int warn(const char *file, int line, const char *fmt, ...)
{
	int errsv = errno;
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "WARN(%s:%d): ", file, line);
	vfprintf(stderr, fmt, va);
	va_end(va);
	errno = errsv;
	return 1;
}

#define WARN_ON(cond, ...) \
	((cond) ? warn(__FILE__, __LINE__, __VA_ARGS__) : 0)


#endif /*ICITEST_COMMON_H*/

