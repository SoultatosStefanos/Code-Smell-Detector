set(FILES 
	IncrementalTests.cpp
	Tests.cpp
)
set(TESTS IntegrationTests)
set(TESTS_LIBS gtest gmock)
set(SUBDIRECTORIES )

foreach(VAR ${SUBDIRECTORIES})
  add_subdirectory(${VAR})
endforeach()

if(FILES)
	add_executable(${TESTS} ${FILES})
	target_link_libraries(${TESTS} PRIVATE ${PROJECT_NAME} ${TESTS_LIBS})
endif()


