if (WIN32)

set(external_source_libs
"${PROJECT_SOURCE_DIR}/external/llama.cpp/src/ggml.lib" 
"${PROJECT_SOURCE_DIR}/external/llama.cpp/src/llama.lib"
)

set(external_includes "${PROJECT_SOURCE_DIR}/include" 
"${PROJECT_SOURCE_DIR}/external/llama.cpp/include"
)

endif (WIN32)

if (UNIX)

find_package(UUID REQUIRED)
find_package(llama REQUIRED)

list(APPEND MBASE_PROJECT_COMMON_INCLUDES
    ${CMAKE_SOURCE_DIR}/include
    ${UUID_INCLUDE_DIRS}
)

list(APPEND MBASE_PROJECT_COMMON_LIBS
    ${UUID_LIBRARIES}
)

list(APPEND MBASE_INFERENCE_TARGET_INCLUDES
    ${MBASE_PROJECT_COMMON_INCLUDES}
    ${LLAMA_INCLUDE_DIR}
)

list(APPEND MBASE_INFERENCE_TARGET_LIBS
    ${MBASE_PROJECT_COMMON_LIBS}
    ${llama_LIBRARY}
    ${ggml_LIBRARY}
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
    inf_common.cpp
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

target_compile_options(mb_core PRIVATE -Wall -Wextra -Wpedantic -O3)
target_compile_options(mb_inference PRIVATE -Wall -Wextra -Wpedantic -O3)

target_include_directories(mb_core PUBLIC ${MBASE_PROJECT_COMMON_INCLUDES})
target_include_directories(mb_inference PUBLIC ${MBASE_INFERENCE_TARGET_INCLUDES})

target_link_libraries(mb_inference PRIVATE mb_core ${MBASE_INFERENCE_TARGET_LIBS})

target_compile_definitions(mb_core PRIVATE MBASE_BUILD=1 MBASE_INTERNAL_API=1)
target_compile_definitions(mb_inference PRIVATE MBASE_BUILD=1 MBASE_INTERNAL_API=1)
