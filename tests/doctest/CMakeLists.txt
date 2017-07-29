

add_library(doctest_main OBJECT test_main.cpp )

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin/tests/)


set (COMMON_TEST_HEADERS
 ${CMAKE_CURRENT_SOURCE_DIR}/testing.hpp
 ${CMAKE_CURRENT_SOURCE_DIR}/doctest.h
)


add_subdirectory(commands)
add_subdirectory(database)
add_subdirectory(dataset)
add_subdirectory(rowcallback)
add_subdirectory(typenames)
add_subdirectory(value)
add_subdirectory(version)

