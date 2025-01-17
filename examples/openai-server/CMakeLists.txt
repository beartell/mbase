add_executable(mbase_openai_server main.cpp)

# Options are commented out because cpp-httplib can't get compiled for some reason

add_library(
    openailib STATIC
    model.cpp
    processor.cpp
    client.cpp
)

target_compile_definitions(openailib PUBLIC ${MBASE_COMMON_COMPILE_DEFINITIONS})
target_include_directories(openailib PUBLIC mb_pc mb_inference ${CMAKE_SOURCE_DIR}/external mb_json)
target_link_libraries(openailib PUBLIC ${MBASE_STD_LIBS} mb_pc mb_inference mb_json)

target_compile_definitions(mbase_openai_server PRIVATE openailib)
#target_compile_options(mbase_openai_server PRIVATE ${MBASE_COMMON_COMPILE_OPTIONS})
target_include_directories(mbase_openai_server PUBLIC openailib)
target_link_libraries(mbase_openai_server PRIVATE openailib)
