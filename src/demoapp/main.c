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
 * OpenGLUserContext - an arbitrary, user-defined data, maintained by user that is passed to opengl as "user context"
 * In our case we have one texture. Texture goes initially in CPU as { data, width, height } then loaded in GPU as id.
 * Once loaded in GPU, data can be discarded from CPU.
 */
typedef struct tagOpenGLUserContext
{
	GLuint		m_texture_id;
	int         m_texture_width;
	int			m_texture_height;
	char*		m_texture_data;
	short		m_unlimited_frames;
	long		m_numframes;
} OpenGLUserContext;

#define USER_CONTEXT( __ctx__ ) ( (OpenGLUserContext*)__ctx__->user_ctx );

#define SUCCESS 	0
#define EGL_ERROR  -1

#define NO_LIMIT    -1

static const char file_name[] = "texture.bmp";
int   reload_textures = 0;
int   leak_textures   = 0;
int   loaded_gpu_texture_data = 0;
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
	int i, rc;

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

	rc = fseek( file, dataPos , SEEK_SET );
	if( rc  )
	{
		LERROR("could not seek file, fatal error");
		exit -1;
	}

	char tmpdata[ user_context->m_texture_width*user_context->m_texture_height*4 ];


    rc = fread( tmpdata, imageSize, 1, file );
    if( rc != 1 )
    {
    	LERROR("could not read the bitmap file, fatal error");
    	exit -1;
    }

	for( i = 0; i < user_context->m_texture_width*user_context->m_texture_height; ++ i )
	{
		user_context->m_texture_data[ 4*i ]   = tmpdata[ 3*i + 2 ];  /*R*/
		user_context->m_texture_data[ 4*i+1 ] = tmpdata[ 3*i + 1 ];  /*G*/
		user_context->m_texture_data[ 4*i+2 ] = tmpdata[ 3*i  ];     /*B*/
		user_context->m_texture_data[ 4*i+3 ] = 0xFF;                /*A*/
	}

	fclose(file);
	return 0;
}


int user_init_context( OpenGLUserContext* user_ctx, int num_frames, int unlimited_frames )
{
	int rc = 0;
	user_ctx->m_numframes = num_frames;
	user_ctx->m_unlimited_frames = unlimited_frames;
	if( ( rc = load_bmp_custom( file_name, user_ctx) ) != 0 )
	{
		fprintf(stderr, "loading resource failed, %s  with code %d\n", file_name, rc );
	}
	return rc;
}

