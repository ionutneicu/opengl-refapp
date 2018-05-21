bin_PROGRAMS = refopenglapp

refopenglapp_SOURCES = main.c \
					   opengl-plugin-params-helper.c \
					   opengl-plugin-registry.c \
					   opengl-user-context-helper.c
					   
refopenglapp_LDADD = ../platform-lib/libplatformopengl.la					   
					   					   
AM_CPPFLAGS= $(EGL_CFLAGS) \
             $(GLES_CFLAGS) \
             $(CAIRO_CFLAGS) \
             -I ../platform-lib \
             -I ../plugins
             
            

AM_LDFLAGS= $(EGL_LIBS) \
            $(GLES_LIBS) \
            $(CAIRO_LIBS) \
            -lpthread -lm 

VALAC=valac
