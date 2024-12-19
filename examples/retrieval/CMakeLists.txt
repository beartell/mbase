add_executable(mbase_retrieval retrieval.cpp)

target_include_directories(mbase_retrieval PUBLIC mb_inference)
target_link_libraries(mbase_retrieval PRIVATE ${MBASE_STD_LIBS} mb_inference)
