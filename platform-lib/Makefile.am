lib_LTLIBRARIES = libplatformopengl.la



libplatformopengl_la_SOURCES = \
				  opengl-context.c \
				  platform-egl-log.c
										       
										   					   					   
AM_CPPFLAGS= $(EGL_CFLAGS) \
             $(GLES_CFLAGS)
            

AM_LDFLAGS= $(EGL_LIBS) \
            $(GLES_LIBS) \
			-lpthread -lm



if HAVE_X11
   libplatformopengl_la_SOURCES +=platform-egl-context-x11.c
   AM_CPPFLAGS+=-DHAVE_X11 $(XLIB_CFLAGS)
   AM_LDFLAGS+=$(XLIB_LIBS)
endif

if HAVE_DISPMANX
   libplatformopengl_la_SOURCES += platform-egl-context-dispmanx.c
   AM_CPPFLAGS+=-DHAVE_DISPMANX $(DISPMANX_CFLAGS) $(DISPMANX_INCLUDES)
   AM_LDFLAGS+=$(DISPMANX_LDFLAGS)
endif
