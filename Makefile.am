bin_PROGRAMS = refopenglapp


refopenglapp_SOURCES = main.c  opengl-context.c  platform-egl-log.c 
AM_CPPFLAGS= $(EGL_CFLAGS) \
             $(GLES_CFLAGS) \
             $(CAIRO_CFLAGS) 
            

AM_LDFLAGS= $(EGL_LIBS) \
            $(GLES_LIBS) \
            $(CAIRO_LIBS) \
            -lpthread -lm

VALAC=valac

if HAVE_X11
   refopenglapp_SOURCES +=platform-egl-context-x11.c
   AM_CPPFLAGS+=-DHAVE_X11 $(XLIB_CFLAGS)
   AM_LDFLAGS+=$(XLIB_LIBS)
endif

if HAVE_DISPMANX
   refopenglapp_SOURCES += platform-egl-context-dispmanx.c
   AM_CPPFLAGS+=-DHAVE_DISPMANX $(DISPMANX_CFLAGS) $(DISPMANX_INCLUDES)
   AM_LDFLAGS+=$(DISPMANX_LDFLAGS)
endif
