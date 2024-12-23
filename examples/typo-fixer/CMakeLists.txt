add_executable(mbase_typo_fixer typo-fixer.cpp)

target_compile_definitions(mbase_typo_fixer PRIVATE ${MBASE_COMMON_COMPILE_DEFINITIONS})
target_compile_options(mbase_typo_fixer PRIVATE ${MBASE_COMMON_COMPILE_OPTIONS})
target_include_directories(mbase_typo_fixer PUBLIC mb_pc mb_inference)
target_link_libraries(mbase_typo_fixer PRIVATE ${MBASE_STD_LIBS} mb_pc mb_inference llama)
