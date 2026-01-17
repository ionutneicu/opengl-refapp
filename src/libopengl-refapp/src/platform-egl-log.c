/*
 * platform-egl-log.c
 *
 *  Created on: Jul 1, 2017
 *      Author: ionut
 */


#include <stdarg.h>
#include <stdio.h>

#include "platform-egl-log.h"



static log_level_t current_level;

void platform_egl_log_init( log_level_t level )
{
	current_level = level;
}

inline void platform_egl_varlog( 	const log_level_t level,
                         	 	 	const char *file,
                         	 	 	const char *function,
                         	 	 	const unsigned int line,
                         	 	 	const char* format, ... )
{
    char dest[1024 * 16];
    va_list argptr;
    if( level < current_level )
    	return;
    va_start(argptr, format);
    vsnprintf( dest, sizeof( dest ) , format, argptr );
    va_end(argptr);
    switch( level )
    {
    	case	LL_DEBUG :
    			fprintf(stdout,"DEBUG: %s:%d [%s] %s\n", file, line, function, dest );
    	break;
    	case	LL_INFO :
    			fprintf(stdout,"INFO : %s:%d [%s] %s\n", file, line, function, dest );
    	break;
    	case	LL_WARN :
    		fprintf(stderr,"WARN : %s:%d [%s] %s\n", file, line, function, dest );
    	break;
    	case	LL_ERROR :
    			fprintf(stderr,"ERROR: %s:%d [%s] %s\n", file, line, function, dest );
    	break;
    }
}
