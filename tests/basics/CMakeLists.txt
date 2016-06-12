
set(sl3_CONFIG_TEST "${CMAKE_CURRENT_SOURCE_DIR}/basics/configtest.cpp")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/basics/configtest.cpp.in" 
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
    dbvaluetest.cpp
    dbvaluestest.cpp
    errtest.cpp
)




