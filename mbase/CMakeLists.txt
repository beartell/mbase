add_library(mb_core SHARED pc_state.cpp pc_config.cpp pc_io_manager.cpp pc_stream_manager.cpp pc_program.cpp pc_diagnostics.cpp pc_net_manager.cpp)
 
target_include_directories(mb_core PUBLIC "${PROJECT_SOURCE_DIR}/include")
add_library(mb_inference SHARED inf_client.cpp inf_processor.cpp inf_model.cpp inf_program.cpp inf_maip_server.cpp inf_sampling.cpp inf_gguf_meta_configurator.cpp)

target_compile_definitions(mb_core PRIVATE MBASE_BUILD=1 MBASE_INTERNAL_API=1)
target_compile_definitions(mb_inference PRIVATE MBASE_BUILD=1 MBASE_INTERNAL_API=1)

target_include_directories(mb_inference PUBLIC "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/external/llama.cpp/include")
#target_link_directories(mb_inference PRIVATE "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/common.lib" "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/ggml.lib" "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/llama.lib")
target_link_libraries(mb_inference PRIVATE mb_core "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/common.lib" "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/ggml.lib" "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/llama.lib")