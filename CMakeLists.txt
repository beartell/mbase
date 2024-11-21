if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

cmake_minimum_required(VERSION 3.15...3.31)
project(mbase_std VERSION 1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")


set(CMAKE_CXX_STANDARD 20)

add_executable(mbase_test_program main.cpp "include/mbase/inference/inf_maip_peer_base.h")
add_subdirectory(mbase)

target_include_directories(mbase_test_program PUBLIC ${MBASE_INFERENCE_TARGET_INCLUDES})
target_link_libraries(mbase_test_program PRIVATE mb_core mb_inference)

