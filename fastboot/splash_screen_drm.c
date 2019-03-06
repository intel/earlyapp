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
#include <sys/mman.h>
#include <sys/sysmacros.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#define FB_DEPTH 24
#define FB_BPP 32

struct  drm_fb_t {
	int drm_fd;
	drmModeModeInfo mode;
	uint32_t conn_id;
	uint32_t crtc_id;
	uint32_t fb_id;
	uint32_t handle;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint32_t size;
	uint8_t *map;
};

static struct drm_fb_t drm_fb0;

static void drm_destroy_fb(struct drm_fb_t *fb)
{
	struct drm_mode_destroy_dumb dreq;

	if (fb->map && fb->map != MAP_FAILED)
		munmap(fb->map, fb->size);
	drmModeRmFB(fb->drm_fd, fb->fb_id);
	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = fb->handle;
	drmIoctl(fb->drm_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
}

static int drm_get_fb0(int fd, struct drm_fb_t *fb)
{
	drmModeRes *res = NULL;
	drmModeConnector *conn = NULL;
	drmModeEncoder *enc = NULL;
	struct drm_mode_create_dumb creq;
	struct drm_mode_map_dumb mreq;
	uint64_t has_dumb;
	int i, j;
	int ret = 0;

	if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || has_dumb == 0) {
		fprintf(stderr, "DRM_CAP_DUMB_BUFFER not support\n");
		return -1;
	}

	res = drmModeGetResources(fd);
	if (!res) {
		fprintf(stderr, "Cannot get DRM resources\n");
		return -1;
	}

	for (i = 0; i < res->count_connectors; ++i) {
		conn = drmModeGetConnector(fd, res->connectors[i]);
		if (conn && conn->connection == DRM_MODE_CONNECTED && conn->count_modes > 0)
			break;
		if (conn)
			drmModeFreeConnector(conn);
	}
	if (!res) {
		fprintf(stderr, "Cannot get valid connector\n");
		ret = -1;
		goto exit;
	}

	fb->drm_fd = fd;
	fb->conn_id = conn->connector_id;
	fb->width = conn->modes[0].hdisplay;
	fb->height = conn->modes[0].vdisplay;
	//fprintf(stdout, "Get connector %d, %ux%u\n", conn->connector_id, fb->width, fb->height);

	enc = drmModeGetEncoder(fd, conn->encoder_id);
	if (!enc) {
		for (i = 0; i < conn->count_encoders; ++i) {
			enc = drmModeGetEncoder(fd, conn->encoders[i]);
			if (!enc)
				continue;
			for (j = 0; j < res->count_crtcs; ++j) {
				if (enc->possible_crtcs & (1 << j))
					break;
			}
			if (j >= res->count_crtcs) {
				drmModeFreeEncoder(enc);
				continue;
			}
			fb->crtc_id = res->crtcs[j];
			break;
		}
	} else {
		fb->crtc_id = enc->crtc_id;
	}

	if (!enc) {
		fprintf(stderr, "Cannot get valid encoder\n");
		ret = -1;
		goto exit;
	}

	memset(&creq, 0, sizeof(creq));
	creq.width = fb->width;
	creq.height = fb->height;
	creq.bpp = FB_BPP;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
	if (ret < 0) {
		fprintf(stderr, "Cannot create dumb\n");
		goto exit;
	}
	fb->stride = creq.pitch;
	fb->size = creq.size;
	fb->handle = creq.handle;

	ret = drmModeAddFB(fd, fb->width, fb->height, 24, 32, fb->stride,
			   fb->handle, &fb->fb_id);
	if (ret) {
		fprintf(stderr, "Cannot create framebuf\n");
		goto err_destroy;
	}

	memset(&mreq, 0, sizeof(mreq));
	mreq.handle = fb->handle;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	if (ret) {
		fprintf(stderr, "Cannot map dump\n");
		goto err_destroy;
	}

	fb->map = mmap(0, fb->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset);
	if (fb->map == MAP_FAILED) {
		fprintf(stderr, "framebuf mmap error\n");
		goto err_destroy;
	}

	ret = drmModeSetCrtc(fd, fb->crtc_id, fb->fb_id, 0, 0, &fb->conn_id, 1, &conn->modes[0]);
	if (ret) {
		fprintf(stderr, "set crtc error (%d): %m\n", errno);
		goto err_destroy;
	}

	goto exit;

err_destroy:
	drm_destroy_fb(fb);
exit:
	if (enc)
		drmModeFreeEncoder(enc);
	if (conn)
		drmModeFreeConnector(conn);
	if (res)
		drmModeFreeResources(res);
	return ret;
}


void *splash_screen_init(void *arg)
{
	dev_t dev;
	int drm_fd = -1;
	int img_fd = -1;
	struct stat sb;

#ifdef SPLASH_SCREEN_START_CMD
	if (system(SPLASH_SCREEN_START_CMD " > /dev/null"))
		fprintf(stderr, "\"%s\" return error\n", SPLASH_SCREEN_START_CMD);
#endif
	img_fd = open(SPLASH_SCREEN_FB_FILE, O_RDONLY);
	if (img_fd < 0) {
		fprintf(stderr, "open %s error, errno = %d\n", SPLASH_SCREEN_FB_FILE, errno);
		goto exit;
	}

	dev = makedev(226, 0);
	mknod(WORKDIR"/drm0_dev", 0600 | S_IFCHR, dev);
	drm_fd = open(WORKDIR"/drm0_dev", O_RDWR | O_CLOEXEC);
	if (drm_fd < 0) {
		fprintf(stderr, "open drm device error (%d): %m\n", errno);
		goto exit;
	}

	if (drm_get_fb0(drm_fd, &drm_fb0) != 0) {
		fprintf(stderr, "get fb0 error\n");
		goto exit;
	}

	fstat(img_fd, &sb);
	if (drm_fb0.size != sb.st_size)
		fprintf(stderr, "fb0 size and splash img size mismatch, fb0 size: %u, img size: %lu\n", drm_fb0.size, sb.st_size);
	if (read(img_fd, drm_fb0.map, drm_fb0.size) <= 0) {
		fprintf(stderr, "read %s error, errno = %d\n", SPLASH_SCREEN_FB_FILE, errno);
		goto exit;
	}

exit:
#ifdef SPLASH_SCREEN_END_CMD
	if (system(SPLASH_SCREEN_END_CMD " > /dev/null"))
		fprintf(stderr, "\"%s\" return error\n", SPLASH_SCREEN_END_CMD);
#endif
	if (img_fd > 0)
		close(img_fd);
	if (drm_fd > 0) {
		drm_destroy_fb(&drm_fb0);
		close(drm_fd);
	}
	return NULL;
}
