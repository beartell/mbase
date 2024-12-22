include_guard(GLOBAL)

include(MBASECommonConfig)
include(MBASEStdSys)

set(MBASE_PC_SYS_STRING "MBASE Core")
set(MBASE_PC_LIB_NAME "pc")

mbase_build_version(1 0 0 ${MBASE_VERSION_ALPHA} MBASE_PC_VERSION)
mbase_build_include_install_path(${MBASE_PC_LIB_NAME} MBASE_PC_INCLUDE_INSTALL_PATH)
mbase_build_lib_path(${MBASE_PC_LIB_NAME} MBASE_PC_LIB_PATH)

list(APPEND MBASE_PC_INCLUDE_DEPENDS ${MBASE_STD_INCLUDES})

list(APPEND MBASE_PC_INCLUDE_STABLE_FILES
    pc_config.h
    pc_diagnostics.h
    pc_io_manager.h
    pc_net_manager.h
    pc_program.h
    pc_state.h
    pc_stream_manager.h
    pc_termination_handler.h
)

add_library(mb_pc SHARED
    ${MBASE_PC_LIB_PATH}/pc_config.cpp
    ${MBASE_PC_LIB_PATH}/pc_state.cpp
    ${MBASE_PC_LIB_PATH}/pc_io_manager.cpp
    ${MBASE_PC_LIB_PATH}/pc_stream_manager.cpp
    ${MBASE_PC_LIB_PATH}/pc_program.cpp
    ${MBASE_PC_LIB_PATH}/pc_diagnostics.cpp
    ${MBASE_PC_LIB_PATH}/pc_net_manager.cpp
)

target_compile_definitions(mb_pc PRIVATE ${MBASE_COMMON_COMPILE_DEFINITIONS})
target_compile_options(mb_pc PUBLIC -W4)
target_include_directories(mb_pc PUBLIC ${MBASE_PC_INCLUDE_DEPENDS})

set_property(TARGET mb_pc PROPERTY POSITION_INDEPENDENT_CODE ON)

foreach(MB_INCLUDE_FILE IN LISTS MBASE_PC_INCLUDE_STABLE_FILES)
    list(APPEND MBASE_PC_INCLUDE_INSTALL_FILES ${MBASE_PC_INCLUDE_INSTALL_PATH}/${MB_INCLUDE_FILE})
endforeach()
