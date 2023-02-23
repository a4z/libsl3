include_guard(GLOBAL)

# enable ccache only if it is there
find_program(CCACHE_BIN ccache)
if(CCACHE_BIN)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_BIN})
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_BIN})
  message("ccache binary foud, ccache enabled")
endif()

if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE )
endif(NOT CMAKE_BUILD_TYPE)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)


# set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)

# ----------
find_package (Threads)
add_library(_DefaultLibs INTERFACE)
target_link_libraries(_DefaultLibs
    INTERFACE Threads::Threads
)
add_library(default::libs ALIAS _DefaultLibs)
# ----------
add_library(_DefaultWarnings INTERFACE)
target_compile_options(_DefaultWarnings
  INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang>>:
    -Wall -Wextra -pedantic
    -Wdeprecated
    -Wformat-security
    -Wimplicit-fallthrough
    -Wmissing-braces
    -Wmissing-field-initializers
    -Wnon-virtual-dtor
    -Wnull-dereference
    -Wold-style-cast
#    -Wpadded interesting but not possible in this context, psys yes
    -Wparentheses
    -Wpointer-arith
    -Wredundant-decls
    -Wreturn-type
    -Wsequence-point
    -Wshadow
    -Wswitch
    -Wuninitialized
    -Wunused-function
    -Wunused-variable
    -Wunused-variable
    -Wconversion
    -Wsign-conversion
    -Wsign-promo
    -Wfloat-conversion
    -pthread>

    $<$<CXX_COMPILER_ID:GNU>: -Wduplicated-branches
                              -Wduplicated-cond
                              -Wlogical-op
                              -Wcast-align
                              -Wno-missing-field-initializers>

    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_library(default::warnings ALIAS _DefaultWarnings)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)


# ----------
add_library(_DefaultFlags INTERFACE)

target_link_libraries(_DefaultFlags
    INTERFACE default::libs default::warnings # default::coverage
)

# TODO: Coverage, check https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake

add_library(default::flags ALIAS _DefaultFlags)


#[[
-Wdeprecated-copy (C++ and Objective-C++ only)

    Warn that the implicit declaration of a copy constructor or copy assignment operator is deprecated if the class has a user-provided copy constructor or copy assignment operator, in C++11 and up. This warning is enabled by -Wextra. With -Wdeprecated-copy-dtor, also deprecate if the class has a user-provided destructor.
]]
