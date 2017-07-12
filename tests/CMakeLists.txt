
# add_subdirectory(btest)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin/tests/)

add_subdirectory(doctest)



# make test should also run the sample, so either put it here 
add_subdirectory(sample)
add_test( NAME sample COMMAND sl3_sample )
add_test( NAME sample1 COMMAND sl3_sample1 )
add_test( NAME sample2 COMMAND sl3_sample2 )
add_test( NAME sample3 COMMAND sl3_sample3 )
add_test( NAME sample4 COMMAND sl3_sample4 )



