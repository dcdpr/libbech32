cmake_minimum_required(VERSION 3.6)
project(txref)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS NO)

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  set(DCD_CXX_FLAGS "-Wall -Wextra -Wpedantic")
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(DCD_CXX_FLAGS "-Weverything -Wno-c++98-compat")
endif()

add_subdirectory(anyoption)
add_subdirectory(libbech32)
add_subdirectory(libtxref)
add_subdirectory(src)
add_subdirectory(test)

