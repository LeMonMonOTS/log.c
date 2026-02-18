cmake_minimum_required(VERSION 3.10)

add_library(log SHARED
    src/log.c
)

set_target_properties(log PROPERTIES C_STANDARD 90 C_STANDARD_REQUIRED ON)

# To enable log color
target_compile_definitions(log PUBLIC LOG_USE_COLOR)

target_include_directories(log
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(log PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
