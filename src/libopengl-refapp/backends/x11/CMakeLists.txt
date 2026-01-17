cmake_minimum_required(VERSION 3.5)

find_package(X11)

find_package(OpenGL REQUIRED COMPONENTS EGL GLES2 )

file(GLOB lib_backend_x11_SRC ./src/*c ./src/*.cpp ./src/*.h )
add_library(lib_backend_x11 SHARED ${lib_backend_x11_SRC} )
target_include_directories( lib_backend_x11 PRIVATE src)
target_include_directories( lib_backend_x11 PRIVATE ../../common )
target_link_libraries( lib_backend_x11  X11::X11 OpenGL::EGL OpenGL::GLES2)

