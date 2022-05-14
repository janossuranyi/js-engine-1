cmake_minimum_required(VERSION 3.7)

project(Demo VERSION 1.0) 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/") 

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(assimp REQUIRED HINTS ${ASSIMP_DIR}) 

add_definitions(
  -DGLEW_STATIC
)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True) 

# Engine
file(GLOB ENGINE_ENGINE_SOURCE  JSE/core/source/engine/*.cpp)
file(GLOB ENGINE_ENGINE_INCLUDE JSE/core/include/engine/*.hpp)
# Graphics
file(GLOB ENGINE_GRAPHICS_SOURCE  JSE/core/source/graphics/*.cpp)
file(GLOB ENGINE_GRAPHICS_INCLUDE JSE/core/include/graphics/*.hpp)
# Scene
file(GLOB ENGINE_SCENE_SOURCE  JSE/core/source/scene/*.cpp)
file(GLOB ENGINE_SCENE_INCLUDE JSE/core/include/scene/*.hpp)
# Input
file(GLOB ENGINE_INPUT_SOURCE  JSE/core/source/input/*.cpp)
file(GLOB ENGINE_INPUT_INCLUDE JSE/core/include/input/*.hpp)

# Math
# file(GLOB ENGINE_MATH_SOURCE  JSE/core/source/math/*.cpp)
# file(GLOB ENGINE_MATH_INCLUDE JSE/core/include/math/*.hpp)
# Resources
# file(GLOB ENGINE_RESOURCES_SOURCE  JSE/core/source/resources/*.cpp)
# file(GLOB ENGINE_RESOURCES_INCLUDE JSE/core/include/resources/*.hpp)
# System
file(GLOB ENGINE_SYSTEM_SOURCE  JSE/core/source/system/*.cpp)
file(GLOB ENGINE_SYSTEM_INCLUDE JSE/core/include/system/*.hpp)
# Impl
file(GLOB ENGINE_IMPL_SOURCE  JSE/core/source/impl/*.cpp)
file(GLOB ENGINE_IMPL_INCLUDE JSE/core/include/impl/*.hpp)

file(GLOB SOIL2_INCLUDE external/soil2/incs/*.h)
file(GLOB SOIL2_SOURCE external/soil2/srcs/*.c)


add_library(engine STATIC 
  ${ENGINE_ENGINE_SOURCE}
  ${ENGINE_ENGINE_INCLUDE}
  ${ENGINE_INPUT_SOURCE}
  ${ENGINE_INPUT_INCLUDE}
  ${ENGINE_GRAPHICS_SOURCE}
  ${ENGINE_GRAPHICS_INCLUDE}
  ${ENGINE_MATH_SOURCE}
  ${ENGINE_MATH_INCLUDE}
  ${ENGINE_RESOURCES_SOURCE}
  ${ENGINE_RESOURCES_INCLUDE}
  ${ENGINE_SYSTEM_SOURCE}
  ${ENGINE_SYSTEM_INCLUDE}
  ${ENGINE_IMPL_SOURCE}
  ${ENGINE_IMPL_INCLUDE}
  ${ENGINE_SCENE_SOURCE}
  ${ENGINE_SCENE_INCLUDE}
)

add_library(SOIL2 STATIC
  ${SOIL2_INCLUDE}
  ${SOIL2_SOURCE}
)

add_library(glew STATIC
  external/glew-2.1.0/src/glew.c
  external/glew-2.1.0/include/GL/glew.h
)

add_library(stb_image STATIC
  external/stb/stb_image.h
  external/stb/stb_image.c
)

include_directories(
  JSE/core/include/  
  external/glew-2.1.0/include/
  external/glm/
  external/stb/
  external/soil2/incs/
  ${ASSIMP_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIR}
)

link_directories(
  ${ASSIMP_LIBRARY_DIRS}
)

message(STATUS "Assimp lib path: ${ASSIMP_LIBRARY_DIRS}")

file(GLOB SHADERS assets/shaders/*.glsl)

add_executable(demo
	demo/demo.cpp
  demo/cube.hpp
	demo/demo.hpp
  ${SHADERS}  
)

target_link_libraries(demo
  engine
  glew
  stb_image
  ${OPENGL_LIBRARY}
  ${ASSIMP_LIBRARIES}
  ${SDL2_LIBRARIES}
)

if(WIN32)

  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH "x64")
  else()
    set(ARCH "x86")
  endif()

  add_custom_command(TARGET demo POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy
     "${assimp_DIR}/../../../bin/assimp${ASSIMP_LIBRARY_SUFFIX}.dll"
     $<TARGET_FILE_DIR:demo>)

  add_custom_command(TARGET demo POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy
     "${SDL2_INCLUDE_DIR}/../lib/${ARCH}/SDL2.dll"
     $<TARGET_FILE_DIR:demo>)
endif()

source_group("Engine" FILES ${ENGINE_ENGINE_SOURCE} ${ENGINE_ENGINE_INCLUDE})
source_group("Graphics" FILES ${ENGINE_GRAPHICS_SOURCE} ${ENGINE_GRAPHICS_INCLUDE})
#source_group("Math" FILES ${ENGINE_MATH_SOURCE} ${ENGINE_MATH_INCLUDE})
#source_group("Resources" FILES ${ENGINE_RESOURCES_SOURCE} ${ENGINE_RESOURCES_INCLUDE})
source_group("Scene" FILES ${ENGINE_SCENE_SOURCE} ${ENGINE_SCENE_INCLUDE})
source_group("System" FILES ${ENGINE_SYSTEM_SOURCE} ${ENGINE_SYSTEM_INCLUDE})
source_group("Impl" FILES ${ENGINE_IMPL_SOURCE} ${ENGINE_IMPL_INCLUDE})
source_group("Input" FILES ${ENGINE_INPUT_SOURCE} ${ENGINE_INPUT_INCLUDE})
