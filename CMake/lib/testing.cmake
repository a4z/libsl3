include_guard(GLOBAL)


find_package(doctest CONFIG REQUIRED)

set(CON_DOCTEST doctest::doctest)

add_library(doctest_main OBJECT ${PROJECT_SOURCE_DIR}/tests/test_main.cpp)

target_link_libraries(doctest_main PUBLIC ${CON_DOCTEST})

function (add_doctest NAME)

    set(option_args WILL_FAIL)
    set(value_args TIMEOUT)
    set(list_args SOURCES)

    cmake_parse_arguments(D_TEST
        "${option_args}" "${value_args}" "${list_args}"
        ${ARGN}
    )

    add_executable(sl3test-${NAME} ${D_TEST_SOURCES} $<TARGET_OBJECTS:doctest_main>)
    # use naming what we had for now
    target_link_libraries(sl3test-${NAME} PRIVATE sl3 ${CON_DOCTEST}
       # a4z::commonCompilerWarnings
    # "$<BUILD_INTERFACE:a4z:commonCompilerWarnings>"
    )
    target_link_libraries(sl3test-${NAME} PRIVATE $<BUILD_INTERFACE:a4z::commonCompilerWarnings>)

    if(NOT D_TEST_TIMEOUT)
        set(D_TEST_TIMEOUT 3)
    endif()
    if(NOT D_TEST_WILL_FAIL)
        set(D_TEST_WILL_FAIL OFF)
    endif()

    # use naming what we had for now
    add_test(NAME sl3test_${NAME} COMMAND sl3test-${NAME})
    # with clang, we use this to get coverage
    if(COVERAGE_TOOL STREQUAL "llvm-profdata")
        # important, the profile file shall have the same name as the test binary, not the test name (cmake test name)
        set_tests_properties(sl3test_${NAME} PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=sl3test-${NAME}.profraw")
    endif()


    set_tests_properties(sl3test_${NAME}
        PROPERTIES
            TIMEOUT ${D_TEST_TIMEOUT}
            WILL_FAIL ${D_TEST_WILL_FAIL}
    )

endfunction(add_doctest)




