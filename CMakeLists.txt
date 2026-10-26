cmake_minimum_required(VERSION 3.14)
project(rv32emu_project VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(rvcore STATIC src/decoder.cpp)
target_compile_options(rvcore PRIVATE -Wall -Wextra -Wconversion)
target_include_directories(rvcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(rv32emu src/main.cpp)
target_link_libraries(rv32emu PRIVATE rvcore)

enable_testing()
add_executable(rv_tests tests/test_decoder.cpp)
target_link_libraries(rv_tests PRIVATE rvcore gtest_main)
include(GoogleTest)
gtest_discover_tests(rv_tests)