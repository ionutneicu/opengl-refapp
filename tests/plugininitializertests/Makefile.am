bin_PROGRAMS = testplugininitializer

testplugininitializer_SOURCES = test-plugin-initializer.cpp \
					   ../../app/opengl-plugin-params-helper.c
					   	      
					   
#testplugininitializer_LDADD = ../../platform-lib/libplatformopengl.la					   
					   					   
AM_CPPFLAGS= $(EGL_CFLAGS) \
             $(GLES_CFLAGS) \
             $(CAIRO_CFLAGS) \
             -I ../../app/
            

AM_LDFLAGS= $(EGL_LIBS) \
            $(GLES_LIBS) \
            $(CAIRO_LIBS) \
            -lpthread -lm -lcunit

VALAC=valac
