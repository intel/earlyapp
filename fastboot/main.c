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
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/sysmacros.h>
#include <pthread.h>

#define DEFAULT_INIT "/sbin/init"

#ifdef SPLASH_SCREEN_FB_FILE
static pthread_t splash_screen_tid;
void *splash_screen_init(void *arg);
#endif

int main(int argc, char *argv[])
{
	int fd;
	int ret;
	char buf[8];

	if (getpid() == 1) {
		if (fork())
			execl(DEFAULT_INIT, DEFAULT_INIT, NULL);
	}

	/* for kpi test */
	if (access("/sys/class/gpio/export", R_OK) != 0) {
		mount("/sys", "/sys", "sysfs", 0, NULL);
	}

	ret = system("mkdir -p " WORKDIR);
	if (ret < 0) {
		fprintf(stderr, "create dir %s error (%d): %m\n", WORKDIR, errno);
		return -1;
	}
	fd = open(WORKDIR "/.fastboot.pid", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0) {
		fprintf(stderr, "open %s error (%d): %m\n", WORKDIR "/.fastboot.pid", errno);
		return -1;
	}
	ret = snprintf(buf, sizeof(buf) - 1, "%d", getpid());
	ret += 1;
	buf[ret] = 0;
	if (write(fd, buf, ret) != ret) {
		fprintf(stderr, "save pid error (%d): %m\n", errno);
		return -1;
	}
	close(fd);

#ifdef SPLASH_SCREEN_FB_FILE
	pthread_create(&splash_screen_tid, NULL, splash_screen_init, NULL);
#endif
	//TODO read earlyapp, lib and res files into page cache in another thread

#ifdef SPLASH_SCREEN_FB_FILE
	pthread_join(splash_screen_tid, NULL);
#endif
	return 0;
}
