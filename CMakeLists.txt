cmake_minimum_required(VERSION 3.8)

project(libbech32 VERSION 1.0.0
                 DESCRIPTION "Digital Contract Design's C++ implementation of the bech32 algorithm"
                 LANGUAGES C CXX )

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  set(DCD_CXX_FLAGS -Wall -Wextra -Wpedantic -Wno-unknown-pragmas)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(DCD_CXX_FLAGS -Weverything -Wno-c++98-compat -Wno-padded -Wno-missing-prototypes)
endif()

# Set option to build tests or not. Other projects that embed libbech32
# may want to set this to OFF
set(LIBBECH32_BUILD_TESTS ON CACHE BOOL "LIBBECH32_BUILD_TESTS")

# Set option to build examples or not. Other projects that embed libbech32
# may want to set this to OFF
set(LIBBECH32_BUILD_EXAMPLES ON CACHE BOOL "LIBBECH32_BUILD_EXAMPLES")

# Set option to install libbech32 library and header file. Other projects that embed libbech32 may want to set this to OFF
option(INSTALL_LIBBECH32 "Enable installation of libbech32. (Projects embedding libbech32 may want to turn this OFF.)" ON)

add_subdirectory(libbech32)

if(LIBBECH32_BUILD_TESTS)
  enable_testing()
  # Set options to build googletest and rapidcheck or not. Other
  # projects that embed libbech32 and already use googletest and/or
  # rapidcheck should set these options to OFF
  option(LIBBECH32_BUILD_GOOGLETEST "Build googletest" ON)
  option(LIBBECH32_BUILD_RAPIDCHECK "Build rapidcheck" ON)
  add_subdirectory(test)
endif()

if(LIBBECH32_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
