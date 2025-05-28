include_guard(GLOBAL)

include(MBASECommonConfig)
include(MBASEStdSys)
include(MBASEJsonSys)

set(MBASE_MCP_SYS_STRING "MBASE MCP")
set(MBASE_MCP_LIB_NAME "mcp")

mbase_build_version(1 0 0 ${MBASE_VERSION_ALPHA} MBASE_MCP_VERSION)
mbase_build_include_install_path(${MBASE_MCP_LIB_NAME} MBASE_MCP_INCLUDE_INSTALL_PATH)
mbase_build_lib_path(${MBASE_MCP_LIB_NAME} MBASE_MCP_LIB_PATH)

set(MBASE_MCP_SYS_VERSION ${MBASE_MCP_VERSION})

message("${MBASE_MCP_SYS_STRING} library version: ${MBASE_MCP_VERSION}")
message("${MBASE_MCP_SYS_STRING} install source path: ${MBASE_MCP_INCLUDE_INSTALL_PATH}")
message("${MBASE_MCP_SYS_STRING} lib source path: ${MBASE_MCP_LIB_PATH}")

list(APPEND MBASE_MCP_INCLUDE_DEPENDS
    ${MBASE_STD_INCLUDES}
    ${MBASE_JSON_INCLUDE_DEPENDS}
    ${CMAKE_SOURCE_DIR}/external
)

list(APPEND MBASE_MCP_LIB_DEPENDS
    ${MBASE_STD_LIBS}
    mb_json
)

list(APPEND MBASE_MCP_COMPILE_DEFINITIONS
    ${MBASE_COMMON_COMPILE_DEFINITIONS}
)

list(APPEND MBASE_MCP_INCLUDE_STABLE_FILES
    mcp_client_arguments.h
    mcp_client_base.h
    mcp_client_callbacks.h
    mcp_client_server_http.h
    mcp_client_server_state.h
    mcp_client_server_stdio.h
    mcp_common.h
    mcp_notifications.h
    mcp_packet_parsing.h
    mcp_server_arguments.h
    mcp_server_base.h
    mcp_server_client_http_streamable.h
    mcp_server_client_state.h
    mcp_server_client_stdio.h
    mcp_server_descriptions.h
    mcp_server_features.h
    mcp_server_http_streamable.h
    mcp_server_responses.h
    mcp_server_stdio.h
    mcp_server_to_client_requests.h
)

add_library(mb_mcp
    ${MBASE_MCP_LIB_PATH}/mcp_client_base.cpp
    ${MBASE_MCP_LIB_PATH}/mcp_client_server_http.cpp
    ${MBASE_MCP_LIB_PATH}/mcp_client_server_state.cpp
    ${MBASE_MCP_LIB_PATH}/mcp_client_server_stdio.cpp
    ${MBASE_MCP_LIB_PATH}/mcp_server_base.cpp
    ${MBASE_MCP_LIB_PATH}/mcp_server_client_http_streamable.cpp
    ${MBASE_MCP_LIB_PATH}/mcp_server_client_state.cpp
    ${MBASE_MCP_LIB_PATH}/mcp_server_client_stdio.cpp
    ${MBASE_MCP_LIB_PATH}/mcp_server_features.cpp
    ${MBASE_MCP_LIB_PATH}/mcp_server_http_streamable.cpp
    ${MBASE_MCP_LIB_PATH}/mcp_server_stdio.cpp
)

if(MBASE_MCP_SSL STREQUAL "ON")
    if(WIN32)
        set(OPENSSL_MSVC_STATIC_RT "TRUE")
    endif()
    set(OPENSSL_USE_STATIC_LIBS "TRUE")
    find_package(OpenSSL REQUIRED)
    list(APPEND MBASE_MCP_COMPILE_DEFINITIONS
        MBASE_MCP_SSL
        CPPHTTPLIB_OPENSSL_SUPPORT
    )
    list(APPEND MBASE_MCP_INCLUDE_DEPENDS
        ${OPENSSL_INCLUDE_DIR}
    )
    list(APPEND MBASE_MCP_LIB_DEPENDS
        ${OPENSSL_SSL_LIBRARY}
        ${OPENSSL_CRYPTO_LIBRARY}
    )
endif()

target_compile_definitions(mb_mcp PRIVATE ${MBASE_MCP_COMPILE_DEFINITIONS})
target_include_directories(mb_mcp PUBLIC ${MBASE_MCP_INCLUDE_DEPENDS})
target_link_libraries(mb_mcp PRIVATE ${MBASE_MCP_LIB_DEPENDS})
target_compile_options(mb_mcp PRIVATE ${MBASE_COMMON_COMPILE_OPTIONS})

set_property(TARGET mb_mcp PROPERTY POSITION_INDEPENDENT_CODE ON)

foreach(MB_INCLUDE_FILE IN LISTS MBASE_MCP_INCLUDE_STABLE_FILES)
    list(APPEND MBASE_MCP_INCLUDE_INSTALL_FILES ${MBASE_MCP_INCLUDE_INSTALL_PATH}/${MB_INCLUDE_FILE})
endforeach()
