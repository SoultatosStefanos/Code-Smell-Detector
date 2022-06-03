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

add_library(${PROJECT_NAME} )

add_definitions("-fno-rtti -D_GNU_SOURCE -D_DEBUG -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")

#target_compile_definitions(${PROJECT_NAME} PRIVATE INCREMENTAL_GENERATION=1)

set(CLANG_LIBS libclang-cpp.so)
set(JSONCPP_LIBS libjsoncpp.so)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CLANG_LIBS} ${JSONCPP_LIBS})

set(SUBDIRECTORIES 
	DependenciesMining
	Graph
	GraphGeneration
	GraphToJson
	Incremental
	Main
	SourceLoader
	ThirdParty
	UnitTests
	Utilities
)

foreach(VAR ${SUBDIRECTORIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${VAR})
	add_subdirectory(${VAR})
endforeach()
