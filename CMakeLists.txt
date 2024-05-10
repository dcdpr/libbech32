#
# Possible Input Settings (defined in environment or set on command line)
#
# LIBBECH32_PROJECT : Project name. Default: "libbech32"
#
# LIBBECH32_BUILD_TESTS : Build test executables [ON OFF]. Default: ON.
# LIBBECH32_BUILD_EXAMPLES : Build example executables [ON OFF]. Default: ON.
# INSTALL_LIBBECH32 : Enable installation [ON OFF]. Default: ON.
#
# The test executables use googletest and rapidcheck. Other projects that
# embed libbech32 and already use googletest and/or rapidcheck could set
# these options to OFF
# LIBBECH32_BUILD_GOOGLETEST : Build googletest for testing [ON OFF]. Default: ON if LIBBECH32_BUILD_TESTS is ON.
# LIBBECH32_BUILD_RAPIDCHECK : Build rapidcheck for testing [ON OFF]. Default: ON if LIBBECH32_BUILD_TESTS is ON.

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Version settings

set(LIBBECH32_VERSION_MAJOR 1)
set(LIBBECH32_VERSION_MINOR 0)
set(LIBBECH32_VERSION_PATCH 2)
set(LIBBECH32_VERSION ${LIBBECH32_VERSION_MAJOR}.${LIBBECH32_VERSION_MINOR}.${LIBBECH32_VERSION_PATCH} CACHE STRING "" FORCE)

# General settings

set(LIBBECH32_PROJECT "libbech32" CACHE STRING "Project Name")

# Test settings

set(LIBBECH32_BUILD_TESTS ON CACHE BOOL "Build test executables")

# Examples settings

set(LIBBECH32_BUILD_EXAMPLES ON CACHE BOOL "Build example executables")

# Install

set(INSTALL_LIBBECH32 ON CACHE BOOL "Enable installation")

# Set default install prefix if we are top-level project, but only
# if it hasn't been set on the command line
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  if(NOT WIN32 AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CMAKE_INSTALL_PREFIX "/opt/contract.design/libbech32" CACHE PATH "..." FORCE)
  endif()
endif()

# Project

project(${LIBBECH32_PROJECT} VERSION ${LIBBECH32_VERSION}
                 DESCRIPTION "Digital Contract Design's C++ implementation of the bech32 algorithm"
                 LANGUAGES C CXX )

# Log settings

message(STATUS "---------------- LIBBECH32 OPTIONS ----------------")

message(STATUS "LIBBECH32_PROJECT            : " ${LIBBECH32_PROJECT})
message(STATUS "LIBBECH32_VERSION            : " ${LIBBECH32_VERSION})

message(STATUS "LIBBECH32_BUILD_TESTS        : " ${LIBBECH32_BUILD_TESTS})
message(STATUS "LIBBECH32_BUILD_EXAMPLES     : " ${LIBBECH32_BUILD_EXAMPLES})

message(STATUS "INSTALL_LIBBECH32            : " ${INSTALL_LIBBECH32})

message(STATUS "----------------------------------------------------")


include(GNUInstallDirs)

add_subdirectory(libbech32)

if(LIBBECH32_BUILD_TESTS)
  enable_testing()
  # Set options to build googletest and rapidcheck or not. Other
  # projects that embed libbech32 and already use googletest and/or
  # rapidcheck could set these options to OFF
  option(LIBBECH32_BUILD_GOOGLETEST "Build googletest" ON)
  option(LIBBECH32_BUILD_RAPIDCHECK "Build rapidcheck" ON)
  add_subdirectory(test)
endif()

if(LIBBECH32_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
