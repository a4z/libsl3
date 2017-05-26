
set(sl3_CONFIG_TEST "${CMAKE_CURRENT_SOURCE_DIR}/configtest.cpp")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/configtest.cpp.in" 
    "${sl3_CONFIG_TEST}")


SET( basics_SRC
    configtest.cpp
    datasettest.cpp
    dbtest.cpp
    callbacktest.cpp
    typetest.cpp
    commandtest.cpp
    testdb.cpp
    testdb.hpp
    errtest.cpp
)

ADD_EXECUTABLE( sl3_basics ${basics_SRC} $<TARGET_OBJECTS:test_main>)
TARGET_LINK_LIBRARIES( sl3_basics sl3 ${mysqlt3_LIBS} ${myBoostLibs} ${OPTION_GCOVLIB})
add_test( NAME basics_tests COMMAND sl3_basics )




