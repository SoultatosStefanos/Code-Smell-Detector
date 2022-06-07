set(FILES Main.cpp)
set(SUBDIRECTORIES )
set(EXECUTABLE ArchitectureMining)

foreach(VAR ${SUBDIRECTORIES})
	add_subdirectory(${VAR})
endforeach()

if(FILES)
	add_executable(${EXECUTABLE} ${FILES})
	target_link_libraries(${EXECUTABLE} PUBLIC ${PROJECT_NAME})
endif()