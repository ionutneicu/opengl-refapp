/*
 * opengl-user-context-helper.h
 *
 *  Created on: Sep 17, 2017
 *      Author: ionut
 *
 * Although not mandatory ( OpenGLUserContext can be completely implemented
 * by user according to his needs, this would be a common example where we
 * want to display multiple ( animated ) textures.
 */

#ifndef OPENGL_USER_CONTEXT_HELPER_H_
#define OPENGL_USER_CONTEXT_HELPER_H_


#include "opengl-context.h"

#include "plugin.h"



typedef struct tagOpenGLPluginListNode
{
	OpenGLPlugin*					plugin;
	unsigned int					plugin_id;
	struct tagOpenGLPluginListNode*	next;
} OpenGLPluginListNode;

typedef struct tagOpenGLUserContext
{
	unsigned int					last_plugin_id;
	OpenGLPluginListNode*			plugins_head;
} OpenGLUserContext;


#define USER_CONTEXT( __glctx__ ) ( (OpenGLUserContext*)__glctx__->user_ctx );

/**
 * opengl_user_ctx_init inits the context
 * @param opengl_user_ctx
 * @return
 */
int opengl_user_ctx_init( OpenGLUserContext *opengl_user_ctx );

/**
 * opengl_user_ctx_destroy destroys the context
 * @param opengl_user_ctx
 * @return
 */
int opengl_user_ctx_destroy( OpenGLUserContext *opengl_user_ctx );

/**
 * opengl_user_ctx_add_plugin adds new plugin in the user's context
 * @param opengl_user_ctx user context to add to
 * @param plugin plugin to be added
 * @return the ID of the new added plugin.
 * same instance of plugin can be added more times and will receive multiple IDs
 */
unsigned int  opengl_user_ctx_add_plugin( OpenGLUserContext *opengl_user_ctx, OpenGLPlugin* plugin );
/**
 * removes
 * @param opengl_user_ctx
 * @param plugin_id plugin id to be removed, this value is the one returned by @see opengl_user_ctx_add_plugin
 * @return 0 if success, non-zero if plugin could not be removed because plugin with such ID was not found in the context
 */
int opengl_user_ctx_remove_plugin( OpenGLUserContext *opengl_user_ctx, unsigned int plugin_id);



int opengl_plugin_init( OpenGLPlugin* plugin );

#endif /* OPENGL_USER_CONTEXT_HELPER_H_ */
