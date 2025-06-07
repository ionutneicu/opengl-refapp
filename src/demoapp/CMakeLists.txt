cmake_minimum_required(VERSION 3.2)

file(GLOB opengl_demoapp_SRC ./*.c ./*.cpp ./*.h )
add_executable(opengl_demoapp ${opengl_demoapp_SRC})
target_link_libraries( opengl_demoapp  opengl_refapp)

#helper for running demo inside build directory
add_custom_target(texture_symlink ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_LIST_DIR}/resources/texture_big.bmp ${CMAKE_CURRENT_BINARY_DIR}/texture.bmp)
add_dependencies( opengl_demoapp texture_symlink)