
function(print_all_variables)
  message(STATUS "CMake Variables:")
  #get_cmake_property(_variableNames VARIABLES)
  get_cmake_property(_variableNames CACHE_VARIABLES)
  list(SORT _variableNames)
  foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endfunction()

# Call the function to print all variables
# print_all_variables()
