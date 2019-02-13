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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <asm/types.h>
#include <unistd.h>

#include "ici.h"
#include "icitest_common.h"
#include "icitest_time.h"
#include "icitest_graph.h"

extern void GPIOControl_outputPattern(void*);
void * g_GpioClass = NULL;
static int g_triggerOnce = 1;

PFNGLPROGRAMBINARYOESPROC glProgramBinaryOES = NULL;
PFNGLGETPROGRAMBINARYOESPROC glGetProgramBinaryOES = NULL;

/* UYVY Interlace*/
static const char *frag_shader_text_UYVY_interlaced  =
  "uniform sampler2D u_texture;"\
  "uniform sampler2D u_texture_bottom;"\
  "uniform bool swap_rb;"\
  "varying mediump vec2 texcoord;"\
  "varying mediump vec2 texsize;"\
  "void main(void) {"\
  "  mediump float y, u, v, tmp;"\
  "  mediump vec4 resultcolor;"\
  "  mediump vec4 raw ;"\
  "	if (fract(texcoord.y * texsize.y) >= 0.5)"\
  "  	raw = texture2D(u_texture, texcoord);"\
  "	else "\
  "		raw = texture2D(u_texture_bottom, texcoord);"\
  "  if (fract(texcoord.x * texsize.x) < 0.5)"\
  "    raw.a = raw.g;"\
  "  u = raw.b-0.5;"\
  "  v = raw.r-0.5;"\
  "  if (swap_rb) {"\
  "    tmp = u;"\
  "    u = v;"\
  "    v = tmp;"\
  "  }"\
  "  y = 1.1643*(raw.a-0.0625);"\
  "  resultcolor.r = (y+1.5958*(v));"\
  "  resultcolor.g = (y-0.39173*(u)-0.81290*(v));"\
  "  resultcolor.b = (y+2.017*(u));"\
  "  resultcolor.a = 1.0;"\
  "  gl_FragColor=resultcolor;"\
  "}";

  /* UYVY */
static const char *frag_shader_text_UYVY  =
  "uniform sampler2D u_texture;"\
  "uniform bool swap_rb;"\
  "varying mediump vec2 texcoord;"\
  "varying mediump vec2 texsize;"\
  "void main(void) {"\
  "  mediump float y, u, v, tmp;"\
  "  mediump vec4 resultcolor;"\
  "  mediump vec4 raw = texture2D(u_texture, texcoord);"\
  "  if (fract(texcoord.x * texsize.x) < 0.5)"\
  "    raw.a = raw.g;"\
  "  u = raw.b-0.5;"\
  "  v = raw.r-0.5;"\
  "  if (swap_rb) {"\
  "    tmp = u;"\
  "    u = v;"\
  "    v = tmp;"\
  "  }"\
  "  y = 1.1643*(raw.a-0.0625);"\
  "  resultcolor.r = (y+1.5958*(v));"\
  "  resultcolor.g = (y-0.39173*(u)-0.81290*(v));"\
  "  resultcolor.b = (y+2.017*(u));"\
  "  resultcolor.a = 1.0;"\
  "  gl_FragColor=resultcolor;"\
  "}";


/* SGRBG8 */
static const char *frag_shader_text_SGRBG8 =
  "uniform sampler2D u_texture;"\
  "varying mediump vec2 texcoord;"\
  "varying mediump vec2 texsize;"\
  "void main(void) {"\
  "  mediump vec2 texcoord2;"\
  "  mediump vec4 resultcolor;"\
  "  texcoord2.x = texcoord.x;"\
  "  texcoord2.y = texcoord.y + 0.5/texsize.y;"\
  "  mediump vec4 raw1 = texture2D(u_texture, texcoord);"\
  "  mediump vec4 raw2 = texture2D(u_texture, texcoord2);"\
  "  if (fract(gl_FragCoord.y/2.0) < 0.5) {"\
  "    resultcolor.g = raw1.r;"\
  "  } else {"\
  "    resultcolor.g = raw2.a;"\
  "  }"\
  "  resultcolor.r = raw1.a;"\
  "  resultcolor.b = raw2.r;"\
  "  resultcolor.a = 1.0;"\
  "  gl_FragColor=resultcolor;"\
  "}";

