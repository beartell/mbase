add_library(mb_core STATIC pc_config.cpp pc_io_manager.cpp pc_stream_manager.cpp pc_program.cpp pc_diagnostics.cpp pc_net_manager.cpp)
message("${PROJECT_SOURCE_DIR}/include")
target_include_directories(mb_core PUBLIC "${PROJECT_SOURCE_DIR}/include")

add_library(mb_inference STATIC inf_client.cpp inf_processor.cpp inf_model.cpp)
target_include_directories(mb_inference PUBLIC "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/external/llama.cpp/include")
target_link_directories(mb_inference PRIVATE "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/common.lib" "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/ggml.lib" "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/llama.lib")