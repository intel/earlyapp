/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Build the Wayland clients */
#define BUILD_CLIENTS 1

/* Build the DRM compositor */
#define BUILD_DRM_COMPOSITOR 1

/* Build the fbdev compositor */
#define BUILD_FBDEV_COMPOSITOR 1

/* Build with frame capture support */
/* #undef BUILD_FRAME_CAPTURE */

/* Build the headless compositor */
#define BUILD_HEADLESS_COMPOSITOR 1

/* Build the IAS compositor */
#define BUILD_IAS_COMPOSITOR 1

/* Build the RDP compositor */
/* #undef BUILD_RDP_COMPOSITOR */

/* Build remote display */
/* #undef BUILD_REMOTE_DISPLAY */

/* Build the vaapi recorder */
#define BUILD_VAAPI_RECORDER 1

/* Build the Wayland (nested) compositor */
/* #undef BUILD_WAYLAND_COMPOSITOR */

/* Build the wcap tools */
#define BUILD_WCAP_TOOLS 1

/* Build the X11 compositor */
/* #undef BUILD_X11_COMPOSITOR */

/* Build the X server launcher */
/* #undef BUILD_XWAYLAND */

/* Build Weston with EGL support */
#define ENABLE_EGL 1

/* Build Weston with JUnit output support */
#define ENABLE_JUNIT_XML 1

/* Enable startup timing */
/* #undef ENABLE_TRACING */

/* Build Weston with libxkbcommon support */
/* #undef ENABLE_XKBCOMMON */

/* Generate binary shaders */
#define GENERATE_BINARY_SHADERS 1

/* Have cairo-egl */
/* #undef HAVE_CAIRO_EGL */

/* Build with dbus support */
#define HAVE_DBUS 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* libdrm supports atomic API */
#define HAVE_DRM_ATOMIC 1

/* Define to 1 if you have the <execinfo.h> header file. */
#define HAVE_EXECINFO_H 1

/* Define to 1 if you have the <freerdp/version.h> header file. */
/* #undef HAVE_FREERDP_VERSION_H */

/* gbm supports dmabuf import */
#define HAVE_GBM_FD_IMPORT 1

/* Define to 1 if you have the `initgroups' function. */
#define HAVE_INITGROUPS 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Have jpeglib */
#define HAVE_JPEG 1

/* Have lcms support */
#define HAVE_LCMS 1

/* Build freedreno dmabuf client */
/* #undef HAVE_LIBDRM_FREEDRENO */

/* Build intel dmabuf client */
#define HAVE_LIBDRM_INTEL 1

/* Define to 1 if you have the <linux/sync_file.h> header file. */
#define HAVE_LINUX_SYNC_FILE_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkostemp' function. */
#define HAVE_MKOSTEMP 1

/* Have pango */
#define HAVE_PANGO 1

/* Define to 1 if you have the `posix_fallocate' function. */
#define HAVE_POSIX_FALLOCATE 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchrnul' function. */
#define HAVE_STRCHRNUL 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Have systemd-login */
#define HAVE_SYSTEMD_LOGIN 1

/* Have systemd-login >= 209 */
#define HAVE_SYSTEMD_LOGIN_209 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Have webp */
#define HAVE_WEBP 1

/* libxcb supports XKB protocol */
/* #undef HAVE_XCB_XKB */

/* Define if xkbcommon is 0.5.0 or newer */
/* #undef HAVE_XKBCOMMON_COMPOSE */

/* Enable hyper dmabuf support */
/* #undef HYPER_DMABUF */

/* Build with ivi-share protocol */
/* #undef IVI_SHARE_ENABLE */

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to 1 if `major', `minor', and `makedev' are declared in <mkdev.h>.
   */
/* #undef MAJOR_IN_MKDEV */

/* Define to 1 if `major', `minor', and `makedev' are declared in
   <sysmacros.h>. */
#define MAJOR_IN_SYSMACROS 1

/* Name of package */
#define PACKAGE "weston"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://bugs.freedesktop.org/enter_bug.cgi?product=Wayland&component=weston&version=4.0.2"

/* Define to the full name of this package. */
#define PACKAGE_NAME "weston"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "weston 4.0.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "weston"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://wayland.freedesktop.org"

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.0.2"

/* Profile remote display */
/* #undef PROFILE_REMOTE_DISPLAY */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Build the systemd sd_notify support */
/* #undef SYSTEMD_NOTIFY_SUPPORT */

/* Text backend for onscreen keyboard */
/* #undef TEXT_BACKEND */

/* Trace buffer size */
#define TRACE_BUFFER_SIZE 0

/* Use the GLESv2 GL cairo backend */
/* #undef USE_CAIRO_GLESV2 */

/* Use resize memory pool as a performance optimization */
#define USE_RESIZE_POOL 1

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Enable VM support */
/* #undef USE_VM */

/* Version number of package */
#define VERSION "4.0.2"

/* The default backend to load, if not wayland nor x11. */
#define WESTON_NATIVE_BACKEND "drm-backend.so"

/* The default desktop shell client to load. */
#define WESTON_SHELL_CLIENT "weston-desktop-shell"

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */
