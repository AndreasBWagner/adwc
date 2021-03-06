AC_PREREQ([2.64])
AC_INIT([weston],
        [0.89],
        [https://bugs.freedesktop.org/enter_bug.cgi?product=weston],
        [weston],
        [http://wayland.freedesktop.org/])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 foreign no-dist-gzip dist-xz])

AM_SILENT_RULES([yes])

# Check for programs
AC_PROG_CC

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT([disable-static])

PKG_PROG_PKG_CONFIG()

AC_CHECK_FUNC([dlopen], [],
              AC_CHECK_LIB([dl], [dlopen], DLOPEN_LIBS="-ldl"))
AC_SUBST(DLOPEN_LIBS)

PKG_CHECK_MODULES(COMPOSITOR,
		  [wayland-server egl >= 7.10 glesv2])

AC_CHECK_PROG(RSVG_CONVERT, rsvg-convert, rsvg-convert)
AM_CONDITIONAL(HAVE_RSVG_CONVERT, test -n "$RSVG_CONVERT")


AC_ARG_ENABLE(setuid-install, [  --enable-setuid-install],,
	      enable_setuid_install=yes)
AM_CONDITIONAL(ENABLE_SETUID_INSTALL, test x$enable_setuid_install = xyes)


AC_ARG_ENABLE(xserver-launcher, [  --enable-xserver-launcher],,
	      enable_xserver_launcher=yes)
AM_CONDITIONAL(ENABLE_XSERVER_LAUNCHER, test x$enable_xserver_launcher = xyes)
if test x$enable_xserver_launcher = xyes; then
  PKG_CHECK_MODULES([XSERVER_LAUNCHER], xcb xcb-xfixes)
  AC_DEFINE([BUILD_XSERVER_LAUNCHER], [1], [Build the X server launcher])

  AC_ARG_WITH(xserver-path, AS_HELP_STRING([--with-xserver-path=PATH],
              [Path to X server]), [XSERVER_PATH="$withval"],
              [XSERVER_PATH="$bindir/Xorg"])
  AC_SUBST([XSERVER_PATH])
fi


AC_ARG_ENABLE(x11-compositor, [  --enable-x11-compositor],,
	      enable_x11_compositor=yes)
AM_CONDITIONAL(ENABLE_X11_COMPOSITOR, test x$enable_x11_compositor = xyes)
if test x$enable_x11_compositor = xyes; then
  PKG_CHECK_MODULES([XCB], xcb)
  xcb_save_LIBS=$LIBS
  xcb_save_CFLAGS=$CFLAGS
  CFLAGS=$XCB_CFLAGS
  LIBS=$XCB_LIBS
  AC_CHECK_FUNCS([xcb_poll_for_queued_event])
  LIBS=$xcb_save_LIBS
  CFLAGS=$xcb_save_CFLAGS

  AC_DEFINE([BUILD_X11_COMPOSITOR], [1], [Build the X11 compositor])
  PKG_CHECK_MODULES(X11_COMPOSITOR, [x11 x11-xcb])
fi


AC_ARG_ENABLE(drm-compositor, [  --enable-drm-compositor],,
	      enable_drm_compositor=yes)
AM_CONDITIONAL(ENABLE_DRM_COMPOSITOR, test x$enable_drm_compositor = xyes)
if test x$enable_drm_compositor = xyes; then
  AC_DEFINE([BUILD_DRM_COMPOSITOR], [1], [Build the DRM compositor])
  PKG_CHECK_MODULES(DRM_COMPOSITOR, [libudev >= 136 libdrm >= 2.4.30 gbm mtdev])
fi


AC_ARG_ENABLE(openwfd-compositor, [  --enable-openwfd-compositor],,
	      enable_openwfd_compositor=no)
AM_CONDITIONAL(ENABLE_OPENWFD_COMPOSITOR, test x$enable_openwfd_compositor = xyes)
if test x$enable_openwfd_compositor = xyes; then
  AC_DEFINE([BUILD_OPENWFD_COMPOSITOR], [1], [Build the OpenWF compositor])
  PKG_CHECK_MODULES(OPENWFD_COMPOSITOR, [openwfd gbm])
fi


AC_ARG_ENABLE(wayland-compositor, [  --enable-wayland-compositor],,
	      enable_wayland_compositor=yes)
AM_CONDITIONAL(ENABLE_WAYLAND_COMPOSITOR,
	       test x$enable_wayland_compositor = xyes)
if test x$enable_wayland_compositor = xyes; then
  AC_DEFINE([BUILD_WAYLAND_COMPOSITOR], [1],
	    [Build the Wayland (nested) compositor])
  PKG_CHECK_MODULES(WAYLAND_COMPOSITOR, [wayland-client wayland-egl])
fi

AC_ARG_WITH(cairo-glesv2, AS_HELP_STRING([--with-cairo-glesv2],
              [Use GLESv2 cairo instead of full GL]),
              [cairo_modules="cairo-glesv2"],
              [cairo_modules="cairo-gl"])
if test x$cairo_modules = xcairo-glesv2; then
  AC_DEFINE([USE_CAIRO_GLESV2], [1], [Use the GLESv2 GL cairo backend])
fi

PKG_CHECK_MODULES(PIXMAN, [pixman-1])
PKG_CHECK_MODULES(PNG, [libpng])
PKG_CHECK_MODULES(WEBP, [libwebp], [have_webp=yes], [have_webp=no])
AS_IF([test "x$have_webp" = "xyes"],
      [AC_DEFINE([HAVE_WEBP], [1], [Have webp])])
IMAGE_LIBS="$PIXMAN_LIBS $PNG_LIBS $WEBP_LIBS"
IMAGE_CFLAGS="$PIXMAN_CFLAGS $PNG_CFLAGS $WEBP_CFLAGS"
AC_SUBST(IMAGE_LIBS)
AC_SUBST(IMAGE_CFLAGS)

AC_CHECK_LIB([jpeg], [jpeg_CreateDecompress], have_jpeglib=yes)
if test x$have_jpeglib = xyes; then
  IMAGE_LIBS="$IMAGE_LIBS -ljpeg"
else
  AC_ERROR([libjpeg not found])
fi

COMPOSITOR_LIBS="$COMPOSITOR_LIBS $IMAGE_LIBS"
COMPOSITOR_CFLAGS="$COMPOSITOR_CFLAGS $IMAGE_CFLAGS"

AC_ARG_ENABLE(simple-clients, [  --enable-simple-clients],, enable_simple_clients=yes)
AM_CONDITIONAL(BUILD_SIMPLE_CLIENTS, test x$enable_simple_clients = xyes)
if test x$enable_simple_clients = xyes; then
  AC_DEFINE([BUILD_SIMPLE_CLIENTS], [1], [Build the Wayland simple clients])
  PKG_CHECK_MODULES(SIMPLE_CLIENT,
                    [egl >= 7.10 glesv2 wayland-client wayland-egl])
fi

AC_ARG_ENABLE(clients, [  --enable-clients],, enable_clients=yes)
AM_CONDITIONAL(BUILD_CLIENTS, test x$enable_clients = xyes)
if test x$enable_clients = xyes; then
  AC_DEFINE([BUILD_CLIENTS], [1], [Build the Wayland clients])

  PKG_CHECK_MODULES(CLIENT, [wayland-client wayland-egl egl >= 7.10 cairo >= 1.10.0 xkbcommon])

  CLIENT_CFLAGS="$CLIENT_CFLAGS $IMAGE_CFLAGS"
  CLIENT_LIBS="$CLIENT_LIBS $IMAGE_LIBS"

  PKG_CHECK_MODULES(POPPLER, [poppler-glib glib-2.0 gobject-2.0 gio-2.0 ],
			     [have_poppler=yes], [have_poppler=no])
  PKG_CHECK_MODULES(CAIRO_EGL, [cairo-egl >= 1.11.3 $cairo_modules],
		    [have_cairo_egl=yes], [have_cairo_egl=no])
  AS_IF([test "x$have_cairo_egl" = "xyes"],
        [AC_DEFINE([HAVE_CAIRO_EGL], [1], [Have cairo-egl])],
        [AC_MSG_WARN([Cairo-EGL not found - clients will use cairo image])])
fi

AC_ARG_ENABLE(weston-launch, [  --enable-weston-launch],, enable_weston_launch=yes)
AM_CONDITIONAL(BUILD_WESTON_LAUNCH, test x$enable_weston_launch == xyes)
if test x$enable_weston_launch == xyes; then
  PKG_CHECK_MODULES(WESTON_LAUNCH, [libdrm])
  PKG_CHECK_MODULES(SYSTEMD_LOGIN, [libsystemd-login],
		    [have_systemd_login=yes], [have_systemd_login=no])
  AS_IF([test "x$have_systemd_login" = "xyes"],
	[AC_DEFINE([HAVE_SYSTEMD_LOGIN], [1], [Have systemd-login])])

  AC_CHECK_LIB([pam], [pam_open_session], [have_pam=yes], [have_pam=no])
  if test x$have_pam == xno; then
    AC_ERROR([weston-launch requires pam])
  fi
  WESTON_LAUNCH_LIBS="$WESTON_LAUNCH_LIBS -lpam"
fi

AM_CONDITIONAL(HAVE_POPPLER, test "x$have_poppler" = "xyes")

AM_CONDITIONAL(BUILD_FULL_GL_CLIENTS,
	       test x$cairo_modules = "xcairo-gl" -a "x$have_cairo_egl" = "xyes")

AM_CONDITIONAL(ENABLE_DESKTOP_SHELL, true)

AC_ARG_ENABLE(tablet-shell, [  --enable-tablet-shell],,
	      enable_tablet_shell=yes)
AM_CONDITIONAL(ENABLE_TABLET_SHELL,
	       test x$enable_tablet_shell = xyes)

if test "x$GCC" = "xyes"; then
	GCC_CFLAGS="-Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes \
		-Wno-unused-parameter -Wno-missing-field-initializers \
		-g -fvisibility=hidden"

fi
AC_SUBST(GCC_CFLAGS)

WAYLAND_SCANNER_RULES(['$(top_srcdir)/protocol'])

AC_CONFIG_FILES([Makefile
		 shared/Makefile
		 src/Makefile
		 clients/Makefile
		 data/Makefile
		 protocol/Makefile
		 tests/Makefile])
AC_OUTPUT

if test "x$enable_setuid_install" = xyes; then
AC_MSG_WARN([

 *** You've enabled the setuid install hook.  Weston is still a 
 *** pre-alpha project and may have bugs and issues that make a
 *** setuid install unsafe. Proceed at your own risk.
])
fi

