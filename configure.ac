AC_INIT([Advertplayer], [1.0], [ionut.neicu@inforide.eu], [advertplay])
#AC_CONFIG_AUX_DIR([build-aux])
#-- these may be in the file already, or you may need to modify the existing lines.
AC_PREREQ(2.62)
AM_INIT_AUTOMAKE([-Wno-portability])

#
# Gobject Introspection
#
#-- add the correct m4 directory location.
AC_CONFIG_MACRO_DIR([build-aux])

#-- any typos here, and you will end up with a message about HAVE_INTROSPECTION not being defined
GOBJECT_INTROSPECTION_CHECK([1.30.0])
VAPIGEN_CHECK([1.0])

# library versioning
#AUXIN_PHONON_PLAYER_LIB_VERSION=0:1:0
#AC_SUBST([AUXIN_PHONON_PLAYER_LIB_VERSION])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

### check for boost signals library
#AX_BOOST_BASE([1.40])
#AX_BOOST_SIGNALS
	
		
AC_ARG_ENABLE([profiling],
[  --enable-profiling    Enable profiling using gprof],
[case "${enableval}" in
  yes) enable-profiling=true ;;
  no)  enable-profiling=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-profiling]) ;;
esac],[enable-profiling=false])
AM_CONDITIONAL([HAVE_PROFILING], [test x$enable-profiling = xtrue])

AC_ARG_ENABLE([x11],
[  --enable-x11    Enable X11 support],
[case "${enableval}" in
  yes) enable_x11=true ;;
  no)  enable_x11=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-x11]) ;;
esac],[enable_x11=false])
AM_CONDITIONAL([HAVE_X11], [test x$enable_x11 = xtrue])

AC_ARG_ENABLE([dispmanx],
[  --enable-dispmanx    Enable Raspberry PI DispmanX window support],
[case "${enableval}" in
  yes) enable_dispmanx=true ;;
  no)  enable_dispmanx=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-dispmanx]) ;;
esac],[enable_dispmanx=false])
AM_CONDITIONAL([HAVE_DISPMANX], [test x$enable_dispmanx = xtrue])

AM_COND_IF([HAVE_X11], [
PKG_CHECK_MODULES([XLIB], [x11])
AC_SUBST(XLIB_CFLAGS)
AC_SUBST(XLIB_LIBS)
])

AM_COND_IF([HAVE_X11], [
PKG_CHECK_MODULES([GLES], [glesv2] )
AC_SUBST(GLES_CFLAGS)
AC_SUBST(GLES_LIBS)
])

AM_COND_IF([HAVE_DISPMANX], [
AC_SUBST(DISPMANX_LIBS, "-lvcos -lvchiq_arm -lbcm_host -lpcre")
AC_SUBST(DISPMANX_CFLAGS, -I/opt/vc/include)
])


PKG_CHECK_MODULES([EGL], [egl] )
AC_SUBST(EGL_CFGLAGS)
AC_SUBST(EGL_LIBS)

PKG_CHECK_MODULES([CAIRO], [cairo] )
AC_SUBST(CAIRO_CFGLAGS)
AC_SUBST(CAIRO_LIBS)

AC_CONFIG_FILES([
                Makefile
        ])
        
AC_OUTPUT
