cmake_minimum_required(VERSION 2.8)
project(Nooskewl_Engine)

option(LOAD_FROM_FILESYSTEM "Load from regular files" OFF)

include_directories("include")

set(USER_INCLUDE_PATH "" CACHE FILEPATH "Any extra include directories you want")
set(USER_LIBRARY_PATH "" CACHE FILEPATH "Any extra library directories you want")

include_directories(${USER_INCLUDE_PATH})
link_directories(${USER_LIBRARY_PATH})

set(LIB_SRC
	src/Nooskewl_Engine/a_star.cpp
	src/Nooskewl_Engine/brain.cpp
	src/Nooskewl_Engine/cpa.cpp
	src/Nooskewl_Engine/engine.cpp
	src/Nooskewl_Engine/font.cpp
	src/Nooskewl_Engine/gui.cpp
	src/Nooskewl_Engine/image.cpp
	src/Nooskewl_Engine/internal.cpp
	src/Nooskewl_Engine/inventory.cpp
	src/Nooskewl_Engine/item.cpp
	src/Nooskewl_Engine/map.cpp
	src/Nooskewl_Engine/map_entity.cpp
	src/Nooskewl_Engine/map_logic.cpp
	src/Nooskewl_Engine/mml.cpp
	src/Nooskewl_Engine/player_brain.cpp
	src/Nooskewl_Engine/sample.cpp
	src/Nooskewl_Engine/shader.cpp
	src/Nooskewl_Engine/speech.cpp
	src/Nooskewl_Engine/sprite.cpp
	src/Nooskewl_Engine/stats.cpp
	src/Nooskewl_Engine/tilemap.cpp
	src/Nooskewl_Engine/tokenizer.cpp
	src/Nooskewl_Engine/translation.cpp
	src/Nooskewl_Engine/tween.cpp
	src/Nooskewl_Engine/utf8.cpp
	src/Nooskewl_Engine/vertex_cache.cpp
	src/Nooskewl_Engine/widgets.cpp
	src/Nooskewl_Engine/xml.cpp
)

if(APPLE)
	list(APPEND LIB_SRC src/Nooskewl_Engine/macosx.M)
endif()

set(RUNNER_SRC
	src/Nooskewl_Runner/runner.cpp
)

add_library(Nooskewl_Engine SHARED ${LIB_SRC})

add_executable(Nooskewl_Runner ${RUNNER_SRC})

if(WIN32)
	if(CMAKE_BUILD_TYPE MATCHES "Release")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	endif()
	set(FLAGS "${FLAGS} /MP /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x501 /DNOOSKEWL_ENGINE_WINDOWS /DWITH_SDL")
	if(LOAD_FROM_FILESYSTEM)
		set(FLAGS "${FLAGS} /DLOAD_FROM_FILESYSTEM=1")
	endif()
	set_target_properties(Nooskewl_Engine PROPERTIES COMPILE_FLAGS "/DNOOSKEWL_ENGINE_BUILD ${FLAGS}")
	set_target_properties(Nooskewl_Runner PROPERTIES COMPILE_FLAGS ${FLAGS})
	target_link_libraries(Nooskewl_Engine tgui3 SDL2_ttf SDL2 SDL2main glew32 zlib opengl32 d3d9 d3dx9 winmm)
	target_link_libraries(Nooskewl_Runner Nooskewl_Engine "${CMAKE_SOURCE_DIR}/windows_icon.res")
else()
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_SDL")
	if(LOAD_FROM_FILESYSTEM)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOAD_FROM_FILESYSTEM=1")
	endif()
	if(APPLE)
		set_target_properties(Nooskewl_Engine PROPERTIES MACOSX_RPATH on)
		set_target_properties(Nooskewl_Engine PROPERTIES BUILD_WITH_INSTALL_RPATH on)

		set_target_properties(Nooskewl_Engine PROPERTIES LINK_FLAGS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework AudioUnit -framework ForceFeedback -framework Carbon")
		set_target_properties(Nooskewl_Runner PROPERTIES BUILD_WITH_INSTALL_RPATH on)
		set_target_properties(Nooskewl_Runner PROPERTIES INSTALL_RPATH "@executable_path/")
		target_link_libraries(Nooskewl_Engine tgui3 GLEW SDL2 SDL2_ttf freetype z)
	else()
		target_link_libraries(Nooskewl_Engine tgui3 SDL2_ttf SDL2 GLEW Xcursor z)
		set_target_properties(Nooskewl_Runner PROPERTIES LINK_FLAGS "-Wl,-rpath=.")
	endif()
	target_link_libraries(Nooskewl_Runner Nooskewl_Engine)
endif()
