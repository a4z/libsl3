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

# Off, we do not want to annoy potential packager
option(WARN_ERROR "Thread warnings as errors. Default OFF" OFF)

set(CMAKE_CXX_STANDARD 11)


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
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang>>:-Wall -Wextra -pedantic -Wdeprecated -pthread>

    $<$<CXX_COMPILER_ID:GNU>: -Wduplicated-branches
                              -Wduplicated-cond
                              -Wlogical-op
                              -Wcast-align
                              -Wno-missing-field-initializers>

    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

if(WARN_ERROR)
  target_compile_options(_DefaultWarnings
    INTERFACE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang>>:-Werror>
      $<$<CXX_COMPILER_ID:MSVC>:/WX>
  )
endif(WARN_ERROR)

add_library(default::warnings ALIAS _DefaultWarnings)

# ----------
add_library(_DefaultFlags INTERFACE)

# TODO, old bad idea, remove
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (_DEBUG_FLAGS "DEBUG")
else()
    set (_DEBUG_FLAGS "RELEASE")
endif()

target_compile_options(_DefaultFlags
  INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang>>:-D${_DEBUG_FLAGS}>
    $<$<CXX_COMPILER_ID:MSVC>:/D${_DEBUG_FLAGS}>
)

target_link_libraries(_DefaultFlags
    INTERFACE default::libs default::warnings # default::coverage
)

add_library(default::flags ALIAS _DefaultFlags)
