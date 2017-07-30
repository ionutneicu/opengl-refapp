/*
 * platform-opengl-draw.h
 *
 *  Created on: Jul 1, 2017
 *      Author: ionut
 */

#ifndef PLATFORM_OPENGL_DRAW_H_
#define PLATFORM_OPENGL_DRAW_H_

#include "platform-egl-context.h"

#include <GLES2/gl2.h>

typedef struct tagPlatformOpenGLContext
{
	PlatformEGLContext*  		m_egl_context;
	GLuint						m_texture;
	GLuint 						m_fragment_shader;
	GLuint						m_vertex_shader;
	GLuint						m_program;
} PlatformOpenGLContext;

typedef struct tagPlatformOpenGLContext PlatformOpenGLContext;

PlatformOpenGLContext* platform_opengl_context_create( PlatformEGLContext* eglctx );

void platform_opengl_init( PlatformOpenGLContext* eglctx);
void platform_opengl_draw(PlatformOpenGLContext* eglctx);
void platform_opengl_wiewport(PlatformOpenGLContext* eglctx,
							   unsigned short orig_x,
							   unsigned short orig_y,
							   unsigned short width,
							   unsigned short height);


extern void platform_opengl_mainloop( PlatformOpenGLContext * ctx );

#endif /* PLATFORM_OPENGL_DRAW_H_ */
