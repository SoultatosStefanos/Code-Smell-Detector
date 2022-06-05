set(INTEGRATIONS 
	IncrementalTests
)
set(TESTS_LIBS gtest gmock)
set(SUBDIRECTORIES )

foreach(VAR ${SUBDIRECTORIES})
  add_subdirectory(${VAR})
endforeach()

foreach(VAR ${INTEGRATIONS})
	add_executable(${VAR} ${VAR}.cpp)
	target_link_libraries(${VAR} PRIVATE ${PROJECT_NAME} ${TESTS_LIBS})
endforeach()



