
SET (TESTNAME falilure)

SET( test_SRC
    failures.cpp
)



ADD_EXECUTABLE( sl3_${TESTNAME} ${test_SRC} $<TARGET_OBJECTS:test_main>)
TARGET_LINK_LIBRARIES( sl3_${TESTNAME} sl3 ${mysqlt3_LIBS} ${myBoostLibs} ${OPTION_GCOVLIB})
add_test( NAME ${TESTNAME}_tests COMMAND sl3_${TESTNAME} )




