/*
 * opengl-context.c
 *
 *  Created on: Jul 1, 2017
 *      Author: ionut
 */
#include "platform-egl-context.h"
#include "platform-egl-context-priv.h"
#include "opengl-context.h"
#include "opengl-context-priv.h"
#include "platform-egl-log.h"






#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>


#define NOTHING_TO_DRAW -1
#define SUCCCESS		0


#define GL_CHECK_ERROR( __x__ ) __x__ ; if ((opengl_error=glGetError()) != GL_NO_ERROR) { LERROR("-----> %s %s: ERROR %d at line %d", #__x__, __FUNCTION__ , opengl_error,  __LINE__ ); }
#define GL_CHECK_ERROR_OR_RETURN( __x__ ) __x__ ; if ((opengl_error=glGetError()) != GL_NO_ERROR) { LERROR("-----> %s %s: ERROR %d at line %d", #__x__, __FUNCTION__ , opengl_error,  __LINE__ ); return opengl_error; }
int opengl_error;

#define MILIS( __x__ )  ( ( __x__.tv_sec*1000 )  + ( __x__.tv_nsec / 1000000 ) )

static const char *vertex_source =
		  "attribute vec4 a_position;   \n"
		  "attribute vec2 a_texCoord;   \n"
		  "varying vec2 v_texCoord;     \n"
		  "void main()                  \n"
		  "{                            \n"
		  "   gl_Position = a_position; \n"
		  "   v_texCoord = a_texCoord;  \n"
		  "}                            \n";

static const char *fragment_source =
		  "precision mediump float;                            \n"
		  "varying vec2 v_texCoord;                            \n"
		  "uniform sampler2D tex;                              \n"
		  "void main()                                         \n"
		  "{                                                   \n"
		  "  gl_FragColor = texture2D( tex, v_texCoord );      \n"
		  "}						       \n";





int opengl_load_texture_in_gpu( void* texture_data, unsigned int tex_width, unsigned int tex_height, GLuint *p_tex_value )
{
	/*TODO: this function does not return any error !!!!*/
	GL_CHECK_ERROR_OR_RETURN( glGenTextures(1, p_tex_value) )
	GL_CHECK_ERROR_OR_RETURN( glBindTexture(GL_TEXTURE_2D, *p_tex_value) )
	/*Always load as RGBA */
	GL_CHECK_ERROR_OR_RETURN( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data) )
	GL_CHECK_ERROR_OR_RETURN( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST) );
	GL_CHECK_ERROR_OR_RETURN( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST) );
	GL_CHECK_ERROR_OR_RETURN( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
	GL_CHECK_ERROR_OR_RETURN( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
	LDEBUG("texture_id = %d %d x %d", *p_tex_value, tex_width, tex_height);
	return GL_NO_ERROR;
}

void opengl_unload_texture_from_gpu( GLuint texture )
{
      GL_CHECK_ERROR( glDeleteTextures(1, &texture ))
}


void opengl_draw_texture( OpenGLContext* ctx, GLuint texture, const float scale_factor )
{

			static const GLfloat texices[] = { 0, 1,
			                            0, 0,
			                            1, 0,
			                            1, 1 };


			GLfloat vertices[] = { -1, -1, 0,  // bottom left corner
			                       -1,  1, 0,  // top left corner
			                        1,  1, 0,  // top right corner
			                        1, -1, 0}; // bottom right corner

			static const GLubyte indices[] =  { 0, 2, 1,     // first triangle (bottom left - top left - top right)
											    0, 3, 2 };


			int i;

			for( i = 0; i < sizeof( vertices ) /  sizeof( GLfloat ); ++ i )
				vertices[i] = vertices[i]*scale_factor;

			glUseProgram ( ctx->m_program  );
			// Load the vertex position
			GLint positionLoc = glGetAttribLocation ( ctx->m_program , "a_position" );
			glVertexAttribPointer ( positionLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), vertices );
			// Load the texture coordinate
			GLint texCoordLoc = glGetAttribLocation ( ctx->m_program , "a_texCoord");
			glVertexAttribPointer ( texCoordLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), texices);
			glEnableVertexAttribArray ( positionLoc );
			glEnableVertexAttribArray ( texCoordLoc );

			glActiveTexture ( GL_TEXTURE0 );
			glBindTexture (GL_TEXTURE_2D, texture);

			GLint tex = glGetUniformLocation ( ctx->m_program  , "tex");
			glUniform1i ( tex, 0 );
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

			glDisableVertexAttribArray( texCoordLoc );
			glDisableVertexAttribArray( texCoordLoc );

}




