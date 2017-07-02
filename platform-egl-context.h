/*
 * platform_egl.h
 *
 *  Created on: Jun 25, 2017
 *      Author: ionut
 */

#ifndef PLATFORM_EGL_H_
#define PLATFORM_EGL_H_

// Todo: move to disppmanx egl context
#ifdef HAVE_DISPMANX
#include "bcm_host.h"
#endif

#ifdef HAVE_DISPMANX
	   DISPMANX_DISPLAY_HANDLE_T m_native_dispmanx_display;
	   DISPMANX_ELEMENT_HANDLE_T m_native_dispmanx_element;
	   EGL_DISPMANX_WINDOW_T 	 m_native_dispmanx_window;
#endif



typedef struct tagPlatformEGLContext PlatformEGLContext;


extern PlatformEGLContext* platform_egl_context_create();
extern void platform_egl_context_destroy( PlatformEGLContext *ctx );
extern int  platform_egl_context_init( PlatformEGLContext *ctx );
extern void platform_egl_context_deinit( PlatformEGLContext *ctx );
extern void platform_egl_context_mainloop( PlatformEGLContext *ctx );
extern void platform_egl_context_swap_buffers( PlatformEGLContext *ctx );


#endif /* PLATFORM_EGL_H_ */
