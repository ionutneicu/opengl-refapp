cmake_minimum_required(VERSION 3.0.2)
project(opengl-refapp)
SET(CMAKE_BUILD_TYPE Debug)


find_package(X11)
find_package(OpenGL REQUIRED COMPONENTS EGL GLES2 )


file(GLOB opengllib_refapp_SRC ./src/libopengl-refapp/*c ./src/libopengl-refapp/*.cpp ./src/libopengl-refapp/*.h )
file(GLOB opengllib_refapp_X11_SRC ./src/libopengl-refapp/x11/*c ./src/libopengl-refapp/x11/*.cpp ./src/libopengl-refapp/x11/*.h )
add_library(opengl_refapp SHARED ${opengllib_refapp_SRC} ${opengllib_refapp_X11_SRC})

target_include_directories( opengl_refapp PRIVATE src )
target_include_directories( opengl_refapp PRIVATE src/x11 )
target_include_directories( opengl_refapp PUBLIC include )

file(GLOB opengl_demoapp_SRC ./src/demoapp/*c ./src/demoapp/*.cpp ./src/demoapp/*.h )
add_executable(opengl_demoapp ${opengl_demoapp_SRC})
target_link_libraries( opengl_demoapp  opengl_refapp OpenGL::EGL OpenGL::GLES2 X11::X11)

