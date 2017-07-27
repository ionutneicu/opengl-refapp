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
#include <time.h>

#define GL_CHECK_ERROR( __x__ ) __x__ ; if ((error=glGetError()) != GL_NO_ERROR) { LERROR("-----> %s %s: ERROR %d at line %d\n", #__x__, __FUNCTION__ , error,  __LINE__ ); }
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
		  "  gl_FragColor = texture2D( tex, v_texCoord );	   \n"
		  "}												   \n";


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

	dataPos   = *(int*)&(header[0x0A]);
	width   = *(int*)&(header[0x12]);
	height   = *(int*)&(header[0x16]);
	imageSize = *(int*)&(header[0x22]);

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
		data[ 4*i+3 ] = 0xffu;
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

void platform_opengl_draw(PlatformEGLContext* eglctx)
{
	static int need_init = 1;
	static GLuint vertex, fragment, program;
	static GLuint texture = 0;
	struct timespec current;
	static int i = 80;
	static int direction = 1;
	static int averaging_frame = 0;

	if( ! need_init )
		goto partial_draw;

	clock_gettime( CLOCK_REALTIME, &past );

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

	GL_CHECK_ERROR( glClearColor(0.0f, 0.0f, 0.0f, 0.0f) )
	GL_CHECK_ERROR( glFlush() )


	loadBMP_custom("./texture.bmp");

        need_init = 0;

partial_draw:
	LINFO("=== calling glClear()\n");

	GL_CHECK_ERROR( glClear(GL_COLOR_BUFFER_BIT) )
	GL_CHECK_ERROR( glFlush() )

	if( ! texture )
	{
		texture = opengl_load_texture_in_gpu( data, width, height );
		LDEBUG("texture = %u", texture );
	}
	if( texture )
	{
		
		i = i + direction;
		if( i > 120 )
			direction = -1;
		else
			if( i < 80 )
				direction = 1;

		opengl_draw_texture( texture, program, i*0.01f );
		//opengl_unload_texture_from_gpu(texture);
		//texture = 0;
	}
	else
	{
		LWARN( "no texture");
	}

	LINFO("=== calling eglSwapBuffers()\n");
	
	platform_egl_context_swap_buffers(eglctx);
	GL_CHECK_ERROR( glFinish() )
	LINFO("=== done\n");
	if( ++averaging_frame == 60 )
	{
		clock_gettime(CLOCK_REALTIME, &current);
		long milis = MILIS( current )- MILIS(past);
		LINFO("FPS -----> %d",  1000*averaging_frame / milis  );
		past = current;
		averaging_frame = 0;
	}
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
