lib_LTLIBRARIES = libplugin_scrolling_text.la  libplugin_static_picture.la

libplugin_scrolling_text_la_SOURCES = plugins.c scrolling-text-plugin.c opengl-plugin-params-helper.c
libplugin_static_picture_la_SOURCES = plugins.c static-picture.c opengl-plugin-params-helper.c
				  
										       
										   					   					   
AM_CPPFLAGS= $(EGL_CFLAGS) \
             $(GLES_CFLAGS)
            

AM_LDFLAGS= $(EGL_LIBS) \
            $(GLES_LIBS) \
			-lpthread -lm