/* RGB565 and RGB888 */
static const char *frag_shader_text_RGB = 
  "uniform sampler2D u_texture;"\
  "uniform bool rgb565;"\
  "uniform bool swap_rb;"\
  "varying mediump vec2 texcoord;"\
  "varying mediump vec2 texsize;"\
  "void main(void) {"\
  "  lowp vec4 resultcolor;"\
  "  lowp vec4 raw = texture2D(u_texture, texcoord);"\
  "  if(rgb565) raw *= vec4(255.0/32.0, 255.0/64.0, 255.0/32.0, 1.0);"\
  "  if (swap_rb) resultcolor.rgb = raw.bgr;"\
  "  else resultcolor.rgb = raw.rgb;"\
  "  resultcolor.a = 1.0;"\
  "  gl_FragColor = resultcolor;"\
  "}";

/**
 * @brief vertex shader for displaying the texture
 */
static const char *vert_shader_text =
  "varying  mediump vec2 texcoord; "\
  "varying  mediump vec2 texsize; "\
  "attribute vec4 pos; "\
  "attribute mediump vec2 itexcoord; "\
  "uniform mat4 modelviewProjection; "\
  "uniform mediump vec2 u_texsize; "\
  "void main(void) "\
  "{ "\
  " texcoord = itexcoord; "\
  " texsize = u_texsize; "\
  " gl_Position = modelviewProjection * pos; "\
  "}";


void handle_ping(void *data, struct wl_shell_surface *shell_surface,
		uint32_t serial);
void handle_configure(void *data, struct wl_shell_surface *shell_surface,
		uint32_t edges, int32_t width, int32_t height);
void handle_popup_done(void *data, struct wl_shell_surface *shell_surface);
void configure_callback(void *data, struct wl_callback *callback,
		uint32_t  time);
void redraw(void *data, struct wl_callback *callback, uint32_t time);
void registry_handle_global(void *data, struct wl_registry *registry,
		uint32_t name, const char *interface, uint32_t version);


struct wl_shell_surface_listener wl_shell_surface_listener = {
	handle_ping,
	handle_configure,
	handle_popup_done
};

struct wl_callback_listener configure_callback_listener = {
	configure_callback,
};

const struct wl_callback_listener frame_listener = {
	redraw
};

const struct wl_registry_listener registry_listener = {
	registry_handle_global
};



struct output *
get_default_output(struct display *display)
{
	struct output *iter;
	int counter = 0;
	wl_list_for_each(iter, &display->output_list, link) {
		if(counter++ == display->window->output)
			return iter;
	}

	/* Unreachable, but avoids compiler warning */
	return NULL;
}

GLuint
create_shader(struct window *window, const char *source, GLenum shader_type)
{
	GLuint shader;
	GLint status;

	shader = glCreateShader(shader_type);
	assert(shader != 0);

	glShaderSource(shader, 1, (const char **) &source, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char log[1000];
		GLsizei len;
		glGetShaderInfoLog(shader, 1000, &len, log);
		fprintf(stderr, "Error: compiling %s: %*s\n",
				shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment",
				len, log);
		exit(1);
	}

	return shader;
}

void handle_ping(void *data, struct wl_shell_surface *shell_surface,
		uint32_t serial)
{
	wl_shell_surface_pong(shell_surface, serial);
}

void handle_configure(void *data, struct wl_shell_surface *shell_surface,
		uint32_t edges, int32_t width, int32_t height)
{
	struct window *window = data;

	window->geometry.width = width;
	window->geometry.height = height;

	if (!window->fullscreen)
		window->window_size = window->geometry;
}

void handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
}

void
configure_callback(void *data, struct wl_callback *callback, uint32_t  time)
{
	struct window *window = data;

	wl_callback_destroy(callback);

	window->configured = 1;

	if (window->callback == NULL)
		redraw(data, NULL, time);
}

void toggle_fullscreen(struct window *window, int fullscreen)
{
	struct wl_callback *callback;
	
	window->fullscreen = fullscreen;
	window->configured = 0;

	if (fullscreen) {
		wl_shell_surface_set_fullscreen(window->shell_surface,
				WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT,
				0, NULL);
	} else {
		wl_shell_surface_set_toplevel(window->shell_surface);
		handle_configure(window, window->shell_surface, 0,
				window->window_size.width,
				window->window_size.height);

	}

	callback = wl_display_sync(window->display->display);
	wl_callback_add_listener(callback, &configure_callback_listener,
			window);
}

