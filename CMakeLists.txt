cmake_minimum_required(VERSION 3.5)
project(opengl-refapp)
SET(CMAKE_BUILD_TYPE Debug)



add_subdirectory(src/libopengl-refapp)
add_subdirectory(src/demoapp)
add_subdirectory(src/plugin_app)