
cmake_minimum_required(VERSION 2.8)

project(libsl3 C CXX) 


set(sl3_MAJOR_VERSION 1)
set(sl3_MINOR_VERSION 0)

# sqlite major is always 3, no need to use that here
# set(internal_SQLITE_MAJOR_V 3)
set(internal_SQLITE_MINOR_V 13)
set(internal_SQLITE_PATCH_V  0)

# sqlite uses (X*1000000 + Y*1000 + Z),
# but minor patch used since major is always 3
math(EXPR sl3_PATCH_VERSION "${internal_SQLITE_MINOR_V}*1000 + ${internal_SQLITE_PATCH_V}")

set(sl3_VERSION ${sl3_MAJOR_VERSION}.${sl3_MINOR_VERSION}.${sl3_PATCH_VERSION})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include necessary submodules
set(CMAKE_MODULE_PATH
  "${libsl3_SOURCE_DIR}/CMake"
)


include( macros )
include( setup_compiler )



if(NOT CMAKE_BUILD_TYPE)

  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE )
endif(NOT CMAKE_BUILD_TYPE)


if (CMAKE_BUILD_TYPE STREQUAL "Debug") 
    set (_DEFAULT_FLAGS "DEBUG")
else()
    set (_DEFAULT_FLAGS "RELEASE")
endif()


PREFIX_COMPILER_DEFINES(_DEFAULT_FLAGS)
add_definitions(${_DEFAULT_FLAGS})

unset(_DEFAULT_FLAGS)

include( sqlite/setup_sqlite3.cmake )

include( setup_boost )
include( setup_doc )

if(USE_INTERNAL_SQLITE3)
  set(CONFIG_SQLITE3_INTERNAL "true")
else(USE_INTERNAL_SQLITE3)  
  set(CONFIG_SQLITE3_INTERNAL "false")
endif(USE_INTERNAL_SQLITE3)

set(sl3_CONFIG_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/sl3/config.hpp")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/config.in" "${sl3_CONFIG_HEADER}")



add_definitions(${mysqlt3_DEFINES})

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/include
)


if( ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    message(STATUS "sl3 set top project output pathes")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
endif( ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})



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

)
################################################################################


# TODO check win build

if(WIN32 AND NOT UNIX)

    option(sl3_BUILD_STATIC_LIB "ON for static lib build, OFF for shared lib build" ON )

    if(NOT sl3_BUILD_STATIC_LIB)
        set(sl3_BUILD_SHARED_LIB ON)
    endif(NOT sl3_BUILD_STATIC_LIB)

elseif(UNIX)
    option(sl3_BUILD_STATIC_LIB "build static library" ON )
    option(sl3_BUILD_SHARED_LIB "build shared library" OFF )

    if(NOT sl3_BUILD_STATIC_LIB AND NOT sl3_BUILD_SHARED_LIB)
        message(WARNING "neither static nor shared build enabled, will use default static")
        set(sl3_BUILD_STATIC_LIB ON)
    endif(NOT sl3_BUILD_STATIC_LIB AND NOT sl3_BUILD_SHARED_LIB) 

endif(WIN32 AND NOT UNIX)



if(sl3_BUILD_STATIC_LIB)
    add_library( sl3 STATIC  ${SQLITE3_FILES} ${sl3_SRC} ${sl3_SRCHDR} ${sl3_HDR} )
    #SET_TARGET_PROPERTIES(sl3 PROPERTIES OUTPUT_NAME sl3)
    set(sl3_install_targets ${sl3_install_targets} sl3)    
endif(sl3_BUILD_STATIC_LIB)

if(sl3_BUILD_SHARED_LIB) 
    # TODO, link against sqlt3 if not included / or required mysqlt3_LIBS ..
    add_library( sl3_so SHARED  ${SQLITE3_FILES} ${sl3_SRC} ${sl3_SRCHDR} ${sl3_HDR} )
    set_target_properties(sl3_so PROPERTIES VERSION ${sl3_VERSION})
    set_target_properties(sl3_so PROPERTIES SOVERSION ${sl3_MAJOR_VERSION}.${sl3_MINOR_VERSION})
    SET_TARGET_PROPERTIES(sl3_so PROPERTIES OUTPUT_NAME sl3)
    set(sl3_install_targets ${sl3_install_targets} sl3_so)
endif(sl3_BUILD_SHARED_LIB)

#since alias the libs, this seems to be required
link_directories( ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )

if (DEFINED sl3_BUILD_TESTS)
    if (${sl3_BUILD_TESTS})
        set (ALL_A4_TESTING_DISABLED FALSE)
    else (${sl3_BUILD_TESTS})
        set (ALL_A4_TESTING_DISABLED TRUE)
    endif(${sl3_BUILD_TESTS})
endif (DEFINED sl3_BUILD_TESTS)
include( setup_testing )
 

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



