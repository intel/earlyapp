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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <libdrm/intel_bufmgr.h>
#include "ici.h"

#include "icitest_stream.h"
#include "icitest_graph.h"
#include "icitest_common.h"

static void err_msg(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));

}

static int xioctl(int fh, int request, void *arg)
{
	int r;

	do {
		r = ioctl(fh, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}

void close_device(int fd)
{
	if (-1 == close(fd))
		err_msg("close_device:");
}

int open_device(const char* dev_name)
{
	struct stat st;
	int fd;
	printf("Opening device \"%s\"...\n", dev_name);
	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n",
				dev_name, errno, strerror(errno));
		return -1;
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		return -1;
	}

	fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
				dev_name, errno, strerror(errno));
	}
	return fd;
}

static int set_format(int fd, struct ici_stream_format* fmt)
{
		/* Set format on Stream device*/
	if (xioctl(fd, ICI_IOC_SET_FORMAT, fmt) == -1) {
		if (EINVAL == errno) {
			fprintf(stderr, "Unable to set streamformat\n");
			return -1;
		}
	}
	return 0;
}

int init_stream(int dev_fd)
{
	return set_format(dev_fd, &stream_fmt);
}

int allocate_buffers(struct buffer* buffers, unsigned long buffer_size,
		int buffer_count, int mem_type, struct display* display)
{
	int rval;	
	int i;

	for (i = 0; i < buffer_count; ++i) {
		buffers[i].length = buffer_size;
		buffers[i].index = i;
		if (mem_type == ICI_MEM_USERPTR) {
			rval = posix_memalign(&buffers[i].start,
					getpagesize(), buffer_size);
			if (rval != 0) {
				fprintf(stderr, "Out of memory\n");
				return -1;
			}
		}
		else if (mem_type == ICI_MEM_DMABUF) {
			rval = create_buffer(display,&buffers[i],buffer_size);
			if(rval < 0) {
				printf("FAIL0!!");
				destroy_gem(display);
				return rval;
			}
		}
	}
	printf("allocate_user_buffers exit \n");
	return 0;
}

void free_buffers(struct buffer* buffers, int n_buffers, int mem_type)
{
	int i;
	for (i = 0; i < n_buffers; i++) {
		if(mem_type == ICI_MEM_DMABUF) {
			drm_intel_bo_unmap(buffers[i].bo);
			close(buffers[i].dbuf_fd);
	 		printf("DMA buffer freed: %d\n", buffers[i].dbuf_fd);
		} else
			free(buffers[i].start);
	}
}


int queue_buffer(int dev_fd, struct buffer* buffer, int mem_type)
{
	struct ici_frame_info capture_buf = {0};
	capture_buf.mem_type = mem_type;
	capture_buf.num_planes = 1;
	if (mem_type == ICI_MEM_USERPTR)
		capture_buf.frame_planes[0].mem.userptr = (unsigned long)buffer->start;
	else {
		capture_buf.frame_planes[0].mem.dmafd = (unsigned long)buffer->dbuf_fd;
	}
	capture_buf.frame_planes[0].length = buffer->length;
	capture_buf.frame_buf_id = buffer->index;

	if (-1 == xioctl(dev_fd, ICI_IOC_GET_BUF, &capture_buf)) {
		err_msg("ICI_IOC_GET_BUF");
		return -1;
	}
	return 0;
}

int queue_buffers(int dev_fd, int buffer_count,
		struct buffer* buffers, int mem_type)
{
	int i;
	for (i = 0; i < buffer_count; ++i) {
		if( queue_buffer(dev_fd, &buffers[i], mem_type) < 0)
			return -1;
	}

	return 0;
}

int dequeue_buffer(int dev_fd, int mem_type, int *is_top)
{
	struct ici_frame_info rcv_buf = {0};
	rcv_buf.mem_type = mem_type;
	if (-1 == xioctl(dev_fd, ICI_IOC_PUT_BUF, &rcv_buf)) {
		err_msg("ICI_IOC_PUT_BUF");
		return -1;
	}

	if (rcv_buf.field == ICI_FIELD_BOTTOM)
		*is_top = 0;
	else
		*is_top = 1;

	return rcv_buf.frame_buf_id;
}

int stream_on(int dev_fd)
{
	printf("Setting Stream on \n");
		int ret = 0;
	if(-1 == (ret = ioctl(dev_fd, ICI_IOC_STREAM_ON)))
		err_msg("ICI_IOC_STREAM_ON failed \n");
	return ret;
}

void cleanup(int fd) {
	printf("Setting Stream off \n");
	if(-1 == ioctl(fd, ICI_IOC_STREAM_OFF))
		err_msg("ICI_IOC_STREAM_OFF failed \n");
}
