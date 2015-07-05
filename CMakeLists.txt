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
	src/animation_set.cpp
	src/audio.cpp
	src/brain.cpp
	src/floor_trigger.cpp
	src/font.cpp
	src/graphics.cpp
	src/image.cpp
	src/log.cpp
	src/main.cpp
	src/map.cpp
	src/map_entity.cpp
	src/player_brain.cpp
	src/resource_manager.cpp
	src/snprintf.cpp
	src/tilemap.cpp
	src/util.cpp
	src/vertex_accel.cpp
	src/video.cpp
	src/xml.cpp
)

add_executable(StarSquatters ${SRC})
target_link_libraries(StarSquatters SDL2 SDL2main SDL2_ttf glew32 opengl32 winmm)