void destroy_surface(struct window *window)
{
	/* Required, otherwise segfault in egl_dri2.c: dri2_make_current()
	 * on eglReleaseThread(). */
	eglMakeCurrent(window->display->egl.dpy, EGL_NO_SURFACE, EGL_NO_SURFACE,
		       EGL_NO_CONTEXT);

	eglDestroySurface(window->display->egl.dpy, window->egl_surface);
	wl_egl_window_destroy(window->native);
	wl_shell_surface_destroy(window->shell_surface);
	wl_surface_destroy(window->surface);

	if (window->callback)
		wl_callback_destroy(window->callback);
}

void update_fps(struct window *window)
{
	float time_diff_secs;
	struct timeval time_diff;
	struct timeval *tmp;

	if (window->print_fps) {
		window->frame_count++;

		gettimeofday(curr_time, NULL);

		timersub(curr_time, prev_time, &time_diff);
		time_diff_secs = (time_diff.tv_sec * 1000 + time_diff.tv_usec / 1000) / 1000;

		if (time_diff_secs >= TARGET_NUM_SECONDS) {
			fprintf(stdout, "Rendered %d frames in %6.3f seconds = %6.3f FPS\n",
				window->frame_count, time_diff_secs, window->frame_count / time_diff_secs);
			fflush(stdout);

			window->frame_count = 0;

		tmp = prev_time;
			prev_time = curr_time;
			curr_time = tmp;
		}
	}
}

void make_orth_matrix(GLfloat *data, GLfloat left, GLfloat right,
		GLfloat bottom, GLfloat top,
		GLfloat znear, GLfloat zfar)
{
	data[0] = 2.0/(right-left);
	data[5] = 2.0/(top-bottom);
	data[10] = -2.0/(zfar-znear);
	data[15] = 1.0;
	data[12] = (right+left)/(right-left);
	data[13] = (top+bottom)/(top-bottom);
	data[14] = (zfar+znear)/(zfar-znear);
}

void make_matrix(GLfloat *data, GLfloat v)
{
	make_orth_matrix(data, -v, v, -v, v, -v, v);
}

void redraw_egl_way(struct window *window, struct buffer *buf,
		unsigned char *start, unsigned char *buf2_start)
{
	int width, height;
	glViewport(0, 0, window->geometry.width, window->geometry.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0); // full transparency

	width = window->display->s->stride_width;
	height = window->display->s->ih;

	if (window->display->s->in_fourcc == ICI_FORMAT_UYVY) {
		width /= 2;
	} else if (window->display->s->in_fourcc == ICI_FORMAT_SGRBG8) {
		width /= 2;
		height /= 2;
	}

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, window->gl.gl_tex_name[0]);
	if (window->display->s->in_fourcc == ICI_FORMAT_SGRBG8) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
			GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, start);
	} else {
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
			GL_RGBA, GL_UNSIGNED_BYTE, start);
		if(window->display->s->in_fourcc == ICI_FORMAT_UYVY &&
				window->display->s->interlaced && buf2_start) {
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, window->gl.gl_tex_name[1]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
						GL_RGBA, GL_UNSIGNED_BYTE, buf2_start);
		}
	}

	glUseProgram(window->gl.program);

	glUniformMatrix4fv(window->gl.modelview_uniform, 1, GL_FALSE, window->gl.model_view);

	glVertexAttribPointer(window->gl.pos, 3, GL_FLOAT, GL_FALSE, 0, window->gl.hmi_vtx);
	glVertexAttribPointer(window->gl.attr_tex, 2, GL_FLOAT, GL_FALSE, 0,
			window->gl.hmi_tex);
	glEnableVertexAttribArray(window->gl.pos);
	glEnableVertexAttribArray(window->gl.attr_tex);
	glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_BYTE, window->gl.hmi_ind);
	glDisableVertexAttribArray(window->gl.pos);
	glDisableVertexAttribArray(window->gl.attr_tex);
	glBindTexture(GL_TEXTURE_2D, 0);

	wl_surface_set_opaque_region(window->surface, NULL);
	eglSwapBuffers(window->display->egl.dpy, window->egl_surface);

	if(g_triggerOnce && g_GpioClass)
	{
		GPIOControl_outputPattern(g_GpioClass);
		g_triggerOnce = 0;
	}
	if (first_frame_received == 1 && first_frame_rendered == 0) {
		first_frame_rendered = 1;
		GET_TS(time_measurements.first_frame_rendered_time);
		print_time_measurements();
	}
}

