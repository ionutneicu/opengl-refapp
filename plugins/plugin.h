/*
 * plugin.h
 *
 *  Created on: Nov 26, 2017
 *      Author: ionut
 */

#ifndef PLUGIN_H_
#define PLUGIN_H_


typedef int plugin_init_context_func( void* plugin_ctx );
typedef int plugin_frame_display_func( void* plugin_ctx );
typedef int plugin_destroy_context_func( void* plugin_ctx );


typedef struct tagOpenGLPluginInfo
{
	char*				plugin_name;
	unsigned  short			plugin_ver_minor;
	unsigned  short			plugin_ver_major;
	plugin_init_context_func*	init_func;
	plugin_frame_display_func* 	frame_display_func;
	plugin_destroy_context_func* 	destroy_func;
} OpenGLPluginInfo;

typedef struct tagOpenGLPlugin
{
	OpenGLPluginInfo		plugin_info;
	void*				plugin_ctx;
} OpenGLPlugin;




#endif /* PLUGIN_H_ */
