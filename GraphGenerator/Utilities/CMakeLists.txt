set(FILES 
	uobject_untyped.cpp
	uobject_untyped.h
	uvalue_untyped.cpp
	uvalue_untyped.h
)
set(SUBDIRECTORIES )

foreach(VAR ${SUBDIRECTORIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${VAR})
	add_subdirectory(${VAR})
endforeach()

if(FILES)
	target_sources(${PROJECT_NAME} PUBLIC ${FILES})
endif()