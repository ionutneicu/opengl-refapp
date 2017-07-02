/*
 * platform-opengl-draw.h
 *
 *  Created on: Jul 1, 2017
 *      Author: ionut
 */

#ifndef PLATFORM_OPENGL_DRAW_H_
#define PLATFORM_OPENGL_DRAW_H_

void platform_opengl_draw(PlatformEGLContext* eglctx);
void platform_opengl_wiewport(PlatformEGLContext* eglctx,
							   unsigned short orig_x,
							   unsigned short orig_y,
							   unsigned short width,
							   unsigned short height);

#endif /* PLATFORM_OPENGL_DRAW_H_ */
