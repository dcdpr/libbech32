cmake_minimum_required(VERSION 3.6)

project(btcr_did VERSION 1.0.0
                 DESCRIPTION "Creating and interacting with BTCR DIDs"
                 LANGUAGES CXX )

#if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
#   message(FATAL_ERROR "In-source builds are not allowed.")
#endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  set(DCD_CXX_FLAGS -Wall -Wextra -Wpedantic -Wno-unknown-pragmas)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(DCD_CXX_FLAGS -Weverything -Wno-c++98-compat -Wno-padded -Wno-missing-prototypes)
endif()

add_subdirectory(external/anyoption)
add_subdirectory(libbech32)
add_subdirectory(libtxref)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)
add_subdirectory(integration)

