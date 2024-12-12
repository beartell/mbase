if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

cmake_minimum_required(VERSION 3.15...3.31)

project("mbase.libs" VERSION 1.0.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 20)

include(MBASEStdSys)
include(MBASEJsonSys)
include(MBASEFrameworkSys)
include(MBASECoreSys)
include(MBASEInferenceSys)

include(InstallRequiredSystemLibraries)
mbase_install_libraries()

set(MBASE_LIBS_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR})
set(MBASE_LIBS_LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set(MBASE_LIBS_BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
set(MBASE_LIBS_LIB_NAME "mbase.libs")
set(MBASE_LIBS_VERSION "1.0.0")

configure_package_config_file(
  ${MBASE_GLOBAL_CONFIG_IN_DIRECTORY}/mbase.libs-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/mbase.libs-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mbase.libs-config.cmake
  PATH_VARS
    MBASE_LIBS_INCLUDE_INSTALL_DIR
    MBASE_LIBS_LIB_INSTALL_DIR
    MBASE_LIBS_BIN_INSTALL_DIR
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/mbase.libs-config.cmake
  PERMISSIONS
      OWNER_READ
      OWNER_WRITE
      GROUP_READ
      WORLD_READ
  DESTINATION ${MBASE_LIBS_LIB_INSTALL_DIR}/cmake/mbase.libs
)

# add_subdirectory(examples/openai-server)
add_subdirectory(examples/simple-conversation)
add_subdirectory(examples/typo-fixer)
# set(CPACK_GENERATOR "DEB")
# set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Saul E. Erdog")
# set(CPACK_PACKAGE_NAME "MBASE-Libraries")
# set(CPACK_PACKAGE_VENDOR "mbaselibs.com")
# set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
#     "MBASELibraries-Installation Wizard")
# set(CPACK_PACKAGE_VERSION "1.3.142stable")
# set(CPACK_PACKAGE_VERSION_MAJOR "1")
# set(CPACK_PACKAGE_VERSION_MINOR "3")
# set(CPACK_PACKAGE_VERSION_PATCH "142")
# set(CPACK_PACKAGE_INSTALL_DIRECTORY "MBASELibraries")

# # This must always be after all CPACK\_\* variables are defined
# include(CPack)

#target_include_directories(mbase_test_program PUBLIC ${MBASE_INFERENCE_TARGET_INCLUDES})
#target_link_libraries(mbase_test_program PRIVATE mb_core mb_inference)

