cmake_minimum_required(VERSION 3.25)


set(sl3_MAJOR_VERSION 1)
set(sl3_MINOR_VERSION 2)

# sqlite major is always 3, no need to use that here
# set(internal_SQLITE_MAJOR_V 3)
set(internal_SQLITE_MINOR_V 41)
set(internal_SQLITE_PATCH_V  2)

# sqlite uses (X*1000000 + Y*1000 + Z),
# but minor patch used since major is always 3
math(EXPR sl3_PATCH_VERSION "${internal_SQLITE_MINOR_V}*1000 + ${internal_SQLITE_PATCH_V}")

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

include(CTest)
include( macros )
include( setup_compiler )
SET (USE_INTERNAL_SQLITE3 TRUE CACHE BOOL
    "use buildin sqlite3 ON, use system sqlite3 header/lib, OFF" )

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

add_library( sl3 ${SQLITE3_FILES} ${sl3_SRC} ${sl3_SRCHDR} ${sl3_HDR} )
target_include_directories(sl3 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
)
target_link_libraries(sl3
    PRIVATE default::flags
)

if (USE_INTERNAL_SQLITE3)
    include( sqlite/setup_sqlite3.cmake )
else(USE_INTERNAL_SQLITE3)
    find_package(SQLite3 REQUIRED)
    target_link_libraries(sl3 PUBLIC ${SQLite3_LIBRARIES})
    target_include_directories(sl3 PRIVATE ${SQLite3_INCLUDE_DIRS})
endif(USE_INTERNAL_SQLITE3)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

set(sl3_install_targets ${sl3_install_targets} sl3)

if(BUILD_SHARED_LIBS)
    set_target_properties(sl3 PROPERTIES
        VERSION ${sl3_VERSION}
        SOVERSION ${sl3_MAJOR_VERSION}.${sl3_MINOR_VERSION}
        POSITION_INDEPENDENT_CODE ON
    )
endif(BUILD_SHARED_LIBS)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif(BUILD_TESTING)

#optional enable personal test/trial dir
include(CMakeLocalOpts.cmake OPTIONAL)


#only add install target if this is the top level project
if( ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    INSTALL(FILES ${sl3_HDR} DESTINATION include/sl3)
    INSTALL(FILES include/sl3.hpp DESTINATION include/sl3)
    INSTALL(TARGETS ${sl3_install_targets}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib${LIB_SUFFIX}
      ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
endif( ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})



