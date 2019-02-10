/*
 * opengl_plugin_registry.c
 *
 *  Created on: Sep 23, 2017
 *      Author: ionut
 */

#include <dirent.h>
#include "opengl-plugin-registry.h"
#include "stdlib.h"


extern OpenGLPlugin * opengl_plugin_static_picture_try_init( PluginInitializerFieldListNode *field_list );

#if 0

/**
 * opengl_plugin_registry_try_load_plugin - load plugin from filename
 * @param filename - filename containing shared library
 * @param info - information retrieved from plugin lib
 * @return 0 if success, non-zero value if error occured
 */
int opengl_plugin_registry_try_load_plugin( const char* filename, OpenGLPluginInfo *info )
{
	return 0;
}

/**
 * opengl_plugin_registry_init - init opengl plugin registry
 * @param plugin_path - path to search for plugins
 * @return 0 if init succeeded, other non-zero value if failed
 */
int opengl_plugin_registry_init( const char *plugin_path)
{
	DIR *directory;
	struct dirent *dir;
	directory = opendir( plugin_path );
	if (directory) {
		while ((dir = readdir(directory)) != NULL)
		{
			printf("%s\n", dir->d_name);
			char* ext = strrchr(dir->d_name, '.');
			if( strcmp( ext, ".so") )
			{
				OpenGLPluginInfo plugin_info;
				if( 0 == opengl_plugin_registry_try_load_plugin( dir->d_name, &plugin_info ) )
				{

				}
				else
				{
					LL_WARN("could not load %s, not valid plugin", dir->d_name );
				}
			}
		}
		closedir(directory);
	}
	return(0);
}

#endif

/**
 * opengl_plugin_registry_create_plugin_by_name creates a plugin of known type using to initialize its members
 **
 *
 * @param plugin_path
 * @return
 */
int opengl_plugin_registry_init( const char *plugin_path)
{
	return 0;
}

/**
 * opengl_plugin_registry_create_plugin_by_name creates a plugin of known type using json to initialize its members
 * @param plugin_name - known plugin type
 * @param json_plugin_initalizer - json-formatted string containing plugin's initializer parameters
 * @return plugin in case of success, or NULL if error
 */
OpenGLPlugin * opengl_plugin_registry_create_plugin_by_name( const char *plugin_name, PluginInitializerFieldListNode *field_list)
{
		 if( strcmp( plugin_name, "static.picture")==0)
			 return opengl_plugin_static_picture_try_init( field_list );
		 //else
		 //	 if( strcmp( plugin_name, "textscrool.horizontal"))
		 //		 return opengl_plugin_textscrool_try_init( field_list);
		return 0;
}
