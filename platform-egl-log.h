/*
 * platorm-egl-log.h
 *
 *  Created on: Jun 28, 2017
 *      Author: ionut
 */

#ifndef PLATORM_EGL_LOG_H_
#define PLATORM_EGL_LOG_H_


#include <stdio.h>
enum log_level
{
	LL_VERBOSE = 0, // use this in loops, wi;l produce a lot of messages
	LL_DEBUG,       // normal debug, must be supreessed for real user
	LL_INFO,        // information for non-developer users
	LL_WARN,
	LL_ERROR,
	LL_OFF			// don't log with this
};

typedef enum log_level log_level_t;


void platform_egl_log_init( log_level_t level );

void platform_egl_varlog(	const log_level_t level,
							const char *file,
							const char *function,
							const unsigned int line,
							const char* format, ... );

#define LVERBOSE( __FORMAT__ , ...)   platform_egl_varlog(  LL_VERBOSE,  __FILE__, __FUNCTION__, __LINE__, __FORMAT__ , ##__VA_ARGS__);
#define LDEBUG( __FORMAT__ , ...)     platform_egl_varlog(  LL_DEBUG,  __FILE__, __FUNCTION__, __LINE__, __FORMAT__ , ##__VA_ARGS__);
#define LINFO( __FORMAT__ , ...)      platform_egl_varlog(  LL_INFO ,  __FILE__, __FUNCTION__, __LINE__, __FORMAT__ , ##__VA_ARGS__);
#define LWARN( __FORMAT__ , ...)      platform_egl_varlog(  LL_WARN ,  __FILE__, __FUNCTION__, __LINE__, __FORMAT__ , ##__VA_ARGS__);
#define LERROR( __FORMAT__ , ...)     platform_egl_varlog(  LL_ERROR,  __FILE__, __FUNCTION__, __LINE__, __FORMAT__ , ##__VA_ARGS__);




#endif /* PLATORM_EGL_LOG_H_ */
