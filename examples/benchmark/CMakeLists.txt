add_executable(mbase-benchmark-t2t benchmark.cpp)

target_compile_definitions(mbase-benchmark-t2t PRIVATE ${MBASE_COMMON_COMPILE_DEFINITIONS})
target_compile_options(mbase-benchmark-t2t PRIVATE ${MBASE_COMMON_COMPILE_OPTIONS})
target_include_directories(mbase-benchmark-t2t PUBLIC mb_pc mb_inference mb_json)
target_link_libraries(mbase-benchmark-t2t PRIVATE ${MBASE_STD_LIBS} mb_pc mb_inference mb_json llama)
