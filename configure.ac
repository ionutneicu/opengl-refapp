AC_INIT([ReferenceApp], [1.0], [ionut.neicu@gmail.com], [refapp])
#AC_CONFIG_AUX_DIR([build-aux])
#-- these may be in the file already, or you may need to modify the existing lines.
AC_PREREQ(2.62)
AM_INIT_AUTOMAKE([-Wno-portability])


AC_CONFIG_MACRO_DIR([build-aux])


# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL


		


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

PKG_CHECK_MODULES([GLES], [glesv2] )
AC_SUBST(GLES_CFLAGS)
AC_SUBST(GLES_LIBS)

PKG_CHECK_MODULES([EGL], [egl] )
AC_SUBST(EGL_CFGLAGS)
AC_SUBST(EGL_LIBS)
])

AM_COND_IF([HAVE_DISPMANX], [
AC_SUBST(DISPMANX_CFLAGS, "-DPLATFORM_RPI -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi" )
AC_SUBST(DISPMANX_LDFLAGS, "-L/opt/vc/lib/ -lbrcmGLESv2 -lbrcmEGL -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -lm" )
AC_SUBST(DISPMANX_INCLUDES, "-I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux")
])


PKG_CHECK_MODULES([CAIRO], [cairo] )
AC_SUBST(CAIRO_CFGLAGS)
AC_SUBST(CAIRO_LIBS)


AC_CONFIG_FILES([
				Makefile
                platform-lib/Makefile                
				plugins/Makefile
				app/Makefile				
				tests/plugininitializertests/Makefile				
        ])
        
AC_OUTPUT
