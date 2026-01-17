/*
 * opengl-context-priv.h
 *
 *  Created on: Jul 31, 2017
 *      Author: ionut
 */

#ifndef OPENGL_CONTEXT_PRIV_H_
#define OPENGL_CONTEXT_PRIV_H_

#include "opengl-context.h"



typedef struct tagOpenGLContext
{
	PlatformEGLContext*  		m_egl_context;
	GLuint 						m_fragment_shader;
	GLuint						m_vertex_shader;
	GLuint						m_program;
	void *						m_private_data;
} OpenGLContext;

void opengl_wiewport(OpenGLContext* eglctx,
							   unsigned short orig_x,
							   unsigned short orig_y,
							   unsigned short width,
							   unsigned short height);


#endif /* OPENGL_CONTEXT_PRIV_H_ */
