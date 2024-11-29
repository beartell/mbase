include_guard(GLOBAL)

include(MBASECommonConfig)
include(MBASEStdSys)

set(MBASE_CORE_SYS_STRING "MBASE Core")
set(MBASE_CORE_LIB_NAME "pc")

mbase_build_version(1 0 0 ${MBASE_VERSION_ALPHA} MBASE_CORE_VERSION)
mbase_build_include_install_path(${MBASE_CORE_LIB_NAME} MBASE_CORE_INCLUDE_INSTALL_PATH)
mbase_build_lib_path(${MBASE_CORE_LIB_NAME} MBASE_CORE_LIB_PATH)

message("Core library path ${MBASE_CORE_LIB_PATH}")

message("${MBASE_CORE_SYS_STRING} library version: ${MBASE_CORE_VERSION}")
message("${MBASE_CORE_SYS_STRING} install source path: ${MBASE_CORE_INCLUDE_INSTALL_PATH}")
message("${MBASE_CORE_SYS_STRING} lib source path: ${MBASE_CORE_LIB_PATH}")

list(APPEND MBASE_CORE_INCLUDE_DEPENDS ${MBASE_STD_INCLUDES})

list(APPEND MBASE_CORE_INCLUDE_STABLE_FILES
    pc_config.h
    pc_diagnostics.h
    pc_io_manager.h
    pc_net_manager.h
    pc_program.h
    pc_state.h
    pc_stream_manager.h
    pc_termination_handler.h
)

add_library(mb_core SHARED
    ${MBASE_CORE_LIB_PATH}pc_config.cpp
    ${MBASE_CORE_LIB_PATH}pc_state.cpp
    ${MBASE_CORE_LIB_PATH}pc_io_manager.cpp
    ${MBASE_CORE_LIB_PATH}pc_stream_manager.cpp
    ${MBASE_CORE_LIB_PATH}pc_program.cpp
    ${MBASE_CORE_LIB_PATH}pc_diagnostics.cpp
    ${MBASE_CORE_LIB_PATH}pc_net_manager.cpp
)

target_compile_definitions(mb_core PRIVATE MBASE_BUILD=1 MBASE_INTERNAL_API=1)
target_include_directories(mb_core PUBLIC ${MBASE_CORE_INCLUDE_DEPENDS})

foreach(MB_INCLUDE_FILE IN LISTS MBASE_CORE_INCLUDE_STABLE_FILES)
    list(APPEND MBASE_CORE_INCLUDE_INSTALL_FILES ${MBASE_CORE_INCLUDE_INSTALL_PATH}${MB_INCLUDE_FILE})
endforeach()

include(GNUInstallDirs)

install(TARGETS mb_core
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/mbase/${MBASE_CORE_LIB_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/mbase/${MBASE_CORE_LIB_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/mbase/${MBASE_CORE_LIB_NAME}
)

install(FILES ${MBASE_CORE_INCLUDE_INSTALL_FILES}
        PERMISSIONS
            OWNER_READ
            OWNER_WRITE
            GROUP_READ
            WORLD_READ
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mbase/${MBASE_CORE_LIB_NAME})
