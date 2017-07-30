/*
 * platform-egl-context-x11.c
 *
 *  Created on: Jun 27, 2017
 *      Author: ionut
 */

#include "platform-egl-context-priv.h"
#include "platform-egl-log.h"

/*TODO circular dependency between OpenGL and EGL*/
#include "platform-opengl-draw.h"

#define ERR_CANNOT_OPEN_DISPLAY  		-1
#define ERR_CANNOT_GET_EGL_DISPLAY		-2
#define ERR_CANNOT_INITIALIZE_EGL		-3
#define ERR_EGL_CHOOSE_CONFIG			-4
#define ERR_GET_VISUAL_INFO				-5
#define ERR_CREATE_WINDOW				-6
#define ERR_CREATE_EGL_CONTEXT			-7
#define ERR_CREATE_EGL_SURFACE			-8
#define ERR_CONTEXT_NOT_INITIALIZED    -100
#define ERR_NOMEM					   -999

#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>

typedef struct tagPlatformEGLContextX11
{
	PlatformEGLContext  m_parent_ctx;
	Display * 			m_native_x_display;
	Window   			m_native_x_window;
} PlatformEGLContextX11;



static int platform_egl_context_display_open_x11(PlatformEGLContextX11 *ctx_x11)
{
	EGLint major, minor;



	ctx_x11->m_native_x_display = XOpenDisplay(NULL);
	if (!ctx_x11->m_native_x_display ) {
		return ERR_CANNOT_OPEN_DISPLAY;
	}

	ctx_x11->m_parent_ctx.m_egl_dpy= eglGetDisplay(ctx_x11->m_native_x_display);
	if (ctx_x11->m_parent_ctx.m_egl_dpy  == EGL_NO_DISPLAY) {
		XCloseDisplay(ctx_x11->m_native_x_display);
		return ERR_CANNOT_GET_EGL_DISPLAY;
	}

	if (!eglInitialize(ctx_x11->m_parent_ctx.m_egl_dpy, &major, &minor)) {
		XCloseDisplay(ctx_x11->m_native_x_display);
		return ERR_CANNOT_INITIALIZE_EGL;
	}
	LDEBUG( "EGL: %d.%d\n", major, minor);
	return 0;
}


static int platform_egl_context_close_x11(PlatformEGLContextX11 *ctx_x11)
{
	if (! ctx_x11->m_parent_ctx.m_initialized)
		return ERR_CONTEXT_NOT_INITIALIZED;

	eglTerminate(ctx_x11->m_parent_ctx.m_egl_ctx);
	XCloseDisplay(ctx_x11->m_native_x_display);
	return 0;
}

static void platform_egl_context_event_loop_x11(PlatformEGLContextX11* eglctx,  PlatformOpenGLContext *opengl_ctx)
{
	while (1)
	{
		char buffer[16];
		XEvent event;
		if(  XPending( eglctx->m_native_x_display ) )
		{
			XNextEvent(eglctx->m_native_x_display, &event);

			switch (event.type)
			{
			case Expose:
				break;

			case ConfigureNotify:
				platform_opengl_wiewport(opengl_ctx, 0,0, event.xconfigure.width, event.xconfigure.height );
				break;

			case KeyPress:
				XLookupString(&event.xkey, buffer, sizeof(buffer), NULL, NULL);
				if (buffer[0] == 27)
					return;
				break;
			default:
				break;
			}
		}
		platform_opengl_draw(opengl_ctx);
	}
}



