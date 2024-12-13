add_executable(mbase-benchmark-t2t benchmark.cpp)

target_include_directories(mbase-benchmark-t2t PUBLIC mb_pc mb_inference)
target_link_libraries(mbase-benchmark-t2t PRIVATE ${MBASE_STD_LIBS} mb_pc mb_inference llama)
