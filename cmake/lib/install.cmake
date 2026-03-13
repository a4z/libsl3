include_guard(GLOBAL)

include(GNUInstallDirs)

set(sl3_CMAKE_PACKAGE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/sl3")

install(
    TARGETS sl3
    EXPORT sl3Targets
    FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT sl3Targets
    FILE sl3Targets.cmake
    NAMESPACE a4z::
    DESTINATION "${sl3_CMAKE_PACKAGE_DIR}"
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sl3ConfigVersion.cmake
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/sl3Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sl3Config.cmake
    INSTALL_DESTINATION "${sl3_CMAKE_PACKAGE_DIR}"
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/sl3Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/sl3ConfigVersion.cmake
    DESTINATION "${sl3_CMAKE_PACKAGE_DIR}"

)
