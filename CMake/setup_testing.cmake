

#requires a tests subdir, creates ${PROJECT_NAME}_BUILD_TESTS and sets a4_TEST_DEFINES   
if(NOT ALL_A4_TESTING_DISABLED)

    if (DEFINED ${PROJECT_NAME}_BUILD_TESTS)
      option(${PROJECT_NAME}_BUILD_TESTS 
        "build tests" 
        ${PROJECT_NAME}_BUILD_TESTS
      )
    else (DEFINED ${PROJECT_NAME}_BUILD_TESTS)
      option(${PROJECT_NAME}_BUILD_TESTS "build tests" ON)
    endif(DEFINED ${PROJECT_NAME}_BUILD_TESTS)
    
    if (${PROJECT_NAME}_BUILD_TESTS)
    
        ENABLE_TESTING()
        
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
