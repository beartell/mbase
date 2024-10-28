cmake_minimum_required(VERSION 3.15)
project(mbase_std)
set (CMAKE_CXX_STANDARD 20)
add_executable(mbase_test_program main.cpp )
add_subdirectory(mbase)
include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/external/llama.cpp/include")
target_link_libraries(mbase_test_program PRIVATE mb_core mb_inference)

