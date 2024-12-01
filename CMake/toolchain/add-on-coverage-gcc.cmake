include_guard(GLOBAL)
# that might not be the elegant way to do it, but it works mosts simple
# alternative would be finding a way to inject coverage into every target
# and that would require much more

  message(STATUS "----- GNU Coverage enabled")
  # TODO, add -fprofile-abs-path when compiler is gcc
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")



