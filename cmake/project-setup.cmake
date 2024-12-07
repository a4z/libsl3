include_guard(GLOBAL)
# Keep this here for now, but add a CI test that builds with 20 or 17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(PROJECT_IS_TOP_LEVEL)
    # make git ignore the build directory
    file(WRITE ${CMAKE_BINARY_DIR}/.gitignore "*")
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
