/*
 * plugin.h
 *
 *  Created on: Nov 26, 2017
 *      Author: ionut
 */

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include "../platform-lib/opengl-context.h"

typedef int plugin_init_context_func(  OpenGLContext *opengl_ctx, void* plugin_ctx );
typedef int plugin_frame_display_func( OpenGLContext *opengl_ctx,  void* plugin_ctx );
typedef int plugin_destroy_context_func( OpenGLContext *opengl_ctx,  void* plugin_ctx );


/*Very Limited descriptior*/
enum e_property_type
{
	PT_STRING = 0,
	PT_INT
};

typedef enum e_property_type property_type_t;

struct s_plugin_property_descriptor
{
	property_type_t property_type;
	char			*property_name;
	char			property_hints;
};

typedef struct s_plugin_property_descriptor plugin_property_descriptor_t;


struct s_plugin_properties_node
{
	plugin_property_descriptor_t 		 property_descriptor;
	struct _plugin_properties_node*		_next;
};

typedef struct s_plugin_properties_node plugin_properties_node_t;
typedef struct s_plugin_properties_node plugin_properties_list_t;


typedef struct tagOpenGLPluginInfo
{
	char*							plugin_name;
	unsigned  short					plugin_ver_minor;
	unsigned  short					plugin_ver_major;
	plugin_init_context_func*		init_func;
	plugin_frame_display_func* 		frame_display_func;
	plugin_destroy_context_func* 	destroy_func;
	plugin_properties_list_t*		properties;
} OpenGLPluginInfo;

typedef struct tagOpenGLPlugin
{
	OpenGLPluginInfo*		plugin_info;
	void*					plugin_ctx;
} OpenGLPlugin;




#endif /* PLUGIN_H_ */
