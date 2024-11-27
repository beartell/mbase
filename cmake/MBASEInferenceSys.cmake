include_guard(GLOBAL)

include(MBASECommonConfig)
include(MBASEStdSys)
include(MBASECoreSys)

set(MBASE_INFERENCE_SYS_STRING "MBASE Inference")
set(MBASE_INFERENCE_PATH_STRING "inference")

mbase_build_version(1 0 0 ${MBASE_VERSION_ALPHA} MBASE_INFERENCE_VERSION)
mbase_build_include_install_path(${MBASE_INFERENCE_PATH_STRING} MBASE_INFERENCE_INCLUDE_INSTALL_PATH)

message("${MBASE_INFERENCE_SYS_STRING} library version: ${MBASE_INFERENCE_VERSION}")
message("${MBASE_INFERENCE_SYS_STRING} install source path: ${MBASE_INFERENCE_INCLUDE_INSTALL_PATH}")

find_package(llama REQUIRED)

if(NOT llama_LIBRARY)
    message(FATAL "ERROR: llama.cpp backend is selected but library not found!")
endif()

list(APPEND MBASE_INFERENCE_INCLUDE_DEPENDS
    ${MBASE_STD_INCLUDES}
    ${MBASE_CORE_INCLUDE_DEPENDS}
    ${LLAMA_INCLUDE_DIR}
)

list(APPEND MBASE_INFERENCE_LIB_DEPENDS
    ${MBASE_STD_LIBS}
    ${llama_LIBRARY}
    ${ggml_LIBRARY}
    mb_core
)

list(APPEND MBASE_INFERENCE_INCLUDE_STABLE_FILES
    inf_chat_templates.h
    inf_client.h
    inf_common.h
    inf_context_line.h
    inf_embedder_client.h
    inf_embedder.h
    inf_gguf_metadata_configurator.h
    inf_maip_callbacks.h
    inf_maip_model_description.h
    inf_maip_peer_base.h
    inf_maip_peer_t2t.h
    inf_maip_server.h
    inf_maip_user.h
    inf_model.h
    inf_processor.h
    inf_program.h
    inf_sampling_set.h
    inf_t2t_client.h
    inf_t2t_model.h
    inf_t2t_processor.h
)

add_library(mb_inference SHARED 
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_client.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_common.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_embedder.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_gguf_meta_configurator.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_maip_callbacks.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_maip_model_description.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_maip_peer_base.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_maip_peer_t2t.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_maip_server.cpp 
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_maip_user.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_model.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_processor.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_program.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_t2t_client.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_t2t_model.cpp
    ${MBASE_GLOBAL_SOURCE_DIRECTORY}inf_t2t_processor.cpp
)

target_compile_definitions(mb_inference PRIVATE MBASE_BUILD=1 MBASE_INTERNAL_API=1)

target_include_directories(mb_inference PUBLIC ${MBASE_INFERENCE_INCLUDE_DEPENDS})
target_link_libraries(mb_inference PRIVATE ${MBASE_INFERENCE_LIB_DEPENDS})

foreach(MB_INCLUDE_FILE IN LISTS MBASE_INFERENCE_INCLUDE_STABLE_FILES)
    list(APPEND MBASE_INFERENCE_INCLUDE_INSTALL_FILES ${MBASE_INFERENCE_INCLUDE_INSTALL_PATH}${MB_INCLUDE_FILE})
endforeach()

