/*
 * cairo-sample-plugin.c
 *
 *  Created on: Aug 30, 2017
 *      Author: ionut
 */



#define NUM_TEXTURES 4


typedef struct _CairoSamplePluginContext
{
     int textures[ NUM_TEXTURES ];
     int position_x;
     const char *text;
} CairoSamplePluginContext;


int cairo_sample_plugin_init_context( CairoSamplePluginContext* plugin_ctx )
{

}


int cairo_sample_plugin_frame_display( CairoSamplePluginContext* plugin_ctx )
{

}


int cairo_sample_plugin_destroy( CairoSamplePluginContext* plugin_ctx )
{

}
