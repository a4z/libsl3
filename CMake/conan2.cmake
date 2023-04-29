

execute_process(
    COMMAND conan install "${CMAKE_SOURCE_DIR}"
      --output-folder=${CMAKE_CURRENT_BINARY_DIR}
      --profile:build=profiles/apple-clang
      --profile:host=profiles/apple-clang
      -s build_type=${CMAKE_BUILD_TYPE}
      --build=missing
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)


message(FATAL_ERROR "Stop for debug")
