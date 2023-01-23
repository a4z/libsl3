
# assumtion that is correct used, not awesome but ...
list(GET CMAKE_MODULE_PATH 0 TOP_CMAKE_MODULE_PATH)

if(NOT EXISTS "${TOP_CMAKE_MODULE_PATH}/conan.cmake")
  message(
    STATUS
      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(
    DOWNLOAD
      "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
      "${TOP_CMAKE_MODULE_PATH}/conan.cmake"
    EXPECTED_HASH
      SHA256=5cdb3042632da3efff558924eecefd580a0e786863a857ca097c3d1d43df5dcd
    TLS_VERIFY ON)
endif()

include(conan REQUIRED)

set(CONAN_SYSTEM_INCLUDES ON)

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# TODO, replace PROFILE, PROFILE_HOST, PROFILE_BUILD

if(isMultiConfig)
  foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
    conan_cmake_install(
      PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}
      REMOTE conancenter
      BUILD missing
      GENERATOR cmake_multi
      PROFILE default
      SETTINGS build_type=${TYPE})
  endforeach()
  include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
else()
  conan_cmake_install(
    PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}
    REMOTE conancenter
    BUILD missing
    GENERATOR cmake
    PROFILE default
  )
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
endif()

conan_basic_setup(TARGETS)
