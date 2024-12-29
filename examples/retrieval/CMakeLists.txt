add_executable(mbase_retrieval retrieval.cpp)

target_compile_definitions(mbase_retrieval PRIVATE ${MBASE_COMMON_COMPILE_DEFINITIONS})
target_compile_options(mbase_retrieval PRIVATE ${MBASE_COMMON_COMPILE_OPTIONS})
target_include_directories(mbase_retrieval PUBLIC mb_inference)
target_link_libraries(mbase_retrieval PRIVATE ${MBASE_STD_LIBS} mb_inference mb_json)
