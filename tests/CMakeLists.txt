
add_subdirectory(btest)

add_subdirectory(doctest)



# make test should also run the sample, so either put it here 
add_subdirectory(sample)
add_test( NAME sample COMMAND sl3_sample )



