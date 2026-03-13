cmake_minimum_required(VERSION 3.20)

project(sl3_pkg_consumer_smoke LANGUAGES CXX)

find_package(sl3 CONFIG REQUIRED)

add_executable(sl3_pkg_consumer_smoke main.cpp)
target_link_libraries(sl3_pkg_consumer_smoke PRIVATE a4z::sl3)
target_compile_features(sl3_pkg_consumer_smoke PRIVATE cxx_std_17)

enable_testing()
add_test(NAME sl3_pkg_consumer_smoke COMMAND sl3_pkg_consumer_smoke)
