# not with cmake 2.8
#include_guard(GLOBAL)


if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")

    message(STATUS "set up defaults for gcc")

    if (NOT CMAKE_C_FLAGS)
    SET (CMAKE_C_FLAGS "-Wall -Wextra -pedantic -std=c11 -pipe")
    endif(NOT CMAKE_C_FLAGS)

    SET (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING
    "Flags used by the compiler during all build types."
    FORCE )

    if (NOT CMAKE_CXX_FLAGS)
        if (MINGW)
            SET (CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++11 -pipe")
        else(MINGW)
            SET (CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pedantic-errors -std=c++11 -pipe -pthread")
        endif(MINGW)
    endif(NOT CMAKE_CXX_FLAGS)


    SET (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING
    "Flags used by the compiler during all build types."
    FORCE )

    if (CODECOVERAGE)
        add_definitions(--coverage -fprofile-arcs -ftest-coverage)
        set (OPTION_GCOVLIB gcov CACHE STRING INTERNAL)
    endif(CODECOVERAGE)


elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")

      message(STATUS "set up defaults for clang")
      
      #set(MYCLANGWARNINGS "-Weverything")
      set(MYCLANGWARNINGS "-Wall -Wextra -pedantic")

      if (NOT CMAKE_C_FLAGS)
        SET (CMAKE_C_FLAGS "-std=c99 ${MYCLANGWARNINGS}")
      endif(NOT CMAKE_C_FLAGS)

      SET (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING
        "Flags used by the compiler during all build types."
        FORCE )

      if (NOT CMAKE_CXX_FLAGS)
        SET (CMAKE_CXX_FLAGS "-std=c++11 -pipe -pthread ${MYCLANGWARNINGS}")
      endif(NOT CMAKE_CXX_FLAGS)

      SET (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING
        "Flags used by the compiler during all build types."
        FORCE )


elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")

    message(STATUS "set up defaults for Apple/Clang")
      
    #set(MYCLANGWARNINGS "-Weverything")
    set(MYCLANGWARNINGS "-Wall -Wextra -pedantic")

    if (NOT CMAKE_C_FLAGS)
    SET (CMAKE_C_FLAGS "-std=c99 ${MYCLANGWARNINGS}")
    endif(NOT CMAKE_C_FLAGS)

    SET (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING
    "Flags used by the compiler during all build types."
    FORCE )

    if (NOT CMAKE_CXX_FLAGS)
    SET (CMAKE_CXX_FLAGS "-std=c++11 -pipe -pthread ${MYCLANGWARNINGS}")
    endif(NOT CMAKE_CXX_FLAGS)

    SET (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING
    "Flags used by the compiler during all build types."
    FORCE )

elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    message(STATUS "using defaults for MSVC")

else()
    message(WARNING "no default flags for compiler ${CMAKE_CXX_COMPILER_ID}, 
    you might want to provide your own.")

endif()

