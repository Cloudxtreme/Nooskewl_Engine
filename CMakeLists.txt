cmake_minimum_required(VERSION 3.0)
project(ss C)

if(NOT ("${USER_INCLUDE_PATH}" STREQUAL ""))
	include_directories(${USER_INCLUDE_PATH})
endif()
if(NOT ("${USER_LIBRARY_PATH}" STREQUAL ""))
	link_directories(${USER_LIBRARY_PATH})
endif()

include_directories("include")

set(SRC src/main.c src/tga.c)
add_executable(ss ${SRC})
target_link_libraries(ss SDL2 SDL2main glew32 opengl32)
