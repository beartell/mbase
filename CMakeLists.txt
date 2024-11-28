if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

cmake_minimum_required(VERSION 3.15...3.31)
project("MBASE Libraries" VERSION 1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")


set(CMAKE_CXX_STANDARD 20)

#add_executable(mbase_test_program main.cpp)

include(MBASEStdSys)
include(MBASECoreSys)
include(MBASEInferenceSys)

include(InstallRequiredSystemLibraries)

# add CPack to project
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "MBASE Libraries")
set(CPACK_PACKAGE_VENDOR "mbaselibs.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "MBASE Libraries - Installation Wizard")
set(CPACK_PACKAGE_VERSION "1.3.142stable")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "142")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "MBASE Libraries")

# This must always be after all CPACK\_\* variables are defined
include(CPack)

#target_include_directories(mbase_test_program PUBLIC ${MBASE_INFERENCE_TARGET_INCLUDES})
#target_link_libraries(mbase_test_program PRIVATE mb_core mb_inference)

