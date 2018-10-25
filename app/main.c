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
#include <assert.h>
#include "platform-egl-log.h"

#include "platform-egl-context.h"
#include "opengl-context.h"
#include "opengl-user-context-helper.h"
#include "opengl-plugin-params-helper.h"

/* For the moment we stick to GLES2.0 */

#include <GLES2/gl2.h>


#define EGL_ERROR -1
#define SUCCESS    0

/**
 * load_bmp_custom loads the bmp file into RGBA compatible texture data
 * that can be used later as parameter for opengl_load_texture_in_gpu() function
 * @param imagepath - the (relative) path of the file. This path is relative to the working dir.
 * @param user_context - user context in which texture is loaded
 * @return 0 for success, or non-zero error code
 */

#if 0
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
#endif

int opengl_user_context_init( OpenGLUserContext* user_ctx )
{
	user_ctx->plugins_head = NULL;
	user_ctx->last_plugin_id = 0;

	PluginInitializerFieldListNode *static_picture_params = NULL;
	PluginInitializerFieldListNode *static_scrolltest_params = NULL;

	opengl_plugin_registry_init(NULL);

	/* creating static picture */
	static_picture_params = opengl_plugin_initializer_field_list_add_string(static_picture_params, "background", "image.gif" );
	OpenGLPlugin *background_plugin =  opengl_plugin_registry_create_plugin_by_name("static.picture", static_picture_params );
	assert( background_plugin != NULL );
	opengl_user_ctx_add_plugin( user_ctx, background_plugin );

	static_scrolltest_params =  opengl_plugin_initializer_field_list_add_string(static_scrolltest_params, "text", "This is sample text. For mode, please add here some text" );
	OpenGLPlugin *text_scroll_plugin =  opengl_plugin_registry_create_plugin_by_name("textscrool.horizontal", static_scrolltest_params);
	assert( text_scroll_plugin != NULL );

	opengl_user_ctx_add_plugin( user_ctx, text_scroll_plugin );


	// at the end:
	opengl_plugin_init(background_plugin);
	opengl_plugin_init(text_scroll_plugin);
	opengl_plugin_initializer_field_list_destroy( static_picture_params );
	opengl_plugin_initializer_field_list_destroy( static_scrolltest_params );
}


int user_loop_function( OpenGLContext * ctx )
{
	int rc;
	OpenGLUserContext* user_ctx = ( OpenGLUserContext *) opengl_context_get_user_ctx( ctx );
	OpenGLPluginListNode* node = user_ctx->plugins_head;
	for( ; node != NULL; node = node->next )
	{
			if( node && node->plugin )
			{
				rc = node->plugin->frame_display_func( node->plugin->plugin_ctx );
				if( rc )
				{
					/* TODO: tear-down this plugin/node */
				}
			}
	}
	return 0;
}


int main(int argc, char *argv[])
{
	int rc;
	platform_egl_log_init( LL_INFO );

	PlatformEGLContext* eglctx = platform_egl_context_create();

	OpenGLContext       *opengl_context = NULL;
	OpenGLUserContext	 opengl_user_ctx = {0};


	if( (rc = platform_egl_context_init( eglctx )) )
	{
		fprintf( stderr, "platform_egl_context_init failed with code %d\n", rc );
		return EGL_ERROR;
	}
	else
	{
		fprintf( stdout, "platform_egl_context_init succeeded\n" );
	}

	opengl_context = opengl_context_create( eglctx );

	/**
	 * Creating and attaching our own user context.
	 * This will contain different information to draw, like textures.
	 */
	opengl_user_context_init( & opengl_user_ctx );
	opengl_context_attach_user_ctx( opengl_context, &opengl_user_ctx);


	/**
	 * Run the opengl loop. This will call user_loop_function for each frame
	 * at the full speed of HW.
	 * The user_loop_function must know about user context and treat it
	 * it can do custom draw and animations, load/unload textures, using functions
	 * defined in opengl-context.c
	 */

	opengl_mainloop( opengl_context,  (user_loop_function_pf) user_loop_function );

	LINFO("exiting app's main loop");

	opengl_context_destroy(opengl_context);

	platform_egl_context_deinit( eglctx );
	platform_egl_context_destroy( eglctx );

	return SUCCESS;
}