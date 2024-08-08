/*
 * platform-egl-context-priv.h
 *
 *  Created on: Jul 1, 2017
 *      Author: ionut
 */

#ifndef PLATFORM_EGL_CONTEXT_PRIV_H_
#define PLATFORM_EGL_CONTEXT_PRIV_H_


#include <EGL/egl.h>


typedef struct tagPlatformEGLContext
{
	   EGLSurface 		 m_egl_surf;
	   EGLContext 		 m_egl_ctx;
	   EGLDisplay 		 m_egl_dpy;
	   uint32_t			 m_width;
	   uint32_t		     m_height;
	   unsigned short	 m_initialized;
} PlatformEGLContext;



#endif /* PLATFORM_EGL_CONTEXT_PRIV_H_ */
