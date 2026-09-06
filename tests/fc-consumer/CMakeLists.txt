cmake_minimum_required(VERSION 3.31)

project(sl3_fc_consumer_smoke LANGUAGES C CXX)

# Path to an already-available libsl3 source tree (e.g. the CI checkout).
# Using SOURCE_DIR keeps this offline: FetchContent skips all download logic
# and just adds the directory as a subproject, so PROJECT_IS_TOP_LEVEL is
# false for libsl3 - the exact path that regressed in issue #72.
if(NOT DEFINED sl3_SOURCE_TREE)
    set(sl3_SOURCE_TREE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
endif()
get_filename_component(sl3_SOURCE_TREE "${sl3_SOURCE_TREE}" ABSOLUTE)

include(FetchContent)
FetchContent_Declare(
    sl3
    SOURCE_DIR "${sl3_SOURCE_TREE}"
    OVERRIDE_FIND_PACKAGE
)

set(sl3_BUILD_TESTING OFF)
set(sl3_USE_INTERNAL_SQLITE3 ON)
set(sl3_USE_COMMON_COMPILER_WARNINGS OFF)

find_package(sl3 CONFIG REQUIRED)

add_executable(sl3_fc_consumer_smoke main.cpp)
target_link_libraries(sl3_fc_consumer_smoke PRIVATE sl3)
target_compile_features(sl3_fc_consumer_smoke PRIVATE cxx_std_17)

enable_testing()
add_test(NAME sl3_fc_consumer_smoke COMMAND sl3_fc_consumer_smoke)
