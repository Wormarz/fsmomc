cmake_minimum_required(VERSION 3.16)

project(fsmomc VERSION 0.1.0)

if (NOT MAX_SM_NAME_LEN)
    set(MAX_SM_NAME_LEN 16)
endif()

if (NOT MAX_SM_EDGES_NUMS)
    set(MAX_SM_EDGES_NUMS 4)
endif()

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/source SRC_FILES)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${PROJECT_BINARY_DIR}/include
)

configure_file (
    "${PROJECT_SOURCE_DIR}/include/autoconf.h.in"
    "${PROJECT_BINARY_DIR}/include/autoconf.h"
)

# enable_testing()
# add_subdirectory(test)
