 
 
cmake_minimum_required(VERSION 3.15)
project(gl LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# source
add_library(gllib
  src/gl.cpp
)
target_include_directories(gllib
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_executable(gl
    src/app.cpp
)
target_link_libraries(gl PRIVATE gllib)

# glfw
add_subdirectory(external/glfw)
target_link_libraries(gl PRIVATE glfw)
if (WIN32)
  target_link_libraries(gl PRIVATE opengl32)
endif()

# glew
add_library(glew STATIC external/glew/src/glew.c)
target_compile_definitions(glew PUBLIC GLEW_STATIC GLEW_NO_GLU)
target_include_directories(glew PUBLIC external/glew/include)
target_link_libraries(gl PRIVATE glew)

