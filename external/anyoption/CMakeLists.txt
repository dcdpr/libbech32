cmake_minimum_required(VERSION 3.8)

project(anyoption
    VERSION 0.0.1
    LANGUAGES CXX)

set(anyoption_SRCS
  anyoption.cpp
  )

set(anyoption_HDRS
  anyoption.h
  )

include_directories(
  .
  )

add_library(anyoption SHARED ${anyoption_SRCS} ${anyoption_HDRS})

target_include_directories(anyoption
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
