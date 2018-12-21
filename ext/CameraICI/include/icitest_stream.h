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

#ifndef ICITEST_STREAM_H
#define ICITEST_STREAM_H

#define BUFFER_COUNT 4
#define DEFAULT_STREAM_ID 9 

#include "ici.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "icitest_graph.h"

struct ici_stream_format stream_fmt;
unsigned long buffer_size;

int open_device(const char* dev_name);
void close_device(int dev_fd);
int init_stream(int dev_fd);
int allocate_buffers(struct buffer* buffers, unsigned long buffer_size,
		int buffer_count, int mem_type, struct display* display);
void free_buffers(struct buffer* buffers, int n_buffers, int mem_type);

int queue_buffer(int dev_fd, struct buffer* buffers, int mem_type);
int queue_buffers(int dev_fd, int buffer_count,
		struct buffer* buffers, int mem_type);
int dequeue_buffer(int dev_fd, int mem_type, int *is_top);
int stream_on(int dev_fd);
void cleanup(int fd);

#endif /*ICITEST_STREAM_H*/
