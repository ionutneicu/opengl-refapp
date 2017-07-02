/*
 * platform-opengl-draw.c
 *
 *  Created on: Jul 1, 2017
 *      Author: ionut
 */
#include "platform-egl-context.h"
#include "platform-egl-context-priv.h"
#include "platform-opengl-draw.h"
#include "platform-egl-log.h"


#include <GLES2/gl2.h>

#define GL_CHECK_ERROR( __x__ ) __x__ ; if ((error=glGetError()) != GL_NO_ERROR) { LERROR("-----> %s %s: ERROR %d at line %d\n", #__x__, __FUNCTION__ , error,  __LINE__ ); }
int error;

static const char *vertex_source =
	"attribute vec4 position;\n"
	"attribute vec4 color;\n"
	"\n"
	"varying vec4 vcolor;\n"
	"\n"
	"void main()\n"
	"{\n"
	"  gl_Position = position;\n"
	"  vcolor = color;\n"
	"}\n";

static const char *fragment_source =
	"precision mediump float;\n"
	"varying vec4 vcolor;\n"
	"\n"
	"void main()\n"
	"{\n"
	"  gl_FragColor = vcolor;\n"
	"}\n";

static GLfloat vertices[] = {
	 0.0f,  0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
};

static GLfloat colors[] = {
	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
};


void platform_opengl_draw(PlatformEGLContext* eglctx)
{
	static int need_init = 1;
	GLuint vertex, fragment, program;
	if( ! need_init )
		goto partial_draw;

	GL_CHECK_ERROR( vertex = glCreateShader(GL_VERTEX_SHADER) )
	GL_CHECK_ERROR( glShaderSource(vertex, 1, &vertex_source, NULL) )
	GL_CHECK_ERROR( glCompileShader(vertex) )

	GL_CHECK_ERROR( fragment = glCreateShader(GL_FRAGMENT_SHADER) )
	GL_CHECK_ERROR( glShaderSource(fragment, 1, &fragment_source, NULL) )
	GL_CHECK_ERROR( glCompileShader(fragment) );

	GL_CHECK_ERROR( program = glCreateProgram() )
	GL_CHECK_ERROR( glAttachShader(program, vertex) )
	GL_CHECK_ERROR( glAttachShader(program, fragment) )

	GL_CHECK_ERROR( glBindAttribLocation(program, 0, "position") )
	GL_CHECK_ERROR( glBindAttribLocation(program, 1, "color") )

	LINFO("=== Calling glLinkProgram()\n");
	GL_CHECK_ERROR( glLinkProgram(program) )
	GL_CHECK_ERROR( glFlush() )

	LINFO("=== Calling glUseProgram()\n");
	GL_CHECK_ERROR( glUseProgram(program) )
	GL_CHECK_ERROR( glFlush() )

	LINFO("=== calling glViewport()\n");
	GL_CHECK_ERROR( glViewport(0, 0, eglctx->m_width, eglctx->m_height) )
	GL_CHECK_ERROR( glFlush() )

	LINFO("=== calling glClearColor()\n");

	GL_CHECK_ERROR( glClearColor(0.0f, 0.0f, 0.0f, 1.0f) )
	GL_CHECK_ERROR( glFlush() )


    need_init = 0;
partial_draw:
	LINFO("=== calling glClear()\n");

	GL_CHECK_ERROR( glClear(GL_COLOR_BUFFER_BIT) )
	GL_CHECK_ERROR( glFlush() )

	LINFO("=== Calling glVertexAttribPointer(0)\n");

	GL_CHECK_ERROR( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices) )
	GL_CHECK_ERROR( glFlush() )

	LINFO("=== Calling glEnableVertexAttribArray(0)\n");

	GL_CHECK_ERROR( glEnableVertexAttribArray(0) )
	GL_CHECK_ERROR( glFlush() )

	LINFO("=== Calling glVertexAttribPointer(1)\n");

	GL_CHECK_ERROR( glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, colors) )
	GL_CHECK_ERROR( glFlush() )

	LINFO("=== Calling glEnableVertexAttribArray(1)\n");
	GL_CHECK_ERROR( glEnableVertexAttribArray(1) )
	GL_CHECK_ERROR( glFlush() )

	LINFO("=== Calling glDrawArrays(GL_TRIANGLES, 0, 3)\n");

	GL_CHECK_ERROR( glDrawArrays(GL_TRIANGLES, 0, 3) )
	GL_CHECK_ERROR( glFlush() )

	LINFO("=== calling eglSwapBuffers()\n");

	platform_egl_context_swap_buffers(eglctx);
	GL_CHECK_ERROR( glFlush() )
	LINFO("=== done\n");
}


void platform_opengl_wiewport(PlatformEGLContext* eglctx,
							   unsigned short orig_x,
							   unsigned short orig_y,
							   unsigned short width,
							   unsigned short height)
{
	eglctx->m_width =  width;
	eglctx->m_height = height;
	LINFO("=== calling glViewport()\n");
	GL_CHECK_ERROR( glViewport(orig_x, orig_y, eglctx->m_width, eglctx->m_height) )
	GL_CHECK_ERROR( glFlush() )
}