void redraw(void *data, struct wl_callback *callback, uint32_t time)
{
	struct window *window = data;
	struct display* disp = window->display;
	struct buffer * buf = (disp->disp_bufs[0]) ? disp->disp_bufs[0]
				: &(window->display->buffers[0]);
	unsigned char *start = NULL;
	unsigned char *buf2_start = NULL;

	if(window->display->s->mem_type == ICI_MEM_DMABUF) {
		start = (unsigned char *) buf->bo->virtual;
		if(window->display->s->interlaced && disp->disp_bufs[1])
			buf2_start = (unsigned char *) disp->disp_bufs[1]->bo->virtual;
	}	else {
		start = (unsigned char *) buf->start;
		if(window->display->s->interlaced && disp->disp_bufs[1])
			buf2_start = (unsigned char *) disp->disp_bufs[1]->start;
	}

	if (callback)
		wl_callback_destroy(callback);

	window->callback = wl_surface_frame(window->surface);
	wl_callback_add_listener(window->callback, &frame_listener, window);

	update_fps(window);

	redraw_egl_way(window, buf, start, buf2_start);
}

void display_add_output(struct display *d, uint32_t id)
{
	struct output *output;

	output = malloc(sizeof *output);
	if (output == NULL)
		return;

	memset(output, 0, sizeof *output);
	output->display = d;
	output->output =
		wl_registry_bind(d->registry, id, &wl_output_interface, 1);
	wl_list_insert(d->output_list.prev, &output->link);
}

void registry_handle_global(void *data, struct wl_registry *registry,
		uint32_t name, const char *interface, uint32_t version)
{
	struct display *d = data;

	if (strcmp(interface, "wl_compositor") == 0) {
		d->compositor =
			wl_registry_bind(registry, name,
					&wl_compositor_interface, 1);
	} else if (strcmp(interface, "wl_shell") == 0) {
			d->wl_shell = wl_registry_bind(registry, name,
					&wl_shell_interface, 1);
	}
	else if (strcmp(interface, "wl_output") == 0) {
		display_add_output(d, name);
	} else if (!strcmp(interface, "wl_drm")) {
		d->wl_drm =
			wl_registry_bind(registry, name, &wl_drm_interface, 1);
	}
}

void init_egl(struct display *display, int opaque)
{
	const char* egl_extensions = NULL;

	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 1,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	EGLint major, minor, n;
	EGLBoolean ret;

	if (opaque)
		config_attribs[9] = 0;

	display->egl.dpy = eglGetDisplay((EGLNativeDisplayType) display->display);
	assert(display->egl.dpy);

	ret = eglInitialize(display->egl.dpy, &major, &minor);
	assert(ret == EGL_TRUE);
	ret = eglBindAPI(EGL_OPENGL_ES_API);
	assert(ret == EGL_TRUE);

	ret = eglChooseConfig(display->egl.dpy, config_attribs,
			&display->egl.conf, 1, &n);
	assert(ret && n == 1);

	display->egl.ctx = eglCreateContext(display->egl.dpy,
			display->egl.conf,
			EGL_NO_CONTEXT, context_attribs);
	assert(display->egl.ctx);

	egl_extensions = eglQueryString(display->egl.dpy, EGL_EXTENSIONS);
	if (strstr(egl_extensions, "EGL_KHR_image_base") &&
	    strstr(egl_extensions, "EXT_image_dma_buf_import")) {
		eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC) eglGetProcAddress("eglCreateImageKHR");
		eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC) eglGetProcAddress("eglDestroyImageKHR");
	}
	BYE_ON(eglCreateImageKHR == NULL, "EGL_KHR_image_base and EXT_image_dma_buf_import not supported\n");
	BYE_ON(eglDestroyImageKHR == NULL, "EGL_KHR_image_base and EXT_image_dma_buf_import not supported\n");
}

