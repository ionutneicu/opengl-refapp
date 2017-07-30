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



#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define GL_CHECK_ERROR( __x__ ) __x__ ; if ((error=glGetError()) != GL_NO_ERROR) { LERROR("-----> %s %s: ERROR %d at line %d", #__x__, __FUNCTION__ , error,  __LINE__ ); }
int error;

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


static char* data;
unsigned int width, height;

int loadBMP_custom(const char * imagepath)
{
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	int i;

	FILE * file = fopen(imagepath,"rb");
	if (!file)
	{
		printf("Image could not be opened 1\n\r");
		return -1;
	}

	if( fread(header, 1, 54, file ) != 54 )
	{
		printf("Not a correct BMP file\n\r");
		return -2;
	}

	if( ( header[0] != 'B' ) || ( header[1] != 'M' ) )
	{
		printf("Not a correct BMP file\n\r");
		return -3;
	}

	dataPos   =  *(int*)&(header[0x0A]);
	width     =  *(int*)&(header[0x12]);
	height    =  *(int*)&(header[0x16]);
	imageSize =  *(int*)&(header[0x22]);

	data = (char*)malloc(  width*height*4 );
	memset( data, 0,  width*height*3 );

	printf("found bitmap in file, width=%d, height=%d, image_size=%d\n\r", width, height, imageSize );
	if ( imageSize == 0 ) imageSize = width*height*3;
	if( imageSize != width*height*3 )
			return -5;

	if ( dataPos == 0 ) dataPos = 54;

	char tmpdata[ width*height*4 ];


	fread( tmpdata,1,imageSize,file );

	for( i = 0; i < width*height; ++ i )
	{
		data[ 4*i ]   = tmpdata[ 3*i + 1 ];
		data[ 4*i+1 ] = tmpdata[ 3*i ];
		data[ 4*i+2 ] = tmpdata[ 3*i+2 ];
		data[ 4*i+3 ] = 0xFF;
	}

	fclose(file);
	return 0;

}

static GLuint opengl_load_texture_in_gpu( void* texture_data, unsigned int tex_width, unsigned int tex_height )
{
	GLuint textureID = 1;
	GL_CHECK_ERROR( glGenTextures(1, &textureID) )
	GL_CHECK_ERROR( glBindTexture(GL_TEXTURE_2D, textureID) )
	GL_CHECK_ERROR( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data) )
	GL_CHECK_ERROR( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST) );
	GL_CHECK_ERROR( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST) );
	GL_CHECK_ERROR( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
	GL_CHECK_ERROR( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
	return textureID;
}

static void opengl_unload_texture_from_gpu( GLuint texture )
{
      GL_CHECK_ERROR( glDeleteTextures(1, &texture ))
}


void opengl_draw_texture( GLuint texture,  GLuint program, const float scale_factor )
{

			static const GLfloat texices[] = { 0, 1,
			                            0, 0,
			                            1, 0,
			                            1, 1 };


			GLfloat vertices[] = { -1, -1, 0,  // bottom left corner
			                             -1,  1, 0,  // top left corner
			                              1,  1, 0,  // top right corner
			                              1, -1, 0}; // bottom right corner

			static const GLubyte indices[] = { 0, 2, 1,     // first triangle (bottom left - top left - top right)
											   0, 3, 2 };


			int i;

			for( i = 0; i < sizeof( vertices ) /  sizeof( GLfloat ); ++ i )
				vertices[i] = vertices[i]*scale_factor;

			glUseProgram ( program  );
			// Load the vertex position
			GLint positionLoc = glGetAttribLocation ( program , "a_position" );
			glVertexAttribPointer ( positionLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), vertices );
			// Load the texture coordinate
			GLint texCoordLoc = glGetAttribLocation ( program , "a_texCoord");
			glVertexAttribPointer ( texCoordLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), texices);
			glEnableVertexAttribArray ( positionLoc );
			glEnableVertexAttribArray ( texCoordLoc );

			glActiveTexture ( GL_TEXTURE0 );
			glBindTexture (GL_TEXTURE_2D, texture);

			GLint tex = glGetUniformLocation ( program , "tex");
			glUniform1i ( tex, 0 );
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

			glDisableVertexAttribArray( texCoordLoc );
			glDisableVertexAttribArray( texCoordLoc );

}

static struct timespec past = { 0 };


void platform_opengl_draw(PlatformOpenGLContext* opengl_ctx)
{

	struct timespec current;
	static int i = 80;
	static int direction = 1;
	static int averaging_frame = 0;

	clock_gettime( CLOCK_REALTIME, &past );
	//LINFO("=== calling glClear()");

	GL_CHECK_ERROR( glClear(GL_COLOR_BUFFER_BIT) )
//	GL_CHECK_ERROR( glFlush() )

	if( ! opengl_ctx->m_texture )
	{
		opengl_ctx->m_texture = opengl_load_texture_in_gpu( data, width, height );
		LDEBUG("texture = %u", opengl_ctx->m_texture  );
	}
	if( opengl_ctx->m_texture)
	{
		
		i = i + direction;
		if( i > 120 )
			direction = -1;
		else
			if( i < 80 )
				direction = 1;

	    opengl_draw_texture( opengl_ctx->m_texture, opengl_ctx->m_program, i*0.01f );

	}
	else
	{
		LWARN( "no texture");
	}
	//LINFO("=== calling eglSwapBuffers()");
	platform_egl_context_swap_buffers(opengl_ctx->m_egl_context);
	//GL_CHECK_ERROR( glFinish() )
	//LINFO("=== done");
	//usleep(10000);
	if( ++ averaging_frame == 100 )
	{
		clock_gettime(CLOCK_REALTIME, &current);
		long milis = MILIS( current )- MILIS(past);
		LINFO("FPS -----> %d",  (int)( 1000.0f *  averaging_frame / milis / 100  ) );
		past = current;
		averaging_frame = 0;

		opengl_unload_texture_from_gpu(opengl_ctx->m_texture);
		opengl_ctx->m_texture = 0;
	}
}

PlatformOpenGLContext* platform_opengl_context_create( PlatformEGLContext* eglctx )
{
	GLint ret;
	PlatformOpenGLContext * ctx = (PlatformOpenGLContext*) malloc( sizeof ( PlatformOpenGLContext ));
	memset( ctx, 0, sizeof (PlatformOpenGLContext));
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


void platform_opengl_init(PlatformOpenGLContext* eglctx)
{
	loadBMP_custom("./texture.bmp");
}

void platform_opengl_wiewport(PlatformOpenGLContext* eglctx,
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
