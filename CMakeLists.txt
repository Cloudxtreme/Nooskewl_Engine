cmake_minimum_required(VERSION 3.0)
project(StarSquatters CXX)

if(NOT ("${USER_INCLUDE_PATH}" STREQUAL ""))
	include_directories(${USER_INCLUDE_PATH})
endif()
if(NOT ("${USER_LIBRARY_PATH}" STREQUAL ""))
	link_directories(${USER_LIBRARY_PATH})
endif()

include_directories("include")

set(SRC
	src/audio.cpp
	src/font.cpp
	src/image.cpp
	src/log.cpp
	src/main.cpp
	src/snprintf.cpp
	src/tilemap.cpp
	src/util.cpp
	src/vertex_accel.cpp
	src/video.cpp
)

add_executable(StarSquatters ${SRC})
target_link_libraries(StarSquatters SDL2 SDL2main SDL2_ttf glew32 opengl32 winmm)