add_library(mb_core STATIC pc_config.cpp pc_io_manager.cpp pc_stream_manager.cpp pc_program.cpp pc_diagnostics.cpp pc_net_manager.cpp)
message("${PROJECT_SOURCE_DIR}/include")
target_include_directories(mb_core PUBLIC "${PROJECT_SOURCE_DIR}/include")