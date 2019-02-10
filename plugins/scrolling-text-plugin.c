/*
 * cairo-sample-plugin.c
 *
 *  Created on: Aug 30, 2017
 *      Author: ionut
 */
#include "plugin.h"
#include "opengl-plugin-params-helper.h"
#include <stdlib.h>

#define NUM_TEXTURES 4


typedef struct _CairoSamplePluginContext
{
     int textures[ NUM_TEXTURES ];
     const char *text;
     int position_x;
     int position_y;
     float zoom_x;
     float zoom_y;
} CairoSamplePluginContext;






