# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CXX_STANDARD 23)

# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS OFF)
# set(CMAKE_C_STANDARD 17)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_BIN ccache)
if(CCACHE_BIN)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_BIN})
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_BIN})
  message("ccache binary foud, ccache enabled")
endif()


if(DEFINED TOOLCHAIN_INCLUDES)
  foreach(TOOLCHAIN_INCLUDE ${TOOLCHAIN_INCLUDES})
    message(STATUS "Include toolchain : ${TOOLCHAIN_INCLUDE}")
    include(${TOOLCHAIN_INCLUDE})
  endforeach()
endif()

message(STATUS "-- Done with the toolchain router")

# TODO, do I want to allow env variables also?
# probably not since that might confuse
# if(DEFINED ENV{TOOLCHAIN_ADDONS})
#   foreach(TC $ENV{TOOLCHAIN_ADDONS})
#     message(STATUS "Add toolchain addon (env): ${TC}")
#     include(${TC})
#   endforeach()
# endif()