static int platform_egl_context_create_window(PlatformEGLContextX11* eglctx)
{
	static const EGLint attribs[] = {
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_DEPTH_SIZE, 1,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	static const EGLint attrs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	static const char name[] = "advertplay";

	XSetWindowAttributes attr;
	unsigned long mask;
	XVisualInfo visual;
	EGLint num_configs;
	XVisualInfo *info;
	XSizeHints hints;
	EGLConfig config;
	int num_visuals;
	EGLint version;
	Window root;
	int screen;
	EGLint vid;



	screen = DefaultScreen(eglctx->m_native_x_display);
	root = RootWindow(eglctx->m_native_x_display, screen);

	if (!eglChooseConfig(eglctx->m_parent_ctx.m_egl_dpy, attribs, &config, 1, &num_configs)) {
		return ERR_EGL_CHOOSE_CONFIG;
	}

	if (!eglGetConfigAttrib(eglctx->m_parent_ctx.m_egl_dpy, config, EGL_NATIVE_VISUAL_ID, &vid)) {
		return ERR_EGL_CHOOSE_CONFIG;
	}

	visual.visualid = vid;

	info = XGetVisualInfo(eglctx->m_native_x_display, VisualIDMask, &visual, &num_visuals);

	if (!info) {
		return ERR_GET_VISUAL_INFO;
	}

	memset(&attr, 0, sizeof(attr));
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(eglctx->m_native_x_display, root, info->visual, AllocNone);
	attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	eglctx->m_native_x_window = XCreateWindow(eglctx->m_native_x_display,
											  root,
											  0,
											  0,
											  eglctx->m_parent_ctx.m_width,
											  eglctx->m_parent_ctx.m_height,
											  0,
											  info->depth,
											  InputOutput, info->visual,
											  mask,
											  &attr);
	if (!eglctx->m_native_x_window ) {
		return ERR_CREATE_WINDOW;
	}

	memset(&hints, 0, sizeof(hints));
	hints.x = 0;
	hints.y = 0;
	hints.width =  eglctx->m_parent_ctx.m_width;
	hints.height = eglctx->m_parent_ctx.m_height;
	hints.flags = USSize | USPosition;


	Atom wm_state   = XInternAtom (eglctx->m_native_x_display, "_NET_WM_STATE", 1 );
	Atom wm_fullscreen = XInternAtom (eglctx->m_native_x_display, "_NET_WM_STATE_FULLSCREEN", 1 );

	XChangeProperty(eglctx->m_native_x_display,  eglctx->m_native_x_window, wm_state, XA_ATOM, 32,
	                PropModeReplace, (unsigned char *)&wm_fullscreen, 1);

	XSetNormalHints(eglctx->m_native_x_display, eglctx->m_native_x_window, &hints);
	XSetStandardProperties(eglctx->m_native_x_display, eglctx->m_native_x_window, name, name, None,
			       NULL, 0, &hints);

	eglBindAPI(EGL_OPENGL_ES_API);

	eglctx->m_parent_ctx.m_egl_ctx = eglCreateContext(eglctx->m_parent_ctx.m_egl_dpy,
													  config,
													  EGL_NO_CONTEXT,
													  attrs);

	if (eglctx->m_parent_ctx.m_egl_ctx == EGL_NO_CONTEXT) {
		return ERR_CREATE_EGL_CONTEXT;
	}

	eglQueryContext(eglctx->m_parent_ctx.m_egl_dpy,
					eglctx->m_parent_ctx.m_egl_ctx,
					EGL_CONTEXT_CLIENT_VERSION,
					&version);
	LINFO("OpenGL ES: %d\n", version);

	eglctx->m_parent_ctx.m_egl_surf = eglCreateWindowSurface(eglctx->m_parent_ctx.m_egl_dpy,
											 config,
											 eglctx->m_native_x_window,
											 NULL);

	if( eglctx->m_parent_ctx.m_egl_surf == EGL_NO_SURFACE ) {
		return ERR_CREATE_EGL_SURFACE;
	}
	XFree(info);
	return 0;
}

void platform_egl_context_swap_buffers_x11( PlatformEGLContextX11 *eglctx )
{
	eglSwapBuffers(eglctx->m_parent_ctx.m_egl_dpy, eglctx->m_parent_ctx.m_egl_surf);

}

static void platform_egl_context_close_window_x11(PlatformEGLContextX11 *eglctx)
{
	if (!eglctx)
		return;

	eglDestroySurface(eglctx->m_parent_ctx.m_egl_dpy, eglctx->m_parent_ctx.m_egl_surf);
	eglDestroyContext(eglctx->m_parent_ctx.m_egl_dpy, eglctx->m_parent_ctx.m_egl_ctx );
	XDestroyWindow(eglctx->m_native_x_display, eglctx->m_native_x_window );

}

static void platform_egl_context_show_window_x11(PlatformEGLContextX11 *eglctx)
{
	XMapWindow(eglctx->m_native_x_display, eglctx->m_native_x_window );
	if (!eglMakeCurrent(eglctx->m_parent_ctx.m_egl_dpy,
						eglctx->m_parent_ctx.m_egl_surf,
						eglctx->m_parent_ctx.m_egl_surf,
						eglctx->m_parent_ctx.m_egl_ctx) )
	{
		LERROR("eglMakeCurrent():\n");
	}
	XFlush(eglctx->m_native_x_display);
}


PlatformEGLContext* platform_egl_context_create()
{
	PlatformEGLContextX11* ctx = ( PlatformEGLContextX11 *)malloc( sizeof( PlatformEGLContextX11 ));
	ctx->m_parent_ctx.m_initialized = 0;
	ctx->m_parent_ctx.m_width = 1;
	ctx->m_parent_ctx.m_height = 1;
	return (PlatformEGLContext*)ctx;
}

void platform_egl_context_destroy( PlatformEGLContext *ctx )
{
	PlatformEGLContextX11* ctx_x11 = ( PlatformEGLContextX11 *)ctx;
	free( ctx_x11 );
}

int  platform_egl_context_init( PlatformEGLContext *ctx )
{
	int rc;
	PlatformEGLContextX11* ctx_x11 = ( PlatformEGLContextX11 *)ctx;
	rc = platform_egl_context_display_open_x11(ctx_x11);
	if( rc )
		return rc;
	rc = platform_egl_context_create_window(ctx_x11);
	if( rc )
		return rc;

    platform_egl_context_show_window_x11(ctx_x11);
	ctx_x11->m_parent_ctx.m_initialized = 1;
	return rc;
}


void platform_egl_context_deinit( PlatformEGLContext *ctx )
{
	PlatformEGLContextX11* ctx_x11 = ( PlatformEGLContextX11 *)ctx;
	platform_egl_context_close_window_x11( ctx_x11 );
	platform_egl_context_close_x11( ctx_x11 );
	ctx_x11->m_parent_ctx.m_initialized = 0;
}


void platform_opengl_mainloop( PlatformOpenGLContext *opengl_ctx )
{
	PlatformEGLContextX11* ctx_x11 = ( PlatformEGLContextX11 *) opengl_ctx->m_egl_context;
	platform_egl_context_event_loop_x11( ctx_x11, opengl_ctx );
}


void platform_egl_context_swap_buffers( PlatformEGLContext *ctx )
{
	PlatformEGLContextX11* ctx_x11 = ( PlatformEGLContextX11 *)ctx;
	platform_egl_context_swap_buffers_x11( ctx_x11 );
}
