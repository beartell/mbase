add_executable(mbase_openai_server openai-server.cpp)

# Options are commented out because cpp-httplib can't get compiled for some reason

target_compile_definitions(mbase_openai_server PRIVATE ${MBASE_COMMON_COMPILE_DEFINITIONS})
#target_compile_options(mbase_openai_server PRIVATE ${MBASE_COMMON_COMPILE_OPTIONS})
target_include_directories(mbase_openai_server PUBLIC mb_pc mb_inference ${CMAKE_SOURCE_DIR}/external mb_json)
target_link_libraries(mbase_openai_server PRIVATE ${MBASE_STD_LIBS} mb_pc mb_inference mb_json llama)
