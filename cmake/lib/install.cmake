include_guard(GLOBAL)

include(GNUInstallDirs)

install(TARGETS sl3 EXPORT sl3Targets)
INSTALL(FILES ${sl3_HDR} DESTINATION include/sl3)
INSTALL(FILES include/sl3.hpp DESTINATION include/sl3)

install(EXPORT sl3Targets
    FILE sl3Targets.cmake
    NAMESPACE a4z::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sl3ConfigVersion.cmake
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/sl3Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sl3Config.cmake
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/sl3Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/sl3ConfigVersion.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"

)


