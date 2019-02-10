/*
 * static-picture.c
 *
 *  Created on: Nov 25, 2017
 *      Author: ionut
 */


#include "plugin.h"
#include "opengl-plugin-params-helper.h"
#include "../platform-lib/platform-egl-log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_TEXTURES 1


typedef struct _StaticImagePluginContext
{
     int 	m_bkg_texture_id;
     int 	m_position_x;
     int 	m_position_y;
     float  m_zoom_x;
     float  m_zoom_y;
     const char *m_file_name;
     size_t m_texture_width;
     size_t m_texture_height;

} StaticImagePluginContext;



int opengl_plugin_static_picture_load_bmp_custom(const char * imagepath, OpenGLContext *opengl_context, StaticImagePluginContext * plugin_ctx )
{
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	int i, rc;
	char  *texture_data;
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

	plugin_ctx->m_texture_width     =  *(int*)&(header[0x12]);
	plugin_ctx->m_texture_height    =  *(int*)&(header[0x16]);

	imageSize =  *(int*)&(header[0x22]);

	texture_data = (char*)malloc(  plugin_ctx->m_texture_width*plugin_ctx->m_texture_height*4 );
	memset( texture_data, 0xff,  plugin_ctx->m_texture_width*plugin_ctx->m_texture_height*3 );

	printf("found bitmap in file, width=%u, height=%u, image_size=%d\n\r", (unsigned int)plugin_ctx->m_texture_width,  (unsigned int)plugin_ctx->m_texture_height, imageSize );
	if ( imageSize == 0 ) imageSize = plugin_ctx->m_texture_width*plugin_ctx->m_texture_height*3;
	if( imageSize != plugin_ctx->m_texture_width*plugin_ctx->m_texture_height*3 )
			return -5;

	if ( dataPos == 0 )
		dataPos = 54;

	rc = fseek( file, dataPos , SEEK_SET );
	if( rc  )
	{
		LERROR("could not seek file, fatal error");
		exit -1;
	}

	char tmpdata[ plugin_ctx->m_texture_width*plugin_ctx->m_texture_height*4 ];


    rc = fread( tmpdata, imageSize, 1, file );
    if( rc != 1 )
    {
    	LERROR("could not read the bitmap file, fatal error");
    	exit -1;
    }

	for( i = 0; i < plugin_ctx->m_texture_width*plugin_ctx->m_texture_height; ++ i )
	{
		texture_data[ 4*i ]   = tmpdata[ 3*i + 2 ];  /*R*/
		texture_data[ 4*i+1 ] = tmpdata[ 3*i + 1 ];  /*G*/
		texture_data[ 4*i+2 ] = tmpdata[ 3*i  ];     /*B*/
		texture_data[ 4*i+3 ] = 0xFF;                /*A*/
	}
	fclose(file);
	opengl_load_texture_in_gpu(  texture_data,  plugin_ctx->m_texture_width, plugin_ctx->m_texture_height, &plugin_ctx->m_bkg_texture_id );
	return 0;
}



int opengl_plugin_static_picture_init_context( OpenGLContext *opengl_context, StaticImagePluginContext* plugin_ctx )
{
	return opengl_plugin_static_picture_load_bmp_custom( "./texture_big.bmp", opengl_context, plugin_ctx );
}


int opengl_plugin_static_picture_display( OpenGLContext *opengl_context, StaticImagePluginContext* plugin_ctx )
{
	if( plugin_ctx->m_bkg_texture_id > 0)
		opengl_draw_texture( opengl_context, plugin_ctx->m_bkg_texture_id, 1 );
}


int opengl_plugin_static_picture_destroy( OpenGLContext *opengl_context, StaticImagePluginContext* plugin_ctx )
{
	if( plugin_ctx->m_bkg_texture_id > 0)
		opengl_unload_texture_from_gpu( plugin_ctx->m_bkg_texture_id );
}



OpenGLPluginInfo* opengl_plugin_static_picture_get_plugin_info()
{
	static OpenGLPluginInfo static_picture_info;
	static_picture_info.plugin_name = "static.picture";
	static_picture_info.plugin_ver_major = 1;
	static_picture_info.plugin_ver_minor = 0;
	static_picture_info.init_func          = opengl_plugin_static_picture_init_context;
	static_picture_info.frame_display_func = opengl_plugin_static_picture_display;
	static_picture_info.destroy_func       = opengl_plugin_static_picture_destroy;
/*
	ADD_PLUGIN_PROPERTY( static_picture_info,
						 "image",
						  SYSTEM_PICTURE,
						  SYSTEM_HINT_LARGE_IMAGES
						)

	ADD_PLUGIN_PROPERTY( static_picture_info,
						 "position_x",
						 BASIC_INTEGER,
						 RANGE( -1080, 1080 )
						)

	ADD_PLUGIN_PROPERTY( static_picture_info,
						 "zoom_x",
						 BASIC_FLOAT,
						 RANGE( 1.0, 100.0 )
						)

*/
	return &static_picture_info;
}

/*
MutableObject*  opengl_plugin_static_picture_get_property( const char* propname  )
{

}

int  opengl_plugin_static_picture_set_property( const char* propname, MutableObject* value  )
{

}
*
*/

OpenGLPlugin* opengl_plugin_static_picture_new( PluginInitializerFieldListNode *field_list )
{
	OpenGLPlugin* plugin = (OpenGLPlugin*) malloc ( sizeof( OpenGLPlugin ));
	plugin->plugin_info  = opengl_plugin_static_picture_get_plugin_info();
	plugin->plugin_ctx   = ( StaticImagePluginContext *) malloc( sizeof (StaticImagePluginContext));
	memset( plugin->plugin_ctx, 0, sizeof(StaticImagePluginContext) );
	return plugin;
}


OpenGLPlugin* opengl_plugin_static_picture_try_init( PluginInitializerFieldListNode *field_list )
{
	return opengl_plugin_static_picture_new( field_list );
}



