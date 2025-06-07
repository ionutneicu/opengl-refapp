cmake_minimum_required(VERSION 3.0.2)


file(GLOB lib_backend_x11_SRC ./src/*c ./src/*.cpp ./src/*.h )
add_library(lib_backend_x11 SHARED ${lib_backend_x11_SRC} )
target_link_libraries( lib_backend_x11  X11::X11)

