include_guard(GLOBAL)


# this is obviously a requirements, tests...
# TODO , find that file, in test and tests


include(FetchContent)

FetchContent_Declare(
        DocTest
        SYSTEM
        GIT_REPOSITORY "https://github.com/onqtam/doctest"
        GIT_TAG "v2.4.11"
)
FetchContent_MakeAvailable(DocTest)

set(CON_DOCTEST doctest)

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
    target_link_libraries(sl3test-${NAME} sl3 ${CON_DOCTEST} default::flags)

    if(NOT D_TEST_TIMEOUT)
        set(D_TEST_TIMEOUT 3)
    endif()
    if(NOT D_TEST_WILL_FAIL)
        set(D_TEST_WILL_FAIL OFF)
    endif()

    # use naming what we had for now
    add_test(NAME sl3test_${NAME} COMMAND sl3test-${NAME})

    set_tests_properties(sl3test_${NAME}
        PROPERTIES
            TIMEOUT ${D_TEST_TIMEOUT}
            WILL_FAIL ${D_TEST_WILL_FAIL}
    )

endfunction(add_doctest)




