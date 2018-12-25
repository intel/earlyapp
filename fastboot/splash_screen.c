/*
 * Copyright (C) 2018 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 *
 * Authors: Bin Yang <bin.yang@intel.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/sysmacros.h>

void *splash_screen_init(void *arg)
{
	int fd = -1;
	int fd_fb0 = -1;
	struct stat sb;
	char *buf = NULL;
	size_t size;
	dev_t dev;

	dev = makedev(29, 0);
	mknod(WORKDIR"/fb0_dev", 0600 | S_IFCHR, dev);

#ifdef SPLASH_SCREEN_START_CMD
	if (system(SPLASH_SCREEN_START_CMD " > /dev/null"))
		fprintf(stderr, "\"%s\" return error\n", SPLASH_SCREEN_START_CMD);
#endif
	fd_fb0 = open(WORKDIR"/fb0_dev", O_WRONLY);
	if (fd_fb0 < 0) {
		fprintf(stderr, "open fb0 dev error (%d): %m\n", errno);
		goto exit;
	}

	fd = open(SPLASH_SCREEN_FB_FILE, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open %s error, errno = %d\n", SPLASH_SCREEN_FB_FILE, errno);
		goto exit;
	}

	fstat(fd, &sb);
	size = sb.st_size;
	buf = malloc(size);

	if (read(fd, buf, size) != size) {
		fprintf(stderr, "read %s error, errno = %d\n", SPLASH_SCREEN_FB_FILE, errno);
		goto exit;
	}
	if (write(fd_fb0, buf, size) != size) {
		fprintf(stderr, "write %s error, errno = %d\n", "/dev/fb0", errno);
		goto exit;
	}

exit:
#ifdef SPLASH_SCREEN_END_CMD
	if (system(SPLASH_SCREEN_END_CMD " > /dev/null"))
		fprintf(stderr, "\"%s\" return error\n", SPLASH_SCREEN_END_CMD);
#endif
	if (fd > 0)
		close(fd);
	if (fd_fb0 > 0)
		close(fd_fb0);
	if (buf)
		free(buf);
	return NULL;
}
