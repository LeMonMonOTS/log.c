cmake_minimum_required(VERSION 3.10)

add_library(log SHARED
    src/log.c
)

target_compile_features(log PRIVATE c_std_90)

target_include_directories(log
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(log PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
