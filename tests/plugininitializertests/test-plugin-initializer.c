/*
 * test-plugin-initializer.c
 *
 *  Created on: Oct 6, 2017
 *      Author: ionut
 */


#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "../plugins/opengl-plugin-params-helper.h"

const char INTEGER_FIELD_PATH[]  = "my.first.integer";
int        INTEGER_VAL			 = 0xcaffebad;

const char FLOAT_FIELD_PATH[]    = "my.first.float";
float      FLOAT_VAL			 = 0.12345678f;


const char STRING_FIELD_PATH[]  = "my.first.string";
const char STRING_VAL[]			= "some first string";

/* The suite initialization function.
 */

PluginInitializerFieldListNode * list;

int init_suite_plugin_initializers(void)
{
	list = NULL;
    return 0;
}

/* The suite cleanup function.
 */

int clean_suite_plugin_initializers(void)
{
	return 0;
}


void test_add_integer_to_plugin_list(void)
{
	list = opengl_plugin_initializer_field_list_add_integer( list, INTEGER_FIELD_PATH, INTEGER_VAL );
	PluginInitializerFieldListNode * integer = opengl_plugin_initializer_field_list_find_by_path( list, INTEGER_FIELD_PATH );
	CU_ASSERT( integer != NULL );
	CU_ASSERT( 0 == strcmp( integer->field_name, INTEGER_FIELD_PATH  ));
	CU_ASSERT( integer->field_value.type == PIFT_INTEGER );
	CU_ASSERT( integer->field_value.value.int_value == INTEGER_VAL );

}

void test_add_float_to_plugin_list(void)
{
	list = opengl_plugin_initializer_field_list_add_float(  list, FLOAT_FIELD_PATH, FLOAT_VAL );
	PluginInitializerFieldListNode * floatnode = opengl_plugin_initializer_field_list_find_by_path( list,  FLOAT_FIELD_PATH );
	CU_ASSERT( floatnode != NULL );
	CU_ASSERT( 0 == strcmp( floatnode->field_name, FLOAT_FIELD_PATH  ));
	CU_ASSERT( floatnode->field_value.type == PIFT_FLOAT );
	CU_ASSERT( floatnode->field_value.value.float_value == FLOAT_VAL );
}


void test_add_string_to_plugin_list(void)
{
	list = opengl_plugin_initializer_field_list_add_string(  list, STRING_FIELD_PATH, STRING_VAL );
	PluginInitializerFieldListNode * stringnode = opengl_plugin_initializer_field_list_find_by_path( list,  STRING_FIELD_PATH );
	CU_ASSERT( stringnode != NULL );
	CU_ASSERT( 0 == strcmp( stringnode->field_name, STRING_FIELD_PATH  ));
	CU_ASSERT( stringnode->field_value.type == PIFT_STRING );
	CU_ASSERT( 0 == strcmp( stringnode->field_value.value.string_value, STRING_VAL ) );
}

void test_add_int_overwrite_string_plugin_list(void)
{
	list = opengl_plugin_initializer_field_list_add_integer( list, STRING_FIELD_PATH, INTEGER_VAL );
	PluginInitializerFieldListNode * integer = opengl_plugin_initializer_field_list_find_by_path( list, STRING_FIELD_PATH );
	CU_ASSERT( integer != NULL );
	CU_ASSERT( 0 == strcmp( integer->field_name, STRING_FIELD_PATH  ));
	CU_ASSERT( integer->field_value.type == PIFT_INTEGER );
	CU_ASSERT( integer->field_value.value.int_value == INTEGER_VAL );
}

void test_cleanup_plugin_list(void)
{
	CU_ASSERT( 3 == opengl_plugin_initializer_field_list_destroy(list) );
}


/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite_plugin_initializers, clean_suite_plugin_initializers);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if (
		   	   (NULL == CU_add_test(pSuite, "test of adding integer in plugin list", test_add_integer_to_plugin_list)) ||
		   	   (NULL == CU_add_test(pSuite, "test of adding float in plugin list", test_add_float_to_plugin_list)) ||
		   	   (NULL == CU_add_test(pSuite, "test of adding string in plugin list", test_add_string_to_plugin_list)) ||
		   	   (NULL == CU_add_test(pSuite, "test of adding integer overwrites string", test_add_int_overwrite_string_plugin_list)) ||
		   	   (NULL == CU_add_test(pSuite, "test of cleanup plugin list", test_cleanup_plugin_list))
		   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}