void create_surface(struct window *window, void *gpioClass)
{
	struct display *display = window->display;
	EGLBoolean ret;

	printf("create_surface: ");

	g_GpioClass = gpioClass;

	window->surface = wl_compositor_create_surface(display->compositor);
	window->shell_surface = wl_shell_get_shell_surface(display->wl_shell,
			window->surface);
	wl_shell_surface_add_listener(window->shell_surface,
			&wl_shell_surface_listener, window);
	printf("WL_SHELL\n");

	wl_shell_surface_set_title(window->shell_surface, "icitest");

	window->native =
		wl_egl_window_create(window->surface,
				window->window_size.width,
				window->window_size.height);
	window->egl_surface =
		eglCreateWindowSurface(display->egl.dpy,
				display->egl.conf,
				(EGLNativeWindowType) window->native, NULL);

	ret = eglMakeCurrent(window->display->egl.dpy, window->egl_surface,
			     window->egl_surface, window->display->egl.ctx);
	assert(ret == EGL_TRUE);

	toggle_fullscreen(window, window->fullscreen);
}

void init_gl_shaders(struct window *window)
{
	GLint status;
	GLuint frag, vert;
	FILE* pf;
	GLint shader_size;
	GLenum shader_format;
	char *shader_binary;
	unsigned int got_binary_shader = 0;
	unsigned int color_format;

	window->gl.program = glCreateProgram();

	if (glProgramBinaryOES) {
		pf = fopen("shader.bin", "rb");
		if (pf) {
			fread(&shader_size, sizeof(shader_size), 1, pf);
			fread(&shader_format, sizeof(shader_format), 1, pf);
			fread(&color_format, sizeof(color_format), 1, pf);

			if (color_format == window->display->s->in_fourcc) { 
				shader_binary = malloc(shader_size);

				fread(shader_binary, shader_size, 1, pf);

				glProgramBinaryOES(window->gl.program,
						   shader_format,
						   shader_binary,
						   shader_size);

				free(shader_binary);
				got_binary_shader = 1;
			}
			fclose(pf);
		};
	}
	if (!got_binary_shader) {
		vert = create_shader(window, vert_shader_text, GL_VERTEX_SHADER);

		if (window->display->s->in_fourcc == ICI_FORMAT_UYVY) {
			if(window->display->s->interlaced)
				frag = create_shader(window,
						frag_shader_text_UYVY_interlaced,
						GL_FRAGMENT_SHADER);
			else
				frag = create_shader(window, frag_shader_text_UYVY, GL_FRAGMENT_SHADER);
		} else if (window->display->s->in_fourcc == ICI_FORMAT_SGRBG8) {
			frag = create_shader(window, frag_shader_text_SGRBG8, GL_FRAGMENT_SHADER);
		} else {
			frag = create_shader(window, frag_shader_text_RGB, GL_FRAGMENT_SHADER);
		}

		glAttachShader(window->gl.program, frag);
		glAttachShader(window->gl.program, vert);
		glLinkProgram(window->gl.program);
	}

	glGetProgramiv(window->gl.program, GL_LINK_STATUS, &status);
	if (!status) {
		char log[1000];
		GLsizei len;
		glGetProgramInfoLog(window->gl.program, 1000, &len, log);
		fprintf(stderr, "Error: linking:\n%*s\n", len, log);
		exit(1);
	}

	if (glProgramBinaryOES && !got_binary_shader) {
		glGetProgramiv(window->gl.program, GL_PROGRAM_BINARY_LENGTH_OES,
			       &shader_size);

		shader_binary = malloc(shader_size);

		glGetProgramBinaryOES(window->gl.program, shader_size, NULL,
				      &shader_format, shader_binary);

		pf = fopen("shader.bin", "wb");
		if (pf) {
			fwrite(&shader_size, sizeof(shader_size), 1, pf);
			fwrite(&shader_format, sizeof(shader_format), 1, pf);
			fwrite(&window->display->s->in_fourcc,
					sizeof(window->display->s->in_fourcc), 1, pf);
			fwrite(shader_binary, 1, shader_size, pf);

			fclose(pf);
		}
		free(shader_binary);
	}
}


