include_guard(GLOBAL)

include(MBASECommonConfig)

set(MBASE_STD_SYS_STRING "MBASE Standard")
set(MBASE_STD_LIB_NAME " ")
set(MBASE_STD_LIB_NAME_EXCEPTION "std")

mbase_build_version(1 0 0 ${MBASE_VERSION_ALPHA} MBASE_STD_VERSION)
mbase_build_include_install_path(${MBASE_STD_LIB_NAME} MBASE_STD_INCLUDE_INSTALL_PATH)

message("${MBASE_STD_SYS_STRING} library version: ${MBASE_STD_VERSION}")
message("${MBASE_STD_SYS_STRING} install source path: ${MBASE_STD_INCLUDE_INSTALL_PATH}")

list(APPEND MBASE_STD_INCLUDES ${MBASE_GLOBAL_INCLUDE})

cmake_host_system_information(RESULT logical_core_count QUERY NUMBER_OF_LOGICAL_CORES)
cmake_host_system_information(RESULT physical_core_count QUERY NUMBER_OF_PHYSICAL_CORES)
cmake_host_system_information(RESULT host_name QUERY HOSTNAME)
cmake_host_system_information(RESULT fq_host_name QUERY FQDN)
cmake_host_system_information(RESULT total_vir_mem QUERY TOTAL_VIRTUAL_MEMORY)
cmake_host_system_information(RESULT avail_vir_mem QUERY AVAILABLE_VIRTUAL_MEMORY)
cmake_host_system_information(RESULT total_phy_mem QUERY TOTAL_PHYSICAL_MEMORY)
cmake_host_system_information(RESULT available_phy_mem QUERY AVAILABLE_PHYSICAL_MEMORY)

message("Logical core count: ${logical_core_count}")
message("Physical core count: ${physical_core_count}")
message("Hostname: ${host_name}")
message("FQDN: ${fq_host_name}")
message("Total virtual memory: ${total_vir_mem}")
message("Available virtual memory: ${avail_vir_mem}")
message("Total physical memory: ${total_phy_mem}")
message("Available physical memory: ${available_phy_mem}")

if(UNIX)

find_package(UUID REQUIRED)
list(APPEND MBASE_STD_INCLUDES ${UUID_INCLUDE_DIRS})
list(APPEND MBASE_STD_LIBS ${UUID_LIBRARIES})

endif(UNIX)

list(APPEND MBASE_STD_INCLUDE_STABLE_FILES
    algorithm.h
    allocator.h
    argument_get_value.h
    atomic.h
    behaviors.h
    binary_iterator.h
    char_stream.h
    common.h
    filesystem.h
    http_parser.h
    index_assigner.h
    io_base.h
    io_file.h
    io_tcp_client.h
    list_iterator.h
    list.h
    maip_client.h
    maip_parser.h
    map.h
    mbase_std.h
    node_type.h
    platform.h
    queue.h
    rng.h
    sequence_iterator.h
    set.h
    smart_conversion.h
    stack.h
    string.h
    synchronization.h
    thread.h
    traits.h
    type_sequence.h
    unordered_map_iterator.h
    unordered_map.h
    vector.h
    wsa_init.h
)

message("Cmake binary directory: ${CMAKE_BINARY_DIR}")

foreach(MB_INCLUDE_FILE IN LISTS MBASE_STD_INCLUDE_STABLE_FILES)
    list(APPEND MBASE_STD_INCLUDE_INSTALL_FILES ${MBASE_STD_INCLUDE_INSTALL_PATH}mbase/${MB_INCLUDE_FILE})
endforeach()

#This type of config management is the only exception for mbase std library

# configure_file(
#     ${MBASE_GLOBAL_CONFIG_IN_DIRECTORY}mbase-std-config.cmake.in
#     ${CMAKE_BINARY_DIR}/mbase-std-config.cmake
#     @ONLY
# )

include(GNUInstallDirs)

install(FILES ${MBASE_STD_INCLUDE_INSTALL_FILES}
        PERMISSIONS
            OWNER_READ
            OWNER_WRITE
            GROUP_READ
            WORLD_READ
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mbase)

