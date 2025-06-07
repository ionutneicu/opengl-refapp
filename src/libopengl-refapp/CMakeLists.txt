cmake_minimum_required(VERSION 3.5)



set(BACKEND "X11" CACHE STRING "Select backend: X11 or Dispanx")
set_property(CACHE BACKEND PROPERTY STRINGS "X11" "Dispanx")



file(GLOB opengllib_refapp_SRC ./src/*c ./src/*.cpp ./src/*.h )


add_library(opengl_refapp SHARED ${opengllib_refapp_SRC} )

target_include_directories( opengl_refapp PRIVATE src )
target_include_directories( opengl_refapp PRIVATE src/x11 )
target_include_directories( opengl_refapp PUBLIC  include )
target_include_directories( opengl_refapp PUBLIC  common )



if(BACKEND STREQUAL "X11")
    message(STATUS "Using X11 backend")
	add_subdirectory("backends/x11")
	target_link_libraries( opengl_refapp lib_backend_x11)
	
elseif(BACKEND STREQUAL "Dispmanx")
    message(STATUS "Using Dispmanx backend")
	add_subdirectory("backends/dispmanx")
	target_link_libraries( opengl_refapp lib_backend_dispmanx)
endif()