
SET (TESTNAME val)

SET( test_SRC
    blobval.cpp
   intval.cpp
    realval.cpp
    textval.cpp
    valuetest.cpp
    eqorder.hpp
)



ADD_EXECUTABLE( sl3_${TESTNAME} ${test_SRC} $<TARGET_OBJECTS:test_main>)
TARGET_LINK_LIBRARIES( sl3_${TESTNAME} sl3 ${mysqlt3_LIBS} ${myBoostLibs} ${OPTION_GCOVLIB})
add_test( NAME ${TESTNAME}_tests COMMAND sl3_${TESTNAME} )




