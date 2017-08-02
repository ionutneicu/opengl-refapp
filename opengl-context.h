/*
 * opengl-context.h
 *
 *  Created on: Jul 1, 2017
 *      Author: ionut
 */

#ifndef OPENGL_CONTEXT_H_
#define OPENGL_CONTEXT_H_

#include "platform-egl-context.h"
#include <GLES2/gl2.h>


typedef struct tagOpenGLContext OpenGLContext;
typedef int ( *user_loop_function_pf)( OpenGLContext *ctx );

OpenGLContext* 		opengl_context_create( PlatformEGLContext* eglctx );
PlatformEGLContext* opengl_get_egl_context( OpenGLContext* ctx );
void  				opengl_context_attach_user_ctx( OpenGLContext* ctx, void *private_data );
void* 				opengl_context_get_user_ctx( OpenGLContext* ctx );
GLuint 				opengl_load_texture_in_gpu( void* texture_data, unsigned int tex_width, unsigned int tex_height );
void 				opengl_unload_texture_from_gpu( GLuint texture );
void 				opengl_draw_texture( OpenGLContext* ctx, GLuint texture, const float scale_factor );
void 				opengl_mainloop( OpenGLContext * ctx, user_loop_function_pf user_loop );
void 				opengl_wiewport(OpenGLContext* eglctx,
							   unsigned short orig_x,
							   unsigned short orig_y,
							   unsigned short width,
							   unsigned short height);
void 				opengl_draw(OpenGLContext* opengl_ctx,
								user_loop_function_pf user_loop);

#endif /* OPENGL_CONTEXT_H_ */
