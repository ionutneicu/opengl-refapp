/*
 * opengl-plugin-params-helper.h
 *
 *  Created on: Oct 2, 2017
 *      Author: ionut
 *  Naive implementation of plugin parameters.
 *  This object is critical and deserves complete library for doing this.
 */

#ifndef OPENGL_PLUGIN_PARAMS_HELPER_H_
#define OPENGL_PLUGIN_PARAMS_HELPER_H_

typedef enum { PIFT_INTEGER, PIFT_FLOAT, PIFT_STRING } PluginInitializerFieldTye;

typedef struct _PluginInitializerField
{
	PluginInitializerFieldTye type;
	union
	{
		int 	 int_value;
		float    float_value;
		char*	 string_value;
	} value;
} PluginInitializerField;

typedef struct _PluginInitializerFieldListNode
{
	char* 									field_name;
	PluginInitializerField  				field_value;
	struct _PluginInitializerFieldListNode*	_next;
}PluginInitializerFieldListNode;

/**
 * opengl_plugin_initializer_field_list_new - creates and initializes ( to empty ) new plugin initializer list;
 * @return new list;
 */
PluginInitializerFieldListNode* opengl_plugin_initializer_field_list_node_new();

/**
 * opengl_plugin_initializer_field_list_node_destroy destroys all subsequent fields allocated in the node
 * @param node to be destroyed
 */
void opengl_plugin_initializer_field_list_node_destroy(PluginInitializerFieldListNode* node);

/**
 * opengl_plugin_initializer_field_list_add_integer - adds PluginInitializerFieldListNode of type integer in the list;
 * @param list - list where new param to be added to
 * @param path - path in list where to add the new param
 * @param value - value to be added as node
 * @return new list of plugins to be used from now on
 */
PluginInitializerFieldListNode * opengl_plugin_initializer_field_list_add_integer( PluginInitializerFieldListNode *list, const char *path, int value );

/**
 * opengl_plugin_initializer_field_list_add_integer - adds PluginInitializerFieldListNode of type float in the list;
 * @param list - list where new param to be added to
 * @param path - path in list where to add the new param
 * @param value - value to be added as node
 * @return new list of plugins to be used from now on
 */
PluginInitializerFieldListNode * opengl_plugin_initializer_field_list_add_float( PluginInitializerFieldListNode *list, const char *path, float value );

/**
 * opengl_plugin_initializer_field_list_add_string - adds PluginInitializerFieldListNode of type string in the list;
 * @param list - list where new param to be added to
 * @param path - path in list where to add the new param
 * @param value - value to be added as node
 * @return new list of plugins to be used from now on
 */
PluginInitializerFieldListNode * opengl_plugin_initializer_field_list_add_string( PluginInitializerFieldListNode *list, const char *path, const char* value );
/**
 * opengl_plugin_initializer_field_list_destroy - destroys a list and its all nodes
 * @param list
 */
int opengl_plugin_initializer_field_list_destroy(PluginInitializerFieldListNode *list);

PluginInitializerFieldListNode * opengl_plugin_initializer_field_list_find_by_path( PluginInitializerFieldListNode *list, const char* path);

#endif /* OPENGL_PLUGIN_PARAMS_HELPER_H_ */
