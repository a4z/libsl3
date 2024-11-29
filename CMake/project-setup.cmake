include_guard(GLOBAL)

if(PROJECT_IS_TOP_LEVEL)
    # make git ignore the build directory
    file(WRITE ${CMAKE_BINARY_DIR}/.gitignore "*")
    # set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
    # set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
    # set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
    include(CTest)
endif()


if(DEFINED ACTIVE_PRESET_NAME)
  message(STATUS "-- Running preset : ${ACTIVE_PRESET_NAME}")
endif()



if(DEFINED PROJECT_ADDONS)
  foreach(ADDON ${PROJECT_ADDONS})
    message(STATUS "Add project addon: ${ADDON}")
    include(${ADDON})
  endforeach()
endif()


# TODO, do I want to allow env variables also?
# probably not since that might confuse
# if(DEFINED ENV{MY_CMAKE_PROJECT_ADDONS})
#   foreach(ADDON $ENV{MY_CMAKE_PROJECT_ADDONS})
#     message(STATUS "Add project addon (env): ${ADDON}")
#     include(${ADDON})
#   endforeach()
# endif()
