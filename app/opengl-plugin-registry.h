/*
 * opengl_plugin_registry.h
 *
 *  Created on: Sep 23, 2017
 *      Author: ionut
 */

#ifndef OPENGL_PLUGIN_REGISTRY_H_
#define OPENGL_PLUGIN_REGISTRY_H_


#include "opengl-user-context-helper.h"
#include "opengl-plugin-params-helper.h"


int opengl_plugin_registry_init( const char *plugin_path);
OpenGLPlugin * opengl_plugin_registry_create_plugin_by_name( const char *plugin_name, PluginInitializerFieldListNode *field_list);



#endif /* OPENGL_PLUGIN_REGISTRY_H_ */
