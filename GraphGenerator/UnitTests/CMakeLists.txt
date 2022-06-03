set(FILES 
  Tests.cpp
)

set(TESTS UnitTests)
set(TESTS_LIBS gtest gmock)

add_executable(${TESTS} ${FILES})
target_link_libraries(${TESTS} PRIVATE ${PROJECT_NAME} ${TESTS_LIBS})

set(SUBDIRECTORIES )

foreach(VAR ${SUBDIRECTORIES})
  add_subdirectory(${VAR})
endforeach()