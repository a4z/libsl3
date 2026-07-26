
include_guard(GLOBAL)

message(STATUS "-- Activate toolchain gcc")

set (GCC_WANTED_VERSION 16)

if(DEFINED GCC_WANTED_VERSION)
    message(STATUS "GCC_WANTED_VERSION is set to: ${GCC_WANTED_VERSION}")
    set (GCC_VERSION_POSTFIX -${GCC_WANTED_VERSION})

endif()


# Find gcc and g++ with the wanted version
find_program(GCC_BIN NAMES gcc${GCC_VERSION_POSTFIX})
find_program(GXX_BIN NAMES g++${GCC_VERSION_POSTFIX})

if(GCC_BIN)
    message(STATUS "Found gcc${GCC_VERSION_POSTFIX}: ${GCC_BIN}")
else()
    message(WARNING "gcc${GCC_VERSION_POSTFIX} not found in PATH")
endif()

if(GXX_BIN)
    message(STATUS "Found g++${GCC_VERSION_POSTFIX}: ${GXX_BIN}")
else()
    message(WARNING "g++${GCC_VERSION_POSTFIX} not found in PATH")
endif()

set(CMAKE_C_COMPILER ${GCC_BIN})
set(CMAKE_CXX_COMPILER ${GXX_BIN})

