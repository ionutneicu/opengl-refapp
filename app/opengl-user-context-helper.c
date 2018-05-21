/*
 * opengl-user-context-helper.c
 *
 *  Created on: Sep 23, 2017
 *      Author: ionut
 */
#include "opengl-user-context-helper.h"
#include <stdlib.h>

int opengl_plugin_init( OpenGLPlugin* plugin )
{
	if( plugin->init_func )
		return plugin->init_func( plugin );
	return 0;
}


int opengl_user_ctx_init( OpenGLUserContext *opengl_user_ctx )
{
	opengl_user_ctx->last_plugin_id  = 0;
	opengl_user_ctx->plugins_head = NULL;
}

int opengl_user_ctx_destroy( OpenGLUserContext *opengl_user_ctx )
{
	OpenGLPluginListNode *node = opengl_user_ctx->plugins_head;
	while(node)
	{
		OpenGLPluginListNode *next = node->next;
		free( node );
		node = next;
	}
	opengl_user_ctx->last_plugin_id  = 0;
	opengl_user_ctx->plugins_head = NULL;

}

unsigned int  opengl_user_ctx_add_plugin( OpenGLUserContext *opengl_user_ctx, OpenGLPlugin* plugin )
{

   OpenGLPluginListNode *new_node = (OpenGLPluginListNode*) malloc( sizeof( OpenGLPluginListNode ));
   new_node->plugin = plugin;
   new_node->next = NULL;
   new_node->plugin_id = ++opengl_user_ctx->last_plugin_id;

   if( opengl_user_ctx->plugins_head == NULL )
	   opengl_user_ctx->plugins_head = new_node;
   else
   {
		OpenGLPluginListNode *node = opengl_user_ctx->plugins_head;
		for(  ; node; node = node->next )
		{
			if( node->next == NULL )
			{
				node->next = new_node;
				break;
			}
		}
   }
   return new_node->plugin_id;

}

int opengl_user_ctx_remove_plugin( OpenGLUserContext *opengl_user_ctx, unsigned int plugin_id)
{
	OpenGLPluginListNode *node = opengl_user_ctx->plugins_head;
	for(  ; node; node = node->next )
	{
		if( node->plugin_id == plugin_id )
		{
			OpenGLPluginListNode *next = node->next;
			free(opengl_user_ctx->plugins_head);
			opengl_user_ctx->plugins_head = next;
		}
	}
}
