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

#ifndef ICITEST_GRAPH_H
#define ICITEST_GRAPH_H

#include <string.h>
#include <intel_bufmgr.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <wayland-egl.h>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <drm/drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

//#include "icitest_common.h"
#include "wayland-drm-client-protocol.h"

#define TARGET_NUM_SECONDS 5

PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
PFNGLPROGRAMBINARYOESPROC glProgramBinaryOES;
PFNGLGETPROGRAMBINARYOESPROC glGetProgramBinaryOES;

struct buffer {
	drm_intel_bo *bo;
	unsigned int index;
	unsigned int fb_handle;
	int dbuf_fd;
	uint32_t flink_name;
	struct wl_buffer *buf;
	EGLImageKHR khrImage;
	void *start;
	size_t length;
	int is_top;
};

struct output {
	struct display *display;
	struct wl_output *output;
	struct wl_list link;
};

struct display {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shell *wl_shell;
	struct wl_drm *wl_drm;
	struct window *window;
	struct wl_list output_list;
	int	strm_fd;
	int drm_fd;
	dri_bufmgr *bufmgr;
	struct buffer *buffers;
	struct buffer *disp_bufs[2];
	struct setup *s;
	struct {
		EGLDisplay dpy;
		EGLContext ctx;
		EGLConfig conf;
	} egl;
};

struct geometry {
	int width, height;
};

struct window {
	struct display *display;
	struct geometry geometry, window_size;
	struct wl_surface *surface;
	void *shell_surface;
	struct wl_egl_window *native;
	EGLSurface egl_surface;
	struct wl_callback *callback;
	int fullscreen, opaque, configured, output;
	int print_fps, frame_count;
	struct {
		GLuint fbo;
		GLuint color_rbo;

		GLuint modelview_uniform;
		GLuint gl_texture_size;
		GLuint gl_tex_name[2];

		GLuint gl_tex_sampler[2];
		GLuint rgb565;
		GLuint swap_rb;

		GLuint pos;
		GLuint col;
		GLuint attr_tex;


		GLuint program;

		GLfloat hmi_vtx[12u]; //!< hold coordinates of vertices for texture
		GLfloat hmi_tex[8u];  //!< hold indices of vertices for texture
		GLubyte hmi_ind[6u];  //!< hold coordinates for texture sample

		GLfloat model_view[16u];
	} gl;
};

struct wl_shell_surface_listener wl_shell_surface_listener;
const struct wl_callback_listener frame_listener;
struct wl_callback_listener configure_callback_listener;
const struct wl_callback_listener frame_listener;
const struct wl_registry_listener registry_listener;

void destroy_surface(struct window *window);
void init_gl(struct window *window);
void init_egl(struct display *display, int opaque);
int init_gem(struct display *display);
int drm_buffer_to_prime(struct display *display, struct buffer *buffer,
		unsigned int size);
void create_surface(struct window *window);
void destroy_gem(struct display *display);
int create_buffer(struct display *display, struct buffer *buffer,
		unsigned int size);


#endif /*ICITEST_GRAPH_H*/

