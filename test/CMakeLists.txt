# testing binary
add_executable(fsmomc_test test.cpp)
target_link_libraries(fsmomc_test ${PROJECT_NAME})

set(url https://github.com/philsquared/Catch/releases/download/v2.13.6/catch.hpp)
file(
  DOWNLOAD ${url} "${CMAKE_CURRENT_BINARY_DIR}/catch.hpp"
  STATUS status
  EXPECTED_HASH SHA256=681e7505a50887c9085539e5135794fc8f66d8e5de28eadf13a30978627b0f47)
list(GET status 0 error)
if(error)
  message(FATAL_ERROR "Could not download ${url}")
endif()
target_include_directories(fsmomc_test PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/include
)

add_test(
  NAME lib_test
  COMMAND $<TARGET_FILE:fsmomc_test> --success
)