add_executable(mbase_simple_conversation simple-conversation.cpp)

target_include_directories(mbase_simple_conversation PUBLIC mb_pc mb_inference ${CMAKE_SOURCE_DIR}/external mb_json)
target_link_libraries(mbase_simple_conversation PRIVATE ${MBASE_STD_LIBS} mb_pc mb_inference mb_json llama)
