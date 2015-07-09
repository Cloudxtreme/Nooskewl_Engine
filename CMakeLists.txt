cmake_minimum_required(VERSION 3.0)
project(Nooskewl_Engine CXX)

option(TRENTS_BUILD "Build my way" off)

if(NOT ("${USER_INCLUDE_PATH}" STREQUAL ""))
	include_directories(${USER_INCLUDE_PATH})
endif()
if(NOT ("${USER_LIBRARY_PATH}" STREQUAL ""))
	link_directories(${USER_LIBRARY_PATH})
endif()

if(WIN32)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x501 /DWITH_SDL")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x501 /DWITH_SDL /DTGUI_DEBUG")
	if(TRENTS_BUILD)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DTRENTS_BUILD")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DTRENTS_BUILD")
	endif()
endif()

include_directories("include")

set(LIB_SRC
	src/lib/audio.cpp
	src/lib/brain.cpp
	src/lib/cpa.cpp
	src/lib/engine.cpp
	src/lib/font.cpp
	src/lib/graphics.cpp
	src/lib/image.cpp
	src/lib/load_dll.cpp
	src/lib/log.cpp
	src/lib/map.cpp
	src/lib/map_entity.cpp
	src/lib/map_logic.cpp
	src/lib/player_brain.cpp
	src/lib/resource_manager.cpp
	src/lib/snprintf.cpp
	src/lib/speech.cpp
	src/lib/sprite.cpp
	src/lib/tilemap.cpp
	src/lib/tween.cpp
	src/lib/util.cpp
	src/lib/vertex_accel.cpp
	src/lib/video.cpp
	src/lib/widgets.cpp
	src/lib/xml.cpp
)

set(RUNNER_SRC
	src/runner/runner.cpp
)

add_library(Nooskewl_Engine SHARED ${LIB_SRC})
set_target_properties(Nooskewl_Engine PROPERTIES COMPILE_FLAGS "/DLIB_BUILD")
target_link_libraries(Nooskewl_Engine tgui3 SDL2_ttf SDL2 SDL2main glew32 zlib opengl32 d3d9 d3dx9 winmm)

add_executable(Nooskewl_Runner ${RUNNER_SRC})
target_link_libraries(Nooskewl_Runner Nooskewl_Engine)