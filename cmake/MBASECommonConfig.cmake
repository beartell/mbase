include_guard(GLOBAL)

include(GNUInstallDirs)

message("System install directory: ${CMAKE_INSTALL_PREFIX}")
message("User executables dir: ${CMAKE_INSTALL_FULL_BINDIR}")
message("System executables dir: ${CMAKE_INSTALL_FULL_SBINDIR}")
message("Program executables dir: ${CMAKE_INSTALL_FULL_LIBEXECDIR}")
message("Read only data dir: ${CMAKE_INSTALL_FULL_SYSCONFDIR}")
message("Modifiable arch independent data dir: ${CMAKE_INSTALL_SHAREDSTATEDIR}")

set(MBASE_GLOBAL_INCLUDE ${CMAKE_SOURCE_DIR}/include/)
set(MBASE_GLOBAL_SOURCE_DIRECTORY ${CMAKE_SOURCE_DIR}/mbase/)
set(MBASE_GLOBAL_CONFIG_IN_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake_config_in/)

list(APPEND MBASE_STABILITY_CATEGORIES
    alpha    
    beta
    stable
)

list(GET MBASE_STABILITY_CATEGORIES 0 MBASE_VERSION_ALPHA)
list(GET MBASE_STABILITY_CATEGORIES 1 MBASE_VERSION_BETA)
list(GET MBASE_STABILITY_CATEGORIES 2 MBASE_VERSION_STABLE)

function(mbase_build_version in_major in_minor in_patch in_stability out_version_string)
    # do input checks later
    set(${out_version_string} ${in_major}.${in_minor}.${in_patch}${in_stability} PARENT_SCOPE)
endfunction()

function(mbase_build_include_install_path in_path_string out_install_path)
    # do input checks later
    if(in_path_string STREQUAL " ")
        # this is the only exception for mbase std library
        set(${out_install_path} ${MBASE_GLOBAL_INCLUDE} PARENT_SCOPE)
    else()
        set(${out_install_path} ${MBASE_GLOBAL_INCLUDE}mbase/${in_path_string}/ PARENT_SCOPE)
    endif()
endfunction()

function(mbase_build_lib_path in_lib_name out_lib_path)
    set(${out_lib_path} ${MBASE_GLOBAL_SOURCE_DIRECTORY}${in_lib_name}/ PARENT_SCOPE)
endfunction()


