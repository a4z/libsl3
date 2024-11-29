
if(DEFINED ENV{VCPKG_INSTALLATION_ROOT})
  set(VCPKG_ROOT $ENV{VCPKG_INSTALLATION_ROOT})
elseif(DEFINED ENV{VCPKG_ROOT})
  set(VCPKG_ROOT $ENV{VCPKG_ROOT})
else()
  message(FATAL_ERROR "Neither VCPKG_INSTALLATION_ROOT nor VCPKG_ROOT environment variables are defined.")
endif()

include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