void init_gl(struct window *window)
{
	GLsizei texture_width = window->display->s->stride_width >> 1;
	const GLfloat HMI_W = 1.f;
	const GLfloat HMI_H = 1.f;
	const GLfloat HMI_Z = 0.f;
	const char* gl_extensions = NULL;
	GLint num_binary_program_formats = 0;
	GLfloat u_max = 1.f;
	
	gl_extensions = (const char *) glGetString(GL_EXTENSIONS);

	if (strstr(gl_extensions, "GL_OES_get_program_binary")) {
		glGetIntegerv(GL_PROGRAM_BINARY_FORMATS_OES,
				&num_binary_program_formats);
		if (num_binary_program_formats) {
			glProgramBinaryOES = (PFNGLPROGRAMBINARYOESPROC) eglGetProcAddress("glProgramBinaryOES");
			glGetProgramBinaryOES = (PFNGLGETPROGRAMBINARYOESPROC) eglGetProcAddress("glGetProgramBinaryOES");
		}
	}

	if (strstr(gl_extensions, "GL_OES_EGL_image_external")) {
		glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) eglGetProcAddress("glEGLImageTargetTexture2DOES");
	}

	BYE_ON(glEGLImageTargetTexture2DOES == NULL,
			"glEGLImageTargetTexture2DOES not supported\n");

	init_gl_shaders(window);

	glUseProgram(window->gl.program);

	window->gl.pos = glGetAttribLocation(window->gl.program, "pos");
	window->gl.col = glGetAttribLocation(window->gl.program, "color");
	window->gl.attr_tex = glGetAttribLocation(window->gl.program, "itexcoord");

	window->gl.modelview_uniform =
		glGetUniformLocation(window->gl.program, "modelviewProjection");
	window->gl.gl_texture_size =
		glGetUniformLocation(window->gl.program, "u_texsize");

	glUniform2f(window->gl.gl_texture_size,
			(float)texture_width,
			(float)window->display->s->ih);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	window->gl.gl_tex_sampler[0] = glGetUniformLocation(window->gl.program, "u_texture");
	glUniform1i(window->gl.gl_tex_sampler[0], 0);

	glGenTextures(1, &window->gl.gl_tex_name[0]);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, window->gl.gl_tex_name[0]);

	if (window->display->s->in_fourcc == ICI_FORMAT_UYVY) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width,
				window->display->s->ih, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		if(window->display->s->interlaced) {
			window->gl.gl_tex_sampler[1] = glGetUniformLocation(window->gl.program, "u_texture_bottom");
			glUniform1i(window->gl.gl_tex_sampler[1], 1);

			glGenTextures(1, &window->gl.gl_tex_name[1]);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, window->gl.gl_tex_name[1]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width,
				window->display->s->ih, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glActiveTexture(GL_TEXTURE0 + 0);
		}
	} else if (window->display->s->in_fourcc == ICI_FORMAT_SGRBG8) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, texture_width,
				window->display->s->ih/2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->display->s->stride_width,
				window->display->s->ih, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		window->gl.rgb565 = glGetUniformLocation(window->gl.program, "rgb565");
		glUniform1i(window->gl.rgb565, 0);
		if (window->display->s->in_fourcc == ICI_FORMAT_RGB565)
			glUniform1i(window->gl.rgb565, 1);
	}

	window->gl.swap_rb = glGetUniformLocation(window->gl.program, "swap_rb");
	/* Because GLES does not support BGRA format, red and blue
	 * components must be swapped in shader, when GL_DMA rendering method
	 * is used, texture is created using BGRA layout and swap is not required
	 */
	glUniform1i(window->gl.swap_rb, 1);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glClearColor(.5, .5, .5, .20);

	make_matrix(window->gl.model_view, 1.0);
	u_max = ((GLfloat)window->display->s->iw) /
		((GLfloat)window->display->s->stride_width);
	window->gl.hmi_vtx[0] =  -HMI_W;
	window->gl.hmi_vtx[1] =   HMI_H;
	window->gl.hmi_vtx[2] =   HMI_Z;

	window->gl.hmi_vtx[3] =  -HMI_W;
	window->gl.hmi_vtx[4] =  -HMI_H;
	window->gl.hmi_vtx[5] =   HMI_Z;

	window->gl.hmi_vtx[6] =   HMI_W;
	window->gl.hmi_vtx[7] =   HMI_H;
	window->gl.hmi_vtx[8] =   HMI_Z;

	window->gl.hmi_vtx[9]  =  HMI_W;
	window->gl.hmi_vtx[10] = -HMI_H;
	window->gl.hmi_vtx[11] =  HMI_Z;

	window->gl.hmi_tex[0] = 0.0f;
	window->gl.hmi_tex[1] = 0.0f;
	window->gl.hmi_tex[2] = 0.0f;
	window->gl.hmi_tex[3] = 1.0f;
	window->gl.hmi_tex[4] = u_max;
	window->gl.hmi_tex[5] = 0.0f;
	window->gl.hmi_tex[6] = u_max;
	window->gl.hmi_tex[7] = 1.0f;

	window->gl.hmi_ind[0] = 0;
	window->gl.hmi_ind[1] = 1;
	window->gl.hmi_ind[2] = 3;
	window->gl.hmi_ind[3] = 0;
	window->gl.hmi_ind[4] = 3;
	window->gl.hmi_ind[5] = 2;
	printf("window->gl->gl_tex_sampler0, gl_tex_sampler1 gl_texturre: %d:%d:%d:%d \n", window->gl.gl_tex_sampler[0], window->gl.gl_tex_sampler[1], window->gl.gl_tex_name[0], window->gl.gl_tex_name[1]);
}

