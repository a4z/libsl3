include_guard(GLOBAL)

# TODO , check that we are on GCC
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "This coverage implementation requires the GCC compiler")
endif()


message(STATUS "----- GNU Coverage enabled")
# TODO, add -fprofile-abs-path when compiler is gcc
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")



find_program(LCOV lcov)
find_program(GCOV gcov)
if(NOT GCOV)
    message(FATAL_ERROR "gcov tool not found for GCC version ${GCC_MAJOR_VERSION}")
endif()

set(COVERAGE_BRANCHES "--rc branch_coverage=1")
set(COVERAGE_WARNINGS "--ignore-errors gcov --ignore-errors unused --ignore-errors mismatch")
set(GENHTML_WARNINGS "")
if(APPLE)
    set(COVERAGE_WARNINGS "--ignore-errors gcov --ignore-errors inconsistent --ignore-errors range --ignore-errors empty")
    set(GENHTML_WARNINGS "--ignore-errors inconsistent --ignore-errors empty")
endif()

set(COVERAGE_TOOL "lcov")


separate_arguments(COVERAGE_BRANCHES)
separate_arguments(COVERAGE_WARNINGS)
separate_arguments(GENHTML_WARNINGS)
add_custom_target(coverage
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
    COMMAND lcov  --directory . --capture --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info ${COVERAGE_WARNINGS} ${COVERAGE_BRANCHES}
    COMMAND lcov --remove ${CMAKE_BINARY_DIR}/coverage/coverage.info '/usr/*' '*/tests/*' '*/vcpkg_installed/*' '${CMAKE_BINARY_DIR}/_deps/*'  '${CMAKE_SOURCE_DIR}/external/*' --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info.cleaned ${COVERAGE_WARNINGS} ${COVERAGE_BRANCHES}
    COMMAND genhtml --branch-coverage ${CMAKE_BINARY_DIR}/coverage/coverage.info.cleaned --output-directory ${CMAKE_BINARY_DIR}/coverage ${GENHTML_WARNINGS} ${COVERAGE_BRANCHES}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


add_custom_target(clean_coverage
    COMMAND find . -name '*.gcda' -delete
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