int user_loop_function( OpenGLContext * ctx )
{
	static int scaling_percent = 80;
	static int scaling_increment = 1;
	int rc;

	OpenGLUserContext* user_ctx = ( OpenGLUserContext *) opengl_context_get_user_ctx( ctx );

    if( user_ctx->m_unlimited_frames == 0 )
    {

    	if( user_ctx->m_numframes -- == 0 )
    	{
    		LINFO("reached max number of frames");
    		return -1;
    	}
    }


	if( ! user_ctx->m_texture_id )
	{
		LDEBUG("LOADING TEXTURE [%dx%d]", user_ctx->m_texture_width, user_ctx->m_texture_height );
		rc = opengl_load_texture_in_gpu( user_ctx->m_texture_data, user_ctx->m_texture_width, user_ctx->m_texture_height, &user_ctx->m_texture_id );
		if( rc != GL_NO_ERROR )
		{
			LERROR("could not load texture, GPU memory full ? total texture loaded data: %l", loaded_gpu_texture_data );
			return -2;
		}
		LDEBUG("texture = %u", user_ctx->m_texture_id );
		loaded_gpu_texture_data += 4*user_ctx->m_texture_width*user_ctx->m_texture_height;
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

		    if( reload_textures || leak_textures )
		    {
		    	if( ! leak_textures )
		    	{
		    		opengl_unload_texture_from_gpu( user_ctx->m_texture_id );
		    		loaded_gpu_texture_data -= 4*user_ctx->m_texture_width*user_ctx->m_texture_height;
		    	}
		    	user_ctx->m_texture_id = 0;
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
	int numloops = 0;
	short unlimited_loops = 1;
    //Specifying the expected options
    //The two options l and b expect numbers as argument

	platform_egl_log_init( LL_INFO );

    static struct option long_options[] = {
        {"loops",     						  required_argument, 0,  'l' },  /*!< after setting OpenGL, display a number of loops and exit, default is 0 which means run forever */
        {"reload_texture_on_every_frame",     no_argument, 0,  'r' },        /*!< reload-texture on every frame - need this to assed CPU=>GPU bandwidth */
        {"leak_textures",     				  no_argument, 0,  'k' },        /*!< makes sense with above reload-texture - don't delete previous texture, determine max amount of textures that fit the GPU */
        {"gather_statistics",				  no_argument, 0,  's' },        /*!< print final statistics at the end of the process */
        {"break_textures",					  required_argument, 0, 'b' },  /*!< when displaying, use NxN tiles to display textures */
        {"compress_textures",				  required_argument, 0, 'c' },  /*!< use certain, TODO: tbd compression */
        {"log_level",						  required_argument, 0, 'v' },   /*!< log_level, 'off', 'error', 'warning', 'debug', 'verbose' */
        {0,0,0,0}
    };


    while ((rc = getopt_long(argc, argv,"l:", long_options, &long_index )) != -1)
    {
    	switch (rc) {
    		case 'l' :
    				if( strcmp(optarg, "infinite") == 0 )
    					unlimited_loops = 1;
    				else if( strcmp(optarg, "none") == 0 )
    				{
    					 unlimited_loops = 0;
    					 numloops = 0;
    				}
    				else
    				{
    					numloops = atoi(optarg);
    					unlimited_loops = 0;
    				}
    		break;
    		case 'r':
    			reload_textures = 1;
    			LWARN("cmdline option: reload texture on every, this will affect performance");
    		break;
    		case 'k':
    			leak_textures = 1;
    			LWARN("cmdline option: leak textures, this is only for debugging purposes");
    		break;
    		case 'v':
    			   if( strcmp(optarg,"off") == 0 )
    			   {
    				   platform_egl_log_init( LL_OFF );
    			   }
    			   else if( strcmp(optarg,"verbose") == 0 )
    			   {
    				   platform_egl_log_init( LL_VERBOSE );
    			   }
    			   else if( strcmp(optarg,"debug") == 0 )
    			   {
    				   platform_egl_log_init( LL_DEBUG );
    			   }
    			   else if( strcmp(optarg,"info") == 0 )
    			   {
    				   platform_egl_log_init( LL_INFO );
    			   }
    			   else if( strcmp(optarg,"warning") == 0 )
    			   {
    				   platform_egl_log_init( LL_WARN );
    			   }
    			   else if( strcmp(optarg,"error") == 0 )
    			   {
    				   platform_egl_log_init( LL_ERROR );
    			   }
    			   else
    				   LWARN("could not init log system with level '%s', defaulting to 'info'", optarg);
    		break;

    		default:
    			print_usage();
    			exit(EXIT_FAILURE);
    	}
    }

	if ( numloops < 0 ) {
		LERROR("invalid value for numloops, use 'infinite', 'none', 0 or positive integer value");
		print_usage();
		exit(EXIT_FAILURE);
	}

	LINFO("loops = %d\n", numloops);

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
	user_init_context( &user_ctx , numloops, unlimited_loops);
	opengl_context_attach_user_ctx( oglctx, &user_ctx);


	/**
	 * Run the opengl loop. This will call user_loop_function for each frame
	 * at the full speed of HW.
	 * The user_loop_function must know about user context and treat it
	 * it can do custom draw and animations, load/unload textures, using functions
	 * defined in opengl-context.c
	 */

	opengl_mainloop( oglctx,  (user_loop_function_pf) user_loop_function );

	LINFO("exiting app's main loop");

	opengl_context_destroy(oglctx);

	platform_egl_context_deinit( eglctx );
	platform_egl_context_destroy( eglctx );

	return SUCCESS;
}
