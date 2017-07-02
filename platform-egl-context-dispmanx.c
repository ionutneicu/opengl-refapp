/*
 * platform-egl-context-dispmanx.c
 *
 *  Created on: Jul 2, 2017
 *      Author: ionut
 */

#include "platform-egl-context.h"
#include "platform-egl-context-priv.h"
#include "platform-egl-log.h"
#include "bcm_host.h"


/*TODO circular dependency between OpenGL and EGL*/
#include "platform-opengl-draw.h"


typedef struct tagPlatformEGLContextX11
{
	PlatformEGLContext  		m_parent_ctx;
    DISPMANX_DISPLAY_HANDLE_T 	m_native_dispmanx_display;
	DISPMANX_ELEMENT_HANDLE_T 	m_native_dispmanx_element;
	EGL_DISPMANX_WINDOW_T 	 	m_native_dispmanx_window;
	DISPMANX_UPDATE_HANDLE_T 	m_native_dispmanx_update;
} PlatformEGLContextDispmanX;



static int platform_egl_context_display_open_dispmanx(PlatformEGLContextDispmanX *ctx)
{
   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 1,
      EGL_GREEN_SIZE, 1,
      EGL_BLUE_SIZE, 1,
      EGL_DEPTH_SIZE, 1,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_NONE };

   EGLConfig config;

   // get an EGL display connection
   ctx->m_parent_ctx.m_egl_dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   if( ctx->m_parent_ctx.m_egl_dpy != EGL_NO_DISPLAY)
   {
	   LERROR("could not get EGL display");
	   return ERR_EGL_DISPLAY_ERROR;
   }

   // initialize the EGL display connection
   result = eglInitialize(ctx->m_parent_ctx.m_egl_dpy, NULL, NULL);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   // this uses a BRCM extension that gets the closest match, rather than standard which returns anything that matches
   result = eglSaneChooseConfigBRCM(ctx->m_parent_ctx.m_egl_dpy, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   // create an EGL rendering context
   ctx->m_parent_ctx.m_egl_ctx = eglCreateContext(state->display, config, EGL_NO_CONTEXT, NULL);
   assert( ctx->m_parent_ctx.m_egl_ctx!=EGL_NO_CONTEXT );

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &ctx->m_parent_ctx->m_width, &ctx->m_parent_ctx->m_height);
   assert( success >= 0 );

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = ctx->m_parent_ctx->m_width;
   dst_rect.height = ctx->m_parent_ctx->m_height;

   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = ctx->m_parent_ctx->m_width << 16;
   src_rect.height = ctx->m_parent_ctx->m_height << 16;

   ctx->m_native_dispmanx_display = vc_dispmanx_display_open( 0 /* LCD */);
   ctx->m_native_dispmanx_update = vc_dispmanx_update_start( 0 );

   ctx->m_native_dispmanx_element = vc_dispmanx_element_add ( ctx->m_native_dispmanx_update, ctx->m_native_dispmanx_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);

   ctx->m_native_dispmanx_window.element = ctx->m_native_dispmanx_element;
   ctx->m_native_dispmanx_window.width = ctx->m_parent_ctx->m_width;
   ctx->m_native_dispmanx_window.height = ctx->m_parent_ctx->m_height;
   vc_dispmanx_update_submit_sync(  ctx->m_native_dispmanx_update );

   state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
   assert(state->surface != EGL_NO_SURFACE);

   return 0;
}


int  platform_egl_context_close_window_dispmanx( PlatformEGLContextDispmanX *ctx )
{

}


PlatformEGLContext* platform_egl_context_create()
{
	PlatformEGLContextDispmanX* ctx_dispmanx = ( PlatformEGLContextDispmanX *)malloc( sizeof( PlatformEGLContextDispmanX ));
	ctx_dispmanx->m_parent_ctx.m_initialized = 0;
	ctx_dispmanx->m_parent_ctx.m_width = 1;
	ctx_dispmanx->m_parent_ctx.m_height = 1;
	return (PlatformEGLContext*)ctx_dispmanx;
}

void platform_egl_context_destroy( PlatformEGLContext *ctx )
{
	PlatformEGLContextDispmanX* ctx_dispmanx = ( PlatformEGLContextDispmanX *)ctx;
	free( ctx_dispmanx );
}

int  platform_egl_context_init( PlatformEGLContext *ctx )
{
	int rc;
	PlatformEGLContextDispmanX* ctx_dispmanx = ( PlatformEGLContextDispmanX *)ctx;
	rc = platform_egl_context_display_open_dispmanx(ctx_dispmanx);
	if( rc )
		return rc;

	ctx_dispmanx->m_parent_ctx.m_initialized = 1;
	return rc;
}


void platform_egl_context_deinit( PlatformEGLContext *ctx )
{
	PlatformEGLContextDispmanX* ctx_dispmanx = ( PlatformEGLContextDispmanX *)ctx;
	platform_egl_context_close_window_dispmanx( ctx_dispmanx );
	ctx_dispmanx->m_parent_ctx.m_initialized = 0;
}


void platform_egl_context_mainloop( PlatformEGLContext *ctx )
{
	platform_opengl_draw( ctx );
}

void platform_egl_context_swap_buffers_dispmanx( PlatformEGLContextDispmanX *eglctx )
{
	eglSwapBuffers(eglctx->m_parent_ctx.m_egl_dpy, eglctx->m_parent_ctx.m_egl_surf);
}

void platform_egl_context_swap_buffers( PlatformEGLContext *ctx )
{
	PlatformEGLContextDispmanX* ctx_dispmanx = ( PlatformEGLContextDispmanX *)ctx;
	platform_egl_context_swap_buffers_dispmanx( ctx_dispmanx );
}

