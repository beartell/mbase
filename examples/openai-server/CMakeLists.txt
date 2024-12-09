add_executable(mbase_openai_server openai-server.cpp)

target_include_directories(mbase_openai_server PUBLIC mb_pc mb_inference ${CMAKE_SOURCE_DIR}/external mb_json)
target_link_libraries(mbase_openai_server PRIVATE ${MBASE_STD_LIBS} mb_pc mb_inference mb_json llama)
