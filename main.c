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
#include <getopt.h>
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
	int			m_numframes;
} OpenGLUserContext;

#define USER_CONTEXT( __ctx__ ) ( (OpenGLUserContext*)__ctx__->user_ctx );

#define SUCCESS 	0
#define EGL_ERROR  -1

#define NO_LIMIT    0

static const char file_name[] = "texture.bmp";

/**
 * load_bmp_custom loads the bmp file into RGBA compatible texture data
 * that can be used later as parameter for opengl_load_texture_in_gpu() function
 * @param imagepath - the (relative) path of the file. This path is relative to the working dir.
 * @param user_context - user context in which texture is loaded
 * @return 0 for success, or non-zero error code
 */
int load_bmp_custom(const char * imagepath, OpenGLUserContext * user_context )
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
		printf("Not a correct BMP file, too short\n\r");
		return -2;
	}

	if( ( header[0] != 'B' ) || ( header[1] != 'M' ) )
	{
		printf("Not a correct BMP file\n\r");
		return -3;
	}

	dataPos   =  *(int*)&(header[0x0A]);
	user_context->m_texture_width     =  *(int*)&(header[0x12]);
	user_context->m_texture_height    =  *(int*)&(header[0x16]);
	imageSize =  *(int*)&(header[0x22]);

	user_context->m_texture_data = (char*)malloc(  user_context->m_texture_width*user_context->m_texture_height*4 );
	memset( user_context->m_texture_data, 0xff,  user_context->m_texture_width*user_context->m_texture_height*3 );

	printf("found bitmap in file, width=%d, height=%d, image_size=%d\n\r", user_context->m_texture_width, user_context->m_texture_height, imageSize );
	if ( imageSize == 0 ) imageSize = user_context->m_texture_width*user_context->m_texture_height*3;
	if( imageSize != user_context->m_texture_width*user_context->m_texture_height*3 )
			return -5;

	if ( dataPos == 0 )
		dataPos = 54;

	fseek( file, dataPos , SEEK_SET );

	char tmpdata[ user_context->m_texture_width*user_context->m_texture_height*4 ];


	fread( tmpdata,1,imageSize,file );

	for( i = 0; i < user_context->m_texture_width*user_context->m_texture_height; ++ i )
	{

		user_context->m_texture_data[ 4*i ]   = tmpdata[ 3*i + 2 ];
		user_context->m_texture_data[ 4*i+1 ] = tmpdata[ 3*i + 1 ];
		user_context->m_texture_data[ 4*i+2 ] = tmpdata[ 3*i  ];
		user_context->m_texture_data[ 4*i+3 ] = 0xFF;
	}

	fclose(file);
	return 0;
}


int user_init_context( OpenGLUserContext* user_ctx, int num_frames )
{
	int rc = 0;
	user_ctx->m_numframes = num_frames;
	if( ( rc = load_bmp_custom( file_name, user_ctx) ) != 0 )
	{
		fprintf(stderr, "loading resource failed, %s  with code %d\n", file_name, rc );
		return rc;
	}

}

int user_loop_function( OpenGLContext * ctx )
{
	static int scaling_percent = 80;
	static int scaling_increment = 1;

	OpenGLUserContext* user_ctx = ( OpenGLUserContext *) opengl_context_get_user_ctx( ctx );
	if( ! user_ctx->m_texture_id )
	{
		LDEBUG("LOADING TEXTURE %dx%d", user_ctx->m_texture_width, user_ctx->m_texture_height );
		user_ctx->m_texture_id = opengl_load_texture_in_gpu( user_ctx->m_texture_data, user_ctx->m_texture_width, user_ctx->m_texture_height );
		LDEBUG("texture = %u", user_ctx->m_texture_id );
	}

	if( user_ctx->m_texture_id )
	{

			scaling_percent = scaling_percent + scaling_increment;
			if( scaling_percent > 120 )
				scaling_increment = -1;
			else
				if( scaling_percent < 80 )
					scaling_increment = 1;

		    opengl_draw_texture( ctx, user_ctx->m_texture_id, scaling_percent*0.01f );
		    if( user_ctx->m_numframes != 0 )
		    {
		    	-- user_ctx->m_numframes;
		    	if( user_ctx->m_numframes == 0 )
		    	{
		    		LINFO("reached max number of frames");
		    		return -1;
		    	}

		    }

	}
	else
	{
			LWARN( "no texture");
	}

	return 0;
}

void print_usage()
{

}

int main(int argc, char *argv[])
{
	int rc;
    int long_index = 0;
	int numloops = NO_LIMIT;
    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"loops",     required_argument, 0,  'l' },
        {0,           0,                 0,  0   }
    };


    while ((rc = getopt_long(argc, argv,"l:", long_options, &long_index )) != -1)
    {
    	switch (rc) {
    		case 'l' :
    				if( strcmp(optarg, "infinite") == 0 )
    					numloops = NO_LIMIT;
    				else
    					numloops = atoi(optarg);
    		break;

    		default:
    			print_usage();
    			exit(EXIT_FAILURE);
    	}
    }

	if (numloops < 0 ) {
		print_usage();
		exit(EXIT_FAILURE);
	}
	printf("loops = %d\n", numloops);

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
	/**
	 * Creating and attaching our own user context.
	 * This will contain different information to draw, like textures.
	 */
	user_init_context( &user_ctx , numloops);
	opengl_context_attach_user_ctx( oglctx, &user_ctx);


	/**
	 * Run the opengl loop. This will call user_loop_function for each frame
	 * at the full speed of HW.
	 * The user_loop_function must know about user context and treat it
	 * it can do custom draw and animations, load/unload textures, using functions
	 * defined in opengl-context.c
	 */

	opengl_mainloop( oglctx,  (user_loop_function_pf) user_loop_function );

	LINFO("exiting opengl loop");

	opengl_context_destroy(oglctx);

	platform_egl_context_deinit( eglctx );
	platform_egl_context_destroy( eglctx );
	return SUCCESS;
}
