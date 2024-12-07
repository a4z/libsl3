include_guard(GLOBAL)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  message(STATUS "-- Clang Coverage enabled, CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-instr-generate")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-instr-generate")
else()
  message(FATAL_ERROR "Clang Coverage is only supported with Clang")
endif()

set(COVERAGE_TOOL "llvm-profdata")

add_custom_target(coverage
  COMMAND ${CMAKE_COMMAND} -E env "PATH=$ENV{PATH}"
  ${CMAKE_CURRENT_LIST_DIR}/coverage-report-clang.sh --build-dir ${CMAKE_BINARY_DIR}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Generating coverage report using coverage-report-clang.sh"
)

add_custom_target(clean_coverage
    COMMAND  find . -name '*.profraw' -delete
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


