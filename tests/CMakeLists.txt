
# add_library(doctest_main OBJECT test_main.cpp )

include(testing)

add_subdirectory(commands)
add_subdirectory(database)
add_subdirectory(dataset)
add_subdirectory(dbvalue)
add_subdirectory(rowcallback)
add_subdirectory(typenames)
add_subdirectory(value)
add_subdirectory(version)



# make test should also run the sample, so either put it here
add_subdirectory(sample)

if (NOT CODECOVERAGE)

  add_test( NAME sample COMMAND sl3_sample )
  add_test( NAME sample1 COMMAND sl3_sample1 )
  add_test( NAME sample2 COMMAND sl3_sample2 )
  add_test( NAME sample3 COMMAND sl3_sample3 )
  add_test( NAME sample4 COMMAND sl3_sample4 )
  add_test( NAME sample5 COMMAND sl3_sample5 )
  add_test( NAME sample6 COMMAND sl3_sample6 )

endif(NOT CODECOVERAGE)




