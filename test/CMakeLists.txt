# testing binary

set(url https://github.com/philsquared/Catch/releases/download/v2.13.6/catch.hpp)
file(
  DOWNLOAD ${url} "${CMAKE_CURRENT_BINARY_DIR}/catch.hpp"
  STATUS status
  EXPECTED_HASH SHA256=681e7505a50887c9085539e5135794fc8f66d8e5de28eadf13a30978627b0f47)
list(GET status 0 error)
if(error)
  message(FATAL_ERROR "Could not download ${url}")
endif()

set(CTEST_MEMORYCHECK_TYPE "AddressSanitizer")
set(CTEST_MEMORYCHECK_SANITIZER_OPTIONS "verbosity=1:symbolize=1:abort_on_error=1:detect_leaks=1")

# version test
add_executable(version version.cpp)
target_compile_options(version
PUBLIC
    -g -fsanitize=address -fno-omit-frame-pointer
)
target_link_libraries(version ${PROJECT_NAME} asan)

target_include_directories(version PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/include
)

add_test( NAME version_test
  COMMAND $<TARGET_FILE:version> --success
)

# initialization test
add_executable(init init.cpp)
target_compile_options(init
PUBLIC
    -g -fsanitize=address -fno-omit-frame-pointer
)
target_link_libraries(init ${PROJECT_NAME} asan)

target_include_directories(init PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/include
)

add_test( NAME init_test
  COMMAND $<TARGET_FILE:init> --success
)

# add states test
add_executable(add_state add_state.cpp)
target_compile_options(add_state
PUBLIC
    -g -fsanitize=address -fno-omit-frame-pointer
)
target_link_libraries(add_state ${PROJECT_NAME} asan)

target_include_directories(add_state PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/include
)

add_test( NAME add_state_test
  COMMAND $<TARGET_FILE:add_state> --success
)