int opengl_draw(OpenGLContext* opengl_ctx, user_loop_function_pf user_loop)
{

	struct timespec current;
	static int num_frames = 0;
	static struct timespec past = { 0 };
	static long milis = 0;
	int rc;

	if( past.tv_sec == 0 )
	{
		clock_gettime( CLOCK_REALTIME, &past );
	}

	GL_CHECK_ERROR( glClear(GL_COLOR_BUFFER_BIT) )

	if( user_loop )
	{
		rc = user_loop( opengl_ctx );
		if( rc )
			return rc;
	}
	else
		return NOTHING_TO_DRAW;

	platform_egl_context_swap_buffers(opengl_ctx->m_egl_context);


	clock_gettime(CLOCK_REALTIME, &current);
	milis = MILIS( current ) - MILIS(past);

	++num_frames;


	if( milis  > 1000 )
	{

		LINFO("FPS = %.2f ",   1000.0f *  num_frames / milis   );
		past = current;
		milis = 0;
		num_frames =0;
	}
	return SUCCCESS;
}

OpenGLContext* opengl_context_create( PlatformEGLContext* eglctx )
{
	GLint ret;
	OpenGLContext * ctx = (OpenGLContext*) malloc( sizeof ( OpenGLContext ));
	memset( ctx, 0, sizeof (OpenGLContext));
	ctx->m_egl_context = eglctx;

	GL_CHECK_ERROR( ctx->m_vertex_shader = glCreateShader(GL_VERTEX_SHADER) )
	GL_CHECK_ERROR( glShaderSource(ctx->m_vertex_shader, 1, &vertex_source, NULL) )
	GL_CHECK_ERROR( glCompileShader(ctx->m_vertex_shader) )

	GL_CHECK_ERROR( ctx->m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER) )
	GL_CHECK_ERROR( glShaderSource(ctx->m_fragment_shader, 1, &fragment_source, NULL) )
	GL_CHECK_ERROR( glCompileShader(ctx->m_fragment_shader) );

	GL_CHECK_ERROR( ctx->m_program = glCreateProgram() )
	GL_CHECK_ERROR( glAttachShader(ctx->m_program, ctx->m_vertex_shader) )
	GL_CHECK_ERROR( glAttachShader(ctx->m_program, ctx->m_fragment_shader) )

	GL_CHECK_ERROR( glBindAttribLocation(ctx->m_program, 0, "position") )
	GL_CHECK_ERROR( glBindAttribLocation(ctx->m_program, 1, "color") )

	LINFO("=== Calling glLinkProgram()");
	GL_CHECK_ERROR( glLinkProgram(ctx->m_program) )
	GL_CHECK_ERROR( glGetProgramiv (ctx->m_program, GL_LINK_STATUS, &ret) )
	assert (ret == GL_TRUE);

	GL_CHECK_ERROR( glFlush() )

	LINFO("=== Calling glUseProgram()");
	GL_CHECK_ERROR( glUseProgram(ctx->m_program) )
	GL_CHECK_ERROR( glFlush() )
	LINFO("=== calling glViewport() : %d %d", eglctx->m_width, eglctx->m_height );
	GL_CHECK_ERROR( glViewport(0, 0, eglctx->m_width, eglctx->m_height) )
	GL_CHECK_ERROR( glFlush() )
	GL_CHECK_ERROR( glClearColor( 1.0f, 0.0f, 0.0f, 1.0f ) );
	return ctx;
}

void opengl_context_destroy( OpenGLContext* 	   oglctx )
{
	free(oglctx);
}



PlatformEGLContext* opengl_get_egl_context( OpenGLContext* ctx )
{
	return ctx->m_egl_context;
}

void  opengl_context_attach_user_ctx( OpenGLContext* ctx, void *private_data )
{
	ctx->m_private_data = private_data;
}

void* opengl_context_get_user_ctx( OpenGLContext* ctx )
{
	return ctx->m_private_data;
}

void opengl_wiewport(OpenGLContext* eglctx,
							   unsigned short orig_x,
							   unsigned short orig_y,
							   unsigned short width,
							   unsigned short height)
{
	eglctx->m_egl_context->m_width =  width;
	eglctx->m_egl_context->m_height = height;
	LINFO("=== calling glViewport() %d %d %d %d\n", orig_x, orig_y, width, height);
	GL_CHECK_ERROR( glViewport(orig_x, orig_y, eglctx->m_egl_context->m_width, eglctx->m_egl_context->m_height ) )
	GL_CHECK_ERROR( glFlush() )
}
