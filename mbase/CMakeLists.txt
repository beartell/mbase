if (WIN32)

set(external_source_libs "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/common.lib" 
"${PROJECT_SOURCE_DIR}/external/llama.cpp/src/ggml.lib" 
"${PROJECT_SOURCE_DIR}/external/llama.cpp/src/llama.lib"
)

set(external_includes "${PROJECT_SOURCE_DIR}/include" 
"${PROJECT_SOURCE_DIR}/external/llama.cpp/include"
)

endif (WIN32)

if (UNIX)

find_library(UUID_LIB uuid)
# if not found, notify...

set(external_source_libs "${PROJECT_SOURCE_DIR}/external/llama.cpp/src/libcommon.a" 
"${PROJECT_SOURCE_DIR}/external/llama.cpp/src/libggml.so" 
"${PROJECT_SOURCE_DIR}/external/llama.cpp/src/libllama.so" 
${UUID_LIB}
)

set(external_includes "${PROJECT_SOURCE_DIR}/include" 
"${PROJECT_SOURCE_DIR}/external/llama.cpp/include" 
)

endif (UNIX)

add_library(mb_core SHARED pc_state.cpp 
    pc_config.cpp 
    pc_io_manager.cpp 
    pc_stream_manager.cpp 
    pc_program.cpp 
    pc_diagnostics.cpp 
    pc_net_manager.cpp
)

add_library(mb_inference SHARED inf_client.cpp 
    inf_processor.cpp 
    inf_model.cpp 
    inf_program.cpp 
    inf_maip_server.cpp 
    inf_sampling.cpp 
    inf_gguf_meta_configurator.cpp 
    inf_maip_user.cpp 
    inf_t2t_processor.cpp 
    inf_t2t_model.cpp 
    inf_embedder.cpp
    inf_maip_model_description.cpp
    inf_maip_callbacks.cpp
)

target_include_directories(mb_core PUBLIC ${external_includes})
target_include_directories(mb_inference PUBLIC ${external_includes})

target_link_libraries(mb_inference PRIVATE mb_core ${external_source_libs})

target_compile_definitions(mb_core PRIVATE MBASE_BUILD=1 MBASE_INTERNAL_API=1)
target_compile_definitions(mb_inference PRIVATE MBASE_BUILD=1 MBASE_INTERNAL_API=1)
