add_executable(mbase_embedder_util embedding.cpp)

target_include_directories(mbase_embedder_util PUBLIC mb_inference mb_json)
target_link_libraries(mbase_embedder_util PRIVATE ${MBASE_STD_LIBS} mb_inference mb_json)
