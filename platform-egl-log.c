/*
 * platform-egl-log.c
 *
 *  Created on: Jul 1, 2017
 *      Author: ionut
 */


#include <stdarg.h>
#include <stdio.h>

#include "platform-egl-log.h"




inline void platform_egl_varlog( 	const log_level_t level,
                         	 	 	const char *file,
                         	 	 	const unsigned int line,
                         	 	 	const char* format, ... )
{
    char dest[1024 * 16];
    va_list argptr;
    va_start(argptr, format);
    vsnprintf( dest, sizeof( dest ) , format, argptr );
    va_end(argptr);
    switch( level )
    {
    	case	LL_DEBUG :
    			fprintf(stdout,"DEBUG: %s:%d %s\n", file, line, dest );
    	break;
    	case	LL_INFO :
    			fprintf(stdout,"INFO : %s:%d %s\n", file, line, dest );
    	break;
    	case	LL_WARN :
    		fprintf(stderr,"WARN : %s:%d %s\n", file, line, dest );
    	break;
    	case	LL_ERROR :
    			fprintf(stderr,"ERROR: %s:%d %s\n", file, line, dest );
    	break;
    }
}
