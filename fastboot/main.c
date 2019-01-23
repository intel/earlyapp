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
#define _GNU_SOURCE
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

#define ARRAY_SIZE(array)       (sizeof(array) / sizeof((array)[0]))
static pthread_t load_ipu4_modules_tid;
char *ipu4_modulesp[]  = { 
	"crlmodule-lite",
	"intel-ipu4",
	"intel-ipu4-mmu",
	"intel-ipu4-psys",
	"ici-isys-mod",
	"intel-ipu4-psys-csslib",
	"intel-ipu4-isys-csslib",
};

void *load_ipu4_modules(void *arg)
{
	int len = ARRAY_SIZE(ipu4_modulesp);
	char mprobe[64];
	int ret;

	for( int i= 0; i < len; i++) {
		//fprintf(stderr, "modprobe %s", ipu4_modulesp[i]);
		sprintf(mprobe, "modprobe %s", ipu4_modulesp[i]);
		ret = system(mprobe);
		if (ret < 0)
			fprintf(stderr, "faile to modprobe %s", ipu4_modulesp[i]);

	}
}
#ifdef PRELOAD_LIST_FILE
static pthread_t preload_tid;
static void *preload_thread(void *arg)
{
	int fd;
	struct stat sb;
	int ret;
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	fp = fopen(PRELOAD_LIST_FILE, "r");
	if (!fp) {
//		fprintf(stderr, "open preload list error (%d): %m\n", errno);
		goto exit;
	}

	while ((nread = getline(&line, &len, fp)) != -1) {
		if (nread < 2) {
			continue;
		}
		if (line[nread - 1] == '\n' || line[nread - 1] == '\r')
			line[nread - 1] = 0;
		fd = open(line, O_RDONLY);
		if (fd > 0) {
			fstat(fd, &sb);
			ret = readahead(fd, 0, sb.st_size);
			if (ret == 0) {
				//fprintf(stdout, "preload %s success\n", line);
			}else {
				fprintf(stdout, "preload %s error (%d): %m\n", line, errno);
			}
			close(fd);
		} else {
			//fprintf(stdout, "preload open %s error (%d): %m\n", line, errno);
		}
	}

exit:
	free(line);
	if (fp)
		fclose(fp);
	return NULL;
}
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

        pthread_create(&load_ipu4_modules_tid, NULL, load_ipu4_modules, NULL);
        pthread_join(load_ipu4_modules_tid, NULL);

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

#ifdef PRELOAD_LIST_FILE
	pthread_create(&preload_tid, NULL, preload_thread, NULL);
#endif

#ifdef SPLASH_SCREEN_FB_FILE
	pthread_join(splash_screen_tid, NULL);
#endif

#ifdef PRELOAD_LIST_FILE
	pthread_join(preload_tid, NULL);
#endif

	return 0;
}
