cmake_minimum_required(VERSION 3.8)

project(jsoncpp
    VERSION 1.8.4
    LANGUAGES CXX)

set(jsoncpp_SRCS
  jsoncpp.cpp
  )

set(jsoncpp_HDRS
  json/json-forwards.h
  json/json.h
  )

include_directories(
  .
  )

add_library(jsoncpp SHARED ${jsoncpp_SRCS} ${jsoncpp_HDRS})

target_include_directories(jsoncpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_compile_features(jsoncpp PRIVATE cxx_std_11)
set_target_properties(jsoncpp PROPERTIES CXX_EXTENSIONS OFF)
