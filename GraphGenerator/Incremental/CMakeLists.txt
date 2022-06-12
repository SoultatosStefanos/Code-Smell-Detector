set(FILES 
	Incremental.cpp
	Incremental.h
)
set(SUBDIRECTORIES Details)

foreach(VAR ${SUBDIRECTORIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${VAR})
	add_subdirectory(${VAR})
endforeach()

if(FILES)
	target_sources(${PROJECT_NAME} PUBLIC ${FILES})
endif()