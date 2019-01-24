cmake_minimum_required(VERSION 3.8)

project(libbech32 VERSION 1.0.0
                 DESCRIPTION "Digital Contract Design's C++ implementation of the bech32 algorithm"
                 LANGUAGES C CXX )

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  set(DCD_CXX_FLAGS -Wall -Wextra -Wpedantic -Wno-unknown-pragmas)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(DCD_CXX_FLAGS -Weverything -Wno-c++98-compat -Wno-padded -Wno-missing-prototypes)
endif()

add_subdirectory(libbech32)

enable_testing()

# Set options so we build googletest and rapidcheck. Other projects that
# embed libbech32 should set these options to OFF
option(LIBBECH32_BUILD_GOOGLETEST "Build googletest" ON)
option(LIBBECH32_BUILD_RAPIDCHECK "Build rapidcheck" ON)
add_subdirectory(test)

add_subdirectory(examples)
