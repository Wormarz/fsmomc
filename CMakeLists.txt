cmake_minimum_required(VERSION 3.16)

project(fsmomc)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/source source_files)

add_library(${PROJECT_NAME} STATIC ${source_files})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
