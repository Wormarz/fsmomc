cmake_minimum_required(VERSION 3.16)

project(fsmomc VERSION 0.1.0)

include(cmake/top.cmake)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/source SRC_FILES)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${PROJECT_BINARY_DIR}/include
)

configure_file (
    "${PROJECT_SOURCE_DIR}/include/version.h.in"
    "${PROJECT_BINARY_DIR}/include/version.h"
)

enable_testing()
add_subdirectory(test)
