add_executable(mbase_openai_server main.cpp)

add_library(
    openailib STATIC
    model.cpp
    processor.cpp
    client.cpp
)

if (WIN32)
    set(OPENSSL_MSVC_STATIC_RT "TRUE")
endif()

set(OPENSSL_USE_STATIC_LIBS "TRUE")

if(MBASE_SERVER_SSL STREQUAL "ON")
    find_package(OpenSSL REQUIRED)
    target_compile_definitions(openailib PUBLIC ${MBASE_COMMON_COMPILE_DEFINITIONS} CPPHTTPLIB_OPENSSL_SUPPORT)
else()
    target_compile_definitions(openailib PUBLIC ${MBASE_COMMON_COMPILE_DEFINITIONS})
endif()

# disclaimer here: even if the openssl is missing, those are empty strings and won't cause a problem at configuration and linking
target_include_directories(openailib PUBLIC mb_pc mb_inference ${CMAKE_SOURCE_DIR}/external mb_json ${OPENSSL_INCLUDE_DIR})
target_link_libraries(openailib PUBLIC ${MBASE_STD_LIBS} mb_pc mb_inference mb_json ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})

target_compile_definitions(mbase_openai_server PRIVATE openailib)
target_include_directories(mbase_openai_server PUBLIC openailib)
target_link_libraries(mbase_openai_server PRIVATE openailib)

install(TARGETS mbase_openai_server RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})