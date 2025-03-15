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

if(APPLE)
    find_package(ICONV)
    list(APPEND MBASE_STD_INCLUDES ${Iconv_INCLUDE_DIRS})
    list(APPEND MBASE_STD_LIBS ${Iconv_LIBRARIES})
elseif(UNIX)
    find_package(UUID REQUIRED)
    find_package(Threads REQUIRED)
    list(APPEND MBASE_STD_INCLUDES ${UUID_INCLUDE_DIRS})
    list(APPEND MBASE_STD_LIBS ${UUID_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    file(READ ${CMAKE_SOURCE_DIR}/cmake/FindUUID.cmake MBASE_EMBEDDED_FIND_UUID_CMAKE)
endif()

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

foreach(MB_INCLUDE_FILE IN LISTS MBASE_STD_INCLUDE_STABLE_FILES)
    list(APPEND MBASE_STD_INCLUDE_INSTALL_FILES ${MBASE_STD_INCLUDE_INSTALL_PATH}/mbase/${MB_INCLUDE_FILE})
endforeach()

set(MBASE_STD_BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
set(MBASE_STD_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR})
set(MBASE_STD_LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set(MBASE_STD_CMAKE_CONFIG_NAME_IN mbase-std-config.cmake.in)
set(MBASE_STD_CMAKE_CONFIG_NAME mbase-std-config.cmake)
set(MBASE_STD_CMAKE_INSTALL_DESTINATION ${MBASE_STD_LIB_INSTALL_DIR}/cmake/mbase.libs)

configure_package_config_file(
            ${CMAKE_SOURCE_DIR}/cmake_config_in/mbase-std-config.cmake.in
            ${CMAKE_BINARY_DIR}/mbase-std-config.cmake
            INSTALL_DESTINATION ${MBASE_STD_CMAKE_INSTALL_DESTINATION}
            PATH_VARS
            MBASE_STD_INCLUDE_INSTALL_DIR
            MBASE_STD_LIB_INSTALL_DIR
            MBASE_STD_BIN_INSTALL_DIR
        )

install(FILES ${MBASE_STD_INCLUDE_INSTALL_FILES}
        PERMISSIONS
            OWNER_READ
            OWNER_WRITE
            GROUP_READ
            WORLD_READ
        DESTINATION ${MBASE_STD_INCLUDE_INSTALL_DIR}/mbase)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MBASE_STD_CMAKE_CONFIG_NAME} 
        PERMISSIONS
            OWNER_READ
            GROUP_READ
            WORLD_READ
        DESTINATION ${MBASE_STD_CMAKE_INSTALL_DESTINATION})