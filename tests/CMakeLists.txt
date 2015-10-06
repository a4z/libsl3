


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${sl3_TEST_BINARY_DIR} )
SET (a4qlt3_TEST_DEFINES ${a4_TEST_DEFINES})
PREFIX_COMPILER_DEFINES(a4qlt3_TEST_DEFINES)
add_definitions(${a4qlt3_TEST_DEFINES})


if (USE_BOOSTTESTING_DYNLINK)
    FIND_PACKAGE( Boost COMPONENTS unit_test_framework  )
    set( myBoostLibs  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )    
endif(USE_BOOSTTESTING_DYNLINK)


# make test should also run the sample, so either put it here 
add_subdirectory(sample)
add_test( NAME sample COMMAND sl3_sample )


include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

#make big main reusable for all others
add_library(test_main OBJECT a4testing.cpp a4testing.hpp )

include( basics/CMakeLists.txt )
PREFIX_WITH( "basics/" basics_SRC ) 
ADD_EXECUTABLE( sl3_basics ${basics_SRC} $<TARGET_OBJECTS:test_main>)
TARGET_LINK_LIBRARIES( sl3_basics sl3 ${mysqlt3_LIBS} ${myBoostLibs} ${OPTION_GCOVLIB})
add_test( NAME basics_tests COMMAND sl3_basics )



