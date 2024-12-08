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
set(TEST_PARTS version.cpp init.cpp add_state.cpp add_rules.cpp tans_state.cpp)

add_executable(fsmomc_tests test_main.cpp ${TEST_PARTS})
target_compile_options(fsmomc_tests
PUBLIC
    -g -fsanitize=address -fno-omit-frame-pointer
)
target_link_libraries(fsmomc_tests ${PROJECT_NAME} asan)

target_include_directories(fsmomc_tests PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/include
)

add_test( NAME fsmomc_tests
  COMMAND $<TARGET_FILE:fsmomc_tests> --success
)
