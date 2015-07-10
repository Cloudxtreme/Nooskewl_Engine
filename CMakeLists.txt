cmake_minimum_required(VERSION 3.0)
project(Nooskewl_Engine CXX)

option(TRENTS_BUILD "Build my way" off)

include_directories("include")

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
	set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:MSVCRT.lib")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:MSVCRT.lib")
endif()

set(LIB_SRC
	src/Nooskewl_Engine/audio.cpp
	src/Nooskewl_Engine/brain.cpp
	src/Nooskewl_Engine/cpa.cpp
	src/Nooskewl_Engine/engine.cpp
	src/Nooskewl_Engine/font.cpp
	src/Nooskewl_Engine/image.cpp
	src/Nooskewl_Engine/load_dll.cpp
	src/Nooskewl_Engine/log.cpp
	src/Nooskewl_Engine/map.cpp
	src/Nooskewl_Engine/map_entity.cpp
	src/Nooskewl_Engine/map_logic.cpp
	src/Nooskewl_Engine/mml.cpp
	src/Nooskewl_Engine/module.cpp
	src/Nooskewl_Engine/player_brain.cpp
	src/Nooskewl_Engine/sample.cpp
	src/Nooskewl_Engine/snprintf.cpp
	src/Nooskewl_Engine/speech.cpp
	src/Nooskewl_Engine/sprite.cpp
	src/Nooskewl_Engine/tilemap.cpp
	src/Nooskewl_Engine/tween.cpp
	src/Nooskewl_Engine/util.cpp
	src/Nooskewl_Engine/vertex_cache.cpp
	src/Nooskewl_Engine/video.cpp
	src/Nooskewl_Engine/widgets.cpp
	src/Nooskewl_Engine/xml.cpp
)

set(RUNNER_SRC
	src/Nooskewl_Runner/runner.cpp
)

add_library(Nooskewl_Engine SHARED ${LIB_SRC})
set_target_properties(Nooskewl_Engine PROPERTIES COMPILE_FLAGS "/DNOOSKEWL_ENGINE_BUILD")
target_link_libraries(Nooskewl_Engine tgui3 SDL2_ttf SDL2 SDL2main glew32 zlib opengl32 d3d9 d3dx9 winmm)

add_executable(Nooskewl_Runner ${RUNNER_SRC})
target_link_libraries(Nooskewl_Runner Nooskewl_Engine)