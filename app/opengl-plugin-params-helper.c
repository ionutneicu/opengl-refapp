/*
 * opengl-plugin-params-helper.c
 *
 *  Created on: Oct 2, 2017
 *      Author: ionut
 */

#ifndef OPENGL_PLUGIN_PARAMS_HELPER_C_
#define OPENGL_PLUGIN_PARAMS_HELPER_C_
#include "opengl-plugin-params-helper.h"
#include <stdlib.h>
#include <string.h>


PluginInitializerFieldListNode* opengl_plugin_initializer_field_list_node_new()
{
	PluginInitializerFieldListNode* node = ( PluginInitializerFieldListNode* ) malloc ( sizeof (PluginInitializerFieldListNode ) );
}

void opengl_plugin_initializer_field_list_node_destroy(PluginInitializerFieldListNode* node)
{
	if( node )
	{
		free( node->field_name );
		switch( node->field_value.type )
		{
			case  PIFT_STRING:
				if( node->field_value.value.string_value )
					free( node->field_value.value.string_value );
			break;
			default:
				break;
		}
		free( node );
	}

}

PluginInitializerFieldListNode * opengl_plugin_initializer_field_list_find_by_path( PluginInitializerFieldListNode *list, const char* path)
{
	PluginInitializerFieldListNode *node;
	for( node = list; node; node = node->_next )
	{
		if( strcmp( node->field_name, path ) == 0 )
			return node;
	}
	return NULL;
}

static PluginInitializerFieldListNode * opengl_plugin_initializer_field_list_add_node( PluginInitializerFieldListNode *list, PluginInitializerFieldListNode *node )
{
	PluginInitializerFieldListNode *current = list;

	node->_next = NULL;

	if( list == NULL )
	{
		list = node;
	}
	else
	{
		for( current = list; current; current = current->_next )
		{
			if( ! current->_next )
			{
				current->_next = node;
				break;
			}
		}
	}
	return list;

}

PluginInitializerFieldListNode * opengl_plugin_initializer_field_list_add_integer( PluginInitializerFieldListNode *list, const char *path, int value )
{
	PluginInitializerFieldListNode *node = opengl_plugin_initializer_field_list_find_by_path( list, path );
	if(  node == NULL )
	{
		node = (PluginInitializerFieldListNode*) malloc( sizeof (PluginInitializerFieldListNode ));
		node->_next = NULL;
		list = opengl_plugin_initializer_field_list_add_node( list, node );
	}
	node->field_value.type = PIFT_INTEGER;
	node->field_value.value.int_value = value;
	node->field_name = strdup( path );
	return list;
}


PluginInitializerFieldListNode * opengl_plugin_initializer_field_list_add_float( PluginInitializerFieldListNode *list, const char *path, float value )
{
	PluginInitializerFieldListNode *node = opengl_plugin_initializer_field_list_find_by_path( list, path );
	if(  node == NULL )
	{
		node = (PluginInitializerFieldListNode*) malloc( sizeof (PluginInitializerFieldListNode ));
		node->_next = NULL;
		list = opengl_plugin_initializer_field_list_add_node( list, node );
	}
	node->field_value.type = PIFT_FLOAT;
	node->field_value.value.float_value = value;
	node->field_name = strdup( path );
	return list;
}


PluginInitializerFieldListNode * opengl_plugin_initializer_field_list_add_string( PluginInitializerFieldListNode *list, const char *path, const char* value )
{
	PluginInitializerFieldListNode *node = opengl_plugin_initializer_field_list_find_by_path( list, path );
	if(  node == NULL )
	{
		node = (PluginInitializerFieldListNode*) malloc( sizeof (PluginInitializerFieldListNode ));
		node->_next = NULL;
		list = opengl_plugin_initializer_field_list_add_node( list, node );
	}
	node->field_value.type = PIFT_STRING;
	node->field_value.value.string_value = strdup( value );
	node->field_name = strdup( path );
	return list;
}

int opengl_plugin_initializer_field_list_destroy(PluginInitializerFieldListNode *list)
{
	int items = 0;
	PluginInitializerFieldListNode *current = list;
	while( current )
	{
		opengl_plugin_initializer_field_list_node_destroy( current );
		current = current->_next;
		++ items;
	}
	return items;
}


#endif /* OPENGL_PLUGIN_PARAMS_HELPER_C_ */
