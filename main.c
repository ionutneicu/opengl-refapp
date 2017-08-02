/*
 * Copyright (c) 2015-2017 Ionut Neicu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "platform-egl-log.h"

#include "platform-egl-context.h"
#include "opengl-context.h"

/* For the moment we stick to GLES2.0 */

#include <GLES2/gl2.h>

/**
 * OpenGLUserContext - an arbitrary data, maintained by user that is passed to opengl as "user context"
 * Here we have one texture. Texture goes initially in CPU as { data, width, height } then loaded in GPU as id.
 */
typedef struct tagOpenGLUserContext
{
	GLuint		m_texture_id;
	int         m_texture_width;
	int			m_texture_height;
	char*		m_texture_data;
} OpenGLUserContext;

#define USER_CONTEXT( __ctx__ ) ( (OpenGLUserContext*)__ctx__->user_ctx );

#define SUCCESS 	0
#define EGL_ERROR  -1


int loadBMP_custom(const char * imagepath, OpenGLUserContext * context )
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
	context->m_texture_width     =  *(int*)&(header[0x12]);
	context->m_texture_height    =  *(int*)&(header[0x16]);
	imageSize =  *(int*)&(header[0x22]);

	context->m_texture_data = (char*)malloc(  context->m_texture_width*context->m_texture_height*4 );
	memset( context->m_texture_data, 0xff,  context->m_texture_width*context->m_texture_height*3 );

	printf("found bitmap in file, width=%d, height=%d, image_size=%d\n\r", context->m_texture_width, context->m_texture_height, imageSize );
	if ( imageSize == 0 ) imageSize = context->m_texture_width*context->m_texture_height*3;
	if( imageSize != context->m_texture_width*context->m_texture_height*3 )
			return -5;

	if ( dataPos == 0 )
		dataPos = 54;

	fseek( file, dataPos , SEEK_SET );

	char tmpdata[ context->m_texture_width*context->m_texture_height*4 ];


	fread( tmpdata,1,imageSize,file );

	for( i = 0; i < context->m_texture_width*context->m_texture_height; ++ i )
	{

		context->m_texture_data[ 4*i ]   = tmpdata[ 3*i + 2 ];
		context->m_texture_data[ 4*i+1 ] = tmpdata[ 3*i + 1 ];
		context->m_texture_data[ 4*i+2 ] = tmpdata[ 3*i  ];
		context->m_texture_data[ 4*i+3 ] = 0xFF;

#if 0
		context->m_texture_data[ 4*i ]   = tmpdata[ 3*i + 1 ];
		context->m_texture_data[ 4*i+1 ] = tmpdata[ 3*i ];
		context->m_texture_data[ 4*i+2 ] = tmpdata[ 3*i+2 ];
		context->m_texture_data[ 4*i+3 ] = 0xFF;

		context->m_texture_data[ 4*i ]   = 0xFF;
		context->m_texture_data[ 4*i+1 ] = 0x00;
		context->m_texture_data[ 4*i+2 ] = 0xFF;
		context->m_texture_data[ 4*i+3 ] = 0xFF;
#endif
	}

	fclose(file);
	return 0;
}

int user_loop_function( OpenGLContext * ctx )
{
	static int i = 80;
	static int direction = 1;

	OpenGLUserContext* user_ctx = ( OpenGLUserContext *) opengl_context_get_user_ctx( ctx );
	if( ! user_ctx->m_texture_id )
	{
		LDEBUG("LOADING TEXTURE %dx%d", user_ctx->m_texture_width, user_ctx->m_texture_height );
		user_ctx->m_texture_id = opengl_load_texture_in_gpu( user_ctx->m_texture_data, user_ctx->m_texture_width, user_ctx->m_texture_height );
		LDEBUG("texture = %u", user_ctx->m_texture_id );
	}

	if( user_ctx->m_texture_id )
	{

			i = i + direction;
			if( i > 120 )
				direction = -1;
			else if( i < 80 )
				direction = 1;

		    opengl_draw_texture( ctx, user_ctx->m_texture_id, i*0.01f );

	}
	else
	{
			LWARN( "no texture");
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int rc;

	PlatformEGLContext* eglctx = platform_egl_context_create();
	OpenGLContext *oglctx;
	OpenGLUserContext	user_ctx;
	memset( &user_ctx, 0, sizeof ( OpenGLUserContext ));

	if( (rc = platform_egl_context_init( eglctx )) )
	{
		fprintf( stderr, "platform_egl_context_init failed with code %d\n", rc );
		return EGL_ERROR;
	}
	else
	{
		fprintf( stdout, "platform_egl_context_init succeeded\n" );
	}

	oglctx = opengl_context_create( eglctx );
	opengl_context_attach_user_ctx( oglctx, &user_ctx);

	loadBMP_custom( "texture.bmp", &user_ctx);

	opengl_mainloop( oglctx,  (user_loop_function_pf) user_loop_function );
	platform_egl_context_deinit( eglctx );
	platform_egl_context_destroy( eglctx );
	return SUCCESS;
}
