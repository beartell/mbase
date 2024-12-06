include_guard(GLOBAL)

include(MBASECommonConfig)
include(MBASEStdSys)

set(MBASE_JSON_SYS_STRING "MBASE Json")
set(MBASE_JSON_LIB_NAME "json")

mbase_build_version(1 0 0 ${MBASE_VERSION_STABLE} MBASE_JSON_VERSION)
mbase_build_include_install_path(${MBASE_JSON_LIB_NAME} MBASE_JSON_INCLUDE_INSTALL_PATH)
mbase_build_lib_path(${MBASE_JSON_LIB_NAME} MBASE_JSON_LIB_PATH)

list(APPEND MBASE_JSON_INCLUDE_DEPENDS ${MBASE_STD_INCLUDES} ${CMAKE_SOURCE_DIR}/external)

add_library(double-conversion STATIC
    ${CMAKE_SOURCE_DIR}/external/double-conversion/bignum.cc
    ${CMAKE_SOURCE_DIR}/external/double-conversion/bignum-dtoa.cc
    ${CMAKE_SOURCE_DIR}/external/double-conversion/cached-powers.cc
    ${CMAKE_SOURCE_DIR}/external/double-conversion/double-to-string.cc
    ${CMAKE_SOURCE_DIR}/external/double-conversion/fast-dtoa.cc
    ${CMAKE_SOURCE_DIR}/external/double-conversion/fixed-dtoa.cc
    ${CMAKE_SOURCE_DIR}/external/double-conversion/string-to-double.cc
    ${CMAKE_SOURCE_DIR}/external/double-conversion/strtod.cc
)

set_property(TARGET double-conversion PROPERTY POSITION_INDEPENDENT_CODE ON)

message("JSON Include depends: ${MBASE_JSON_INCLUDE_DEPENDS}")

list(APPEND MBASE_JSON_INCLUDE_STABLE_FILES
    json.h
    jtckdint.h
)

foreach(MB_INCLUDE_FILE IN LISTS MBASE_JSON_INCLUDE_STABLE_FILES)
    list(APPEND MBASE_JSON_INCLUDE_INSTALL_FILES ${MBASE_JSON_INCLUDE_INSTALL_PATH}/${MB_INCLUDE_FILE})
endforeach()

add_library(mb_json SHARED
    ${MBASE_JSON_LIB_PATH}/json.cpp
)



target_compile_definitions(mb_json PRIVATE MBASE_BUILD=1 MBASE_INTERNAL_API=1)
target_include_directories(mb_json PUBLIC ${MBASE_JSON_INCLUDE_DEPENDS})
target_link_libraries(mb_json PRIVATE double-conversion)

set_property(TARGET mb_json PROPERTY POSITION_INDEPENDENT_CODE ON)