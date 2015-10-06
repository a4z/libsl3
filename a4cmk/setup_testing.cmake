

#requires a tests subdir, creates ${PROJECT_NAME}_BUILD_TESTS and sets a4_TEST_DEFINES   
if(NOT ALL_A4_TESTING_DISABLED)
    
    option(${PROJECT_NAME}_BUILD_TESTS "build tests" ON)
    
    if (${PROJECT_NAME}_BUILD_TESTS)
    
        ENABLE_TESTING()
        
        if(NOT HAVE_USE_BOOSTTESTING_DYNLINK_OPTION )
            option( USE_BOOSTTESTING_DYNLINK "link against libboost_testing library(ON), use header only(OFF)" OFF)    
        endif(NOT HAVE_USE_BOOSTTESTING_DYNLINK_OPTION )
        
        if(NOT USE_BOOSTTESTING_DYNLINK)
            set (a4_TEST_DEFINES BOOST_UNIT_TEST_FRAMEWORK_NO_LIB BOOST_TEST_NO_LIB)
        endif(NOT USE_BOOSTTESTING_DYNLINK)
        
        # check if comes as bundle/subproject, if use subdir for test output
        if(${CMAKE_BINARY_DIR} STREQUAL ${PROJECT_BINARY_DIR})
            set(${PROJECT_NAME}_TEST_BINARY_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)

        else(${CMAKE_BINARY_DIR} STREQUAL ${PROJECT_BINARY_DIR})
            set(${PROJECT_NAME}_TEST_BINARY_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}_tests)

        endif(${CMAKE_BINARY_DIR} STREQUAL ${PROJECT_BINARY_DIR})

        message (STATUS "** set " ${PROJECT_NAME}_TEST_BINARY_DIR " to " ${${PROJECT_NAME}_TEST_BINARY_DIR} )
             
        
        add_subdirectory(tests)        
        
    endif (${PROJECT_NAME}_BUILD_TESTS)
endif(NOT ALL_A4_TESTING_DISABLED)
