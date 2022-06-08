cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(GraphGenerator CXX)

if (MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(
		-Wall -Wextra -pedantic
		-I/usr/lib/gcc/x86_64-linux-gnu/11/include
)
endif()

find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
include(${wxWidgets_USE_FILE})

add_library(${PROJECT_NAME} )

add_definitions("-fno-rtti -D_GNU_SOURCE -D_DEBUG -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")

target_compile_definitions(${PROJECT_NAME} PUBLIC INCREMENTAL_GENERATION=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC GUI=1)

set(CLANG_LIBS libclang-cpp.so)
set(JSONCPP_LIBS libjsoncpp.so)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CLANG_LIBS} ${JSONCPP_LIBS} ${wxWidgets_LIBRARIES})

set(SUBDIRECTORIES 
	DependenciesMining
	Graph
	GraphGeneration
	GraphToJson
	Gui
	Incremental
	Main
	SourceLoader
	Tests
	ThirdParty
	Utilities
)

foreach(VAR ${SUBDIRECTORIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${VAR})
	add_subdirectory(${VAR})
endforeach()
