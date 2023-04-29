cmake_minimum_required(VERSION 3.25)

set(sl3_MAJOR_VERSION 1)
set(sl3_MINOR_VERSION 2)
set(sl3_PATCH_VERSION 0)
set(sl3_VERSION ${sl3_MAJOR_VERSION}.${sl3_MINOR_VERSION}.${sl3_PATCH_VERSION})

project(libsl3
    VERSION ${sl3_VERSION}
    LANGUAGES C CXX
)


if(PROJECT_IS_TOP_LEVEL)
    # make git ignore the build directory
    file(WRITE ${CMAKE_BINARY_DIR}/.gitignore "*")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

LIST(PREPEND CMAKE_MODULE_PATH "${libsl3_SOURCE_DIR}/CMake")
include(conan1_setup)

include( macros )
include( setup_compiler )

include( setup_doc )

set(sl3_CONFIG_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/sl3/config.hpp")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/config.in" "${sl3_CONFIG_HEADER}")



################################################################################

SET ( sl3_HDR
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
SET ( sl3_SRCHDR
  src/sl3/connection.hpp

)
#-------------------------------------------------------------------------------
SET ( sl3_SRC

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


# TODO check win build

# TODO , legacy

# TODO, if on, forward to conan usage?
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

add_library( sl3 ${SQLITE3_FILES} ${sl3_SRC} ${sl3_SRCHDR} ${sl3_HDR} )
set(sl3_install_targets ${sl3_install_targets} sl3)
target_link_libraries(sl3
    PRIVATE default::flags
    PUBLIC CONAN_PKG::sqlite3
)
target_include_directories(sl3 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
)

if(BUILD_SHARED_LIBS)
    set_target_properties(sl3 PROPERTIES
        VERSION ${sl3_VERSION}
        SOVERSION ${sl3_MAJOR_VERSION}.${sl3_MINOR_VERSION}
        POSITION_INDEPENDENT_CODE ON
    )
endif(BUILD_SHARED_LIBS)

enable_testing()
add_subdirectory(tests)

#optional enable personal test/trial dir
include(CMakeLocalOpts.cmake OPTIONAL)
if(a4qlt3_LOCAL_TRIALS)
    add_subdirectory(trials)
endif(a4qlt3_LOCAL_TRIALS)

#only add install target if this is the top level project
if( ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    INSTALL(FILES ${sl3_HDR} DESTINATION include/sl3)
    INSTALL(TARGETS ${sl3_install_targets}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib${LIB_SUFFIX}
      ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
endif( ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})