int init_gem(struct display *display)
{
	/* Init GEM */
	display->drm_fd = drmOpenRender(128);
	if (display->drm_fd < 0)
		return -1;

	/* In case that drm will be opened before weston will do it, 
	 * master mode needs to be released otherwiser weston won't initialize
	 */
	drmDropMaster(display->drm_fd);

	display->bufmgr = intel_bufmgr_gem_init(display->drm_fd, BATCH_SIZE);

	if (display->bufmgr == NULL)
		return -1;

	return 0;
}

void destroy_gem(struct display *display)
{
	/* Free the GEM buffer */
	drm_intel_bufmgr_destroy(display->bufmgr);
	drmClose(display->drm_fd);
}

int drm_buffer_to_prime(struct display *display, struct buffer *buffer,
		unsigned int size)
{
	int ret;
	buffer->bo = drm_intel_bo_gem_create_from_prime(display->bufmgr,
			buffer->dbuf_fd, (int) size);
	if(!buffer->bo) {
		printf("ERROR: Couldn't create from prime\n");
		return -1;
	}

	// Do a mmap once 
	ret = drm_intel_gem_bo_map_gtt(buffer->bo);
	if(ret) {
		printf("ERROR: Couldn't map buffer->bo\n");
		return -1;
	}

	ret = drm_intel_bo_flink(buffer->bo, &buffer->flink_name);
	if (ret) {
		printf("ERROR: Couldn't flink buffer\n");
		return -1;
	}

	return 0;
}

int create_buffer(struct display *display, struct buffer *buffer,
		unsigned int size)
{
	int ret;

	buffer->bo =  drm_intel_bo_alloc_for_render(display->bufmgr,
			"display surface",
			size,
			0);

	if (buffer->bo == NULL)
		return -1;

	struct drm_prime_handle prime;
	memset(&prime, 0, sizeof prime);
	prime.handle = buffer->bo->handle;

	ret = ioctl(display->drm_fd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &prime);
	if (WARN_ON(ret, "PRIME_HANDLE_TO_FD failed: %s\n", ERRSTR))
		return -1;
	buffer->dbuf_fd = prime.fd;

	// FLINK on render node fails with permission denied
	ret = 0;
	if (ret) {
		printf("ERROR: Couldn't flink buffer\n");
		return -1;
	}

	/* Do a mmap once */
	ret = drm_intel_bo_map(buffer->bo, 1);
	if(ret) {
		printf("ERROR: Couldn't map buf->bo\n");
		return -1;
	}	/* Do a mmap once */
	printf("DMA buffers mapped FD:%d\n",buffer->dbuf_fd);

	return 0;
}
