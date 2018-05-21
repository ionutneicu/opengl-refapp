/*
 * cairo-sample-plugin.c
 *
 *  Created on: Aug 30, 2017
 *      Author: ionut
 */
#include "opengl-plugin.h"


#define NUM_TEXTURES 4


typedef struct _CairoSamplePluginContext
{
     int textures[ NUM_TEXTURES ];
     const char *text;
     int position_x;
     int position_y;
     float zoom_x;
     float zoom_y;
} CairoSamplePluginContext;





int opengl_plugin_static_picture_init_context( CairoSamplePluginContext* plugin_ctx )
{

}


int opengl_plugin_static_picture_display( CairoSamplePluginContext* plugin_ctx )
{

}


int opengl_plugin_static_picture_destroy( CairoSamplePluginContext* plugin_ctx )
{

}


OpenGLPlugin* opengl_plugin_static_picture_new( PluginInitializerFieldListNode *field_list )
{
	OpenGLPlugin* plugin = (OpenGLPlugin*) malloc ( sizeof( OpenGLPlugin ));
	plugin->plugin_info.plugin_name = "static.picture";
	plugin->plugin_info.plugin_ver_major = 1;
	plugin->plugin_info.plugin_ver_minor = 0;
	plugin->plugin_info.init_func = opengl_plugin_static_picture_init_context;
	plugin->plugin_info.frame_display_func = opengl_plugin_static_picture_display;
	plugin->plugin_info.destroy_func =  opengl_plugin_static_picture_destroy;
	plugin->plugin_ctx = ( CairoSamplePluginContext *) malloc( sizeof (CairoSamplePluginContext));
	memset( plugin->plugin_ctx, 0, sizeof(CairoSamplePluginContext) );
	return plugin;
}
