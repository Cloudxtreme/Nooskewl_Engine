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
	src/brain.cpp
	src/cpa.cpp
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
	src/speech.cpp
	src/sprite.cpp
	src/tilemap.cpp
	src/util.cpp
	src/vertex_accel.cpp
	src/video.cpp
	src/xml.cpp
)

set(TGUI3_SRC
	src/tgui3.cpp
)

add_library(tgui3 SHARED ${TGUI3_SRC})
add_executable(StarSquatters ${SRC})

if(WIN32)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x501 /DWITH_SDL")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x501 /DWITH_SDL")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x501 /DWITH_SDL /DTGUI_DEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x501 /DWITH_SDL /DTGUI_DEBUG")
endif()

target_link_libraries(StarSquatters SDL2 SDL2main SDL2_ttf glew32 zlib opengl32 tgui3 winmm)