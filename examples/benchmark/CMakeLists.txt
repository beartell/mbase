add_executable(mbase_benchmark_t2t benchmark.cpp)

target_compile_definitions(mbase_benchmark_t2t PRIVATE ${MBASE_COMMON_COMPILE_DEFINITIONS})
target_compile_options(mbase_benchmark_t2t PRIVATE ${MBASE_COMMON_COMPILE_OPTIONS})
target_include_directories(mbase_benchmark_t2t PUBLIC mb_pc mb_inference mb_json)
target_link_libraries(mbase_benchmark_t2t PRIVATE ${MBASE_STD_LIBS} mb_pc mb_inference mb_json llama)

install(TARGETS mbase_benchmark_t2t RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})