cmake_minimum_required(VERSION 3.31)


set(sl3_MAJOR_VERSION 1)
set(sl3_MINOR_VERSION 3)

# sqlite major is always 3, no need to use that here
# set(internal_SQLITE_MAJOR_V 3)
set(internal_SQLITE_MINOR_V 51)
set(internal_SQLITE_PATCH_V  3)

# sqlite uses (X*1000000 + Y*1000 + Z),
# but minor patch used since major is always 3
math(EXPR sl3_PATCH_VERSION "${internal_SQLITE_MINOR_V}*1000 + ${internal_SQLITE_PATCH_V}")

set(sl3_VERSION ${sl3_MAJOR_VERSION}.${sl3_MINOR_VERSION}.${sl3_PATCH_VERSION})

project(libsl3
    VERSION ${sl3_VERSION}
    LANGUAGES C CXX
)



# done differently now
list(PREPEND CMAKE_MODULE_PATH "${libsl3_SOURCE_DIR}/cmake")

#optional enable personal test/trial dir
include(CMakeLocalOpts.cmake OPTIONAL)


option(sl3_USE_INTERNAL_SQLITE3 "use build-in sqlite3 ON, use system sqlite3 header/lib, OFF" OFF)
option(sl3_USE_COMMON_COMPILER_WARNINGS "Use common compiler warning preset target" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

if (sl3_USE_COMMON_COMPILER_WARNINGS)
    find_package(commonCompilerWarnings CONFIG REQUIRED)
    set(LIBWARNINGS a4z::commonCompilerWarnings)
else()
    add_library(all_warnings_off INTERFACE)
    set(LIBWARNINGS all_warnings_off)
    message(STATUS "sl3_USE_COMMON_COMPILER_WARNINGS is OFF")
endif()


set(sl3_CONFIG_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/sl3/config.hpp")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/config.in" "${sl3_CONFIG_HEADER}")

################################################################################

set(sl3_PUBLIC_HEADERS
    include/sl3.hpp
    include/sl3/columns.hpp
    include/sl3/command.hpp
    include/sl3/config.hpp
    include/sl3/container.hpp
    include/sl3/database.hpp
    include/sl3/dataset.hpp
    include/sl3/dbvalue.hpp
    include/sl3/dbvalues.hpp
    include/sl3/error.hpp
    include/sl3/rowcallback.hpp
    include/sl3/types.hpp
    include/sl3/value.hpp
)
#-------------------------------------------------------------------------------
set(sl3_PRIVATE_HEADERS
    src/sl3/connection.hpp
)
#-------------------------------------------------------------------------------
set(sl3_SRC
    src/sl3/columns.cpp
    src/sl3/config.cpp
    src/sl3/command.cpp
    src/sl3/database.cpp
    src/sl3/dataset.cpp
    src/sl3/dbvalue.cpp
    src/sl3/dbvalues.cpp
    src/sl3/error.cpp
    src/sl3/rowcallback.cpp
    src/sl3/types.cpp
    src/sl3/value.cpp
)
################################################################################

add_library(sl3)
target_sources(sl3
    PRIVATE
        ${sl3_SRC}
        ${sl3_PRIVATE_HEADERS}
    PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES
            ${sl3_PUBLIC_HEADERS}
)

target_link_libraries(sl3 PRIVATE $<BUILD_INTERFACE:${LIBWARNINGS}>)

include( lib/find_sqlite )
target_link_libraries(sl3 PUBLIC ${SQLITE_LINK_NAME})

set(sl3_install_targets sl3)

if(BUILD_SHARED_LIBS)
    set_target_properties(sl3 PROPERTIES
        VERSION ${sl3_VERSION}
        SOVERSION ${sl3_MAJOR_VERSION}.${sl3_MINOR_VERSION}
        POSITION_INDEPENDENT_CODE ON
    )
endif()

if(sl3_BUILD_TESTING)
    add_subdirectory(tests)
endif()

include(lib/install)
include(lib/doxydoc)

# include(CMakePrintSystemInformation)
# include(CMakePrintHelpers)
# cmake_print_variables(CMAKE_SYSTEM_AND_CXX_COMPILER_INFO_FILE)
