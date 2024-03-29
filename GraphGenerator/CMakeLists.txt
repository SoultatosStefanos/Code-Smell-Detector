cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(GraphGenerator CXX)

if(MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -pedantic)
endif()

find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
include(${wxWidgets_USE_FILE})

add_library(${PROJECT_NAME})

add_definitions("-fno-rtti -D_GNU_SOURCE -D_DEBUG -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")

set(CLANG_LIBS libclang-cpp.so)
set(JSONCPP_LIBS libjsoncpp.so)

# Try to guess the clang include paths (linux)
set(CLANG_INCLUDE_DIR "/usr/lib/llvm-15/lib/clang/15.0.2/include" CACHE STRING "The directory path to the .h files for clang.")
set(INCREMENTAL_GENERATION TRUE CACHE BOOL "Wether to mine incrementally after cancel and re-run.")
set(GUI TRUE CACHE BOOL "Wether to render the gui progress bar.")

message("CLANG_INCLUDE_DIR: " ${CLANG_INCLUDE_DIR})
message("INCREMENTAL_GENERATION: " ${INCREMENTAL_GENERATION})
message("GUI: " ${GUI})

add_definitions(-DCLANG_INCLUDE_DIR="${CLANG_INCLUDE_DIR}")
target_compile_definitions(${PROJECT_NAME} PUBLIC INCREMENTAL_GENERATION=${INCREMENTAL_GENERATION})
target_compile_definitions(${PROJECT_NAME} PUBLIC GUI=${GUI})

target_link_libraries(${PROJECT_NAME} PRIVATE ${CLANG_LIBS} ${JSONCPP_LIBS} ${wxWidgets_LIBRARIES})

set(SUBDIRECTORIES
	DependenciesMining
	FileSystem
	Graph
	GraphGeneration
	GraphToJson
	Gui
	Incremental
	Main
	Messages
	SourceLoader
	Utilities
)

foreach(VAR ${SUBDIRECTORIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${VAR})
	add_subdirectory(${VAR})
endforeach